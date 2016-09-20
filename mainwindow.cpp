#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <vector>

using namespace std;

Q_DECLARE_METATYPE(cv::Mat)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    myPlayer = new Player();
    bgSubtractor = new BgSubtractor();
    csTracker = NULL;
    initialized = false;



    connect(myPlayer, SIGNAL(processedImage(QImage)), this, SLOT(updatePlayerUI(QImage)));
    qRegisterMetaType< cv::Mat >("cv::Mat");
    connect(myPlayer, SIGNAL(processedFrame(cv::Mat)), this, SLOT(processFrame(cv::Mat)));



    ui->setupUi(this);
    connect(ui->label_input, SIGNAL(selected(QRect)), this, SLOT(initCamshiftTracker(QRect)));
    connect(ui->label_input, SIGNAL(selected(QRect)), this, SLOT(initPFTracker(QRect)));
}


void MainWindow::processFrame(Mat frame)
{
    qDebug() << "elapsed time" << timer.restart();
    Scalar color_blue( 0, 0, 255 );
    Scalar color_red(255, 0 , 0);
    Scalar color_yellow(255, 255, 0);
    bgSubtractor->processFrame(frame);

    if (!initialized) {
        track = cv::Mat::zeros(frame.size(), frame.type());

        initialized = true;
    }

    cv::Mat morph_mask= *bgSubtractor->getMorphMask();
    cv::Mat morph_open = bgSubtractor->mask_open;
    cv::Mat morph_close = bgSubtractor->mask_close;

    mask = *bgSubtractor->getMask();

    usingAlgCombination = true;
    if (csTracker) {


        if (usingAlgCombination) {
            csTracker->processFrame(frame, mask);
        } else {
            csTracker->processFrame(frame);
        }

        cv::Mat roi = csTracker->getRoi();
        cv::Mat cs_mask_roi = csTracker->mask_roi;
        cv::Mat backproj = csTracker->getBackProjection();
        cv::Mat heatmap = csTracker->getHeatmap();



        cv::Mat topright_mat = heatmap.clone();

        //draw track
        circle(track, csTracker->getPosition(), 1, color_yellow);

        cv::Rect csBoundRect = csTracker->getBoundingRect();
        cv::RotatedRect rotatedRect = csTracker->getRotatedRect();

        cvtColor( topright_mat, topright_mat, COLOR_GRAY2RGB);

        if (ui->track_checkbox->isChecked()) {
            addWeighted(frame, 0.5, track, 0.5, 0, topright_mat);
        }
        rectangle(topright_mat, csBoundRect, color_blue, 2);
        ellipse(topright_mat, rotatedRect, color_red, 2, cv::LINE_AA);

        QImage topright_img = QImage((uchar*) topright_mat.data, topright_mat.cols, topright_mat.rows, topright_mat.step, QImage::Format_RGB888);
        QPixmap topright_pix = QPixmap::fromImage(topright_img);
        ui->label_topright->setPixmap(topright_pix.scaled(ui->label_topright->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));


        bool pf_tracker_enabled = true;
        if (pf_tracker_enabled) {
            pfTracker->processFrame(frame);
            cv::Mat pf_mat = myPlayer->getCurrentFrame().clone();
            std::vector<Particle> particle_set = pfTracker->particle_set;
            for(std::vector<Particle>::const_iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
                Particle p = *i;
                circle(pf_mat, cv::Point(p.x, p.y), 1, color_red, -1);
            }

            QImage pf_img = QImage((uchar*) pf_mat.data, pf_mat.cols, pf_mat.rows, pf_mat.step, QImage::Format_RGB888);
            QPixmap pf_pix = QPixmap::fromImage(pf_img);
            ui->label_down->setPixmap(pf_pix.scaled(ui->label_down->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
        }

    }

//    QImage morph_img = QImage((uchar*) morph_mask.data, morph_mask.cols, morph_mask.rows, morph_mask.step, QImage::Format_Grayscale8);
//    QPixmap morph_pix = QPixmap::fromImage(morph_img);
//    QImage contour_img = QImage((uchar*) dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);
//    QPixmap contour_pix = QPixmap::fromImage(contour_img);

//    ui->label_bottomright->setPixmap(contour_pix.scaled(ui->label_bottomright->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

void MainWindow::initCamshiftTracker(QRect rect)
{
    qDebug() << "INIT CSTracker";
    cv::Mat curFrame = myPlayer->getCurrentFrame();

    float xScale = (float) curFrame.cols / ui->label_input->width();
    float yScale = (float) curFrame.rows / ui->label_input->height();

    qDebug() << curFrame.cols << curFrame.rows;
    qDebug() << ui->label_input->size();
    qDebug() << xScale << yScale;

    cv::Rect targetRect(rect.x() * xScale, rect.y() * yScale, rect.width() * xScale, rect.height() * yScale);
    csTracker = new CamShiftTracker(curFrame, targetRect);
}

void MainWindow::initPFTracker(QRect rect)
{
    qDebug() << "INIT PFTracker";
    cv::Mat curFrame = myPlayer->getCurrentFrame();

    float xScale = (float) curFrame.cols / ui->label_input->width();
    float yScale = (float) curFrame.rows / ui->label_input->height();

    cv::Rect targetRect(rect.x() * xScale, rect.y() * yScale, rect.width() * xScale, rect.height() * yScale);
    pfTracker = new ParticleFilterTracker();
    pfTracker->init(curFrame, targetRect);

}

void MainWindow::updatePlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->label_input->setAlignment(Qt::AlignCenter);
        ui->label_input->setPixmap(QPixmap::fromImage(img).scaled(ui->label_input->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
//        QPixmap pix = QPixmap::fromImage(img);
//        ui->label_input->setPixmap();
    }
}

void MainWindow::on_load_button_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                          tr("Open Video"), ".",
                                          tr("Video Files (*.avi *.mpg *.mp4)"));
    if (!filename.isEmpty()){
        if (!myPlayer->loadVideo(filename.toLatin1().data()))
        {
            QMessageBox msgBox;
            msgBox.setText("The selected video could not be opened!");
            msgBox.exec();
        }
    }
}

void MainWindow::on_play_button_clicked()
{
    if (myPlayer->isStopped())
    {
        qDebug() << "player is stopped. now PLAY!";
        myPlayer->Play();
        ui->play_button->setText(tr("Stop"));
    }else
    {
        myPlayer->Stop();
        ui->play_button->setText(tr("Play"));
    }
}


MainWindow::~MainWindow()
{
    delete myPlayer;
    delete ui;
}


void MainWindow::on_bgs_params_clicked()
{
    bg_params_ui = new ParamsForm();
    bg_params_ui->setWindowTitle("Background subtraction params");
    bg_params_ui->setBgSubtractor(bgSubtractor);
    connect(myPlayer, SIGNAL(processedFrame(cv::Mat)), bg_params_ui, SLOT(updateFrames(cv::Mat)));
    bg_params_ui->show();
}

void MainWindow::on_ms_params_clicked()
{
    cs_params_ui = new MsParamsForm();
    cs_params_ui->setWindowTitle("Camshift params");
    cs_params_ui->setCsTracker(&csTracker);
    cs_params_ui->setBgSubtractor(&bgSubtractor
       );
    connect(myPlayer, SIGNAL(processedFrame(cv::Mat)), cs_params_ui, SLOT(updateFrames()));
    cs_params_ui->show();

}
