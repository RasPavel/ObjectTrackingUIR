#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <vector>
#include <QTime>
using namespace std;

Q_DECLARE_METATYPE(cv::Mat)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    myPlayer = new Player();
    bgSubtractor = new BgSubtractor();
    msTracker = NULL;
    initialized = false;



    connect(myPlayer, SIGNAL(processedImage(QImage)), this, SLOT(updatePlayerUI(QImage)));
    qRegisterMetaType< cv::Mat >("cv::Mat");
    connect(myPlayer, SIGNAL(processedFrame(cv::Mat)), this, SLOT(processFrame(cv::Mat)));

    ui->setupUi(this);
    connect(ui->label_input, SIGNAL(selected(QRect)), this, SLOT(initMeanshiftTracker(QRect)));
}


void MainWindow::processFrame(Mat frame)
{
    QTime time = QTime::currentTime();

    qDebug() << time.msec();
    Scalar color_blue( 0, 0, 255 );
    Scalar color_red(255, 0 , 0);
    Scalar color_yellow(0, 255, 255);
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
    if (msTracker) {


        if (usingAlgCombination) {
            msTracker->processFrame(frame, mask);
        } else {
            msTracker->processFrame(frame);
        }

        cv::Mat roi = msTracker->getRoi();
        cv::Mat ms_mask_roi = msTracker->mask_roi;
        cv::Mat backproj = msTracker->getBackProjection();
        cv::Mat heatmap = msTracker->getHeatmap();



        //draw track
        circle(track, msTracker->getPosition(), 1, color_yellow);

        cv::Rect msBoundRect = msTracker->getBoundingRect();
        cvtColor( backproj, backproj, COLOR_GRAY2RGB);
        rectangle(backproj, msBoundRect, color_blue);

        cv::Mat topright_mat = backproj;
        cv::Mat bottomleft_mat = morph_close;
        cv::Mat bottomright_mat = heatmap;

        if (ui->track_checkbox->isChecked()) {
            addWeighted(backproj, 0.5, track, 0.5, 0, topright_mat);
        }

        QImage topright_img = QImage((uchar*) topright_mat.data, topright_mat.cols, topright_mat.rows, topright_mat.step, QImage::Format_RGB888);
        QPixmap topright_pix = QPixmap::fromImage(topright_img);
        ui->label_topright->setPixmap(topright_pix.scaled(ui->label_topright->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }

//    cv::Point final_position = msTracker->getPosition();
//    circle(track, final_position, 1, color_red);





    QImage bg_img = QImage((uchar*) morph_close.data, morph_close.cols, morph_close.rows, morph_close.step, QImage::Format_Grayscale8);
    QPixmap bg_pix = QPixmap::fromImage(bg_img);

    QImage open_img = QImage((uchar*) morph_open.data, morph_open.cols, morph_open.rows, morph_open.step, QImage::Format_Grayscale8);
    QPixmap open_pix = QPixmap::fromImage(open_img);

    QImage close_img = QImage((uchar*) morph_close.data, morph_close.cols, morph_close.rows, morph_close.step, QImage::Format_Grayscale8);
    QPixmap close_pix = QPixmap::fromImage(close_img);

    int N = bgSubtractor->getMixtureCount();
//    qDebug() << N << "mixtures";


    Mat dst = Mat::zeros(frame.size(), CV_8UC3);



    cv::Rect boundRect = bgSubtractor->getBoundingRect();
    cv::Point position = bgSubtractor->getPosition();
    rectangle( dst, boundRect.tl(), boundRect.br(), color_blue, 2, 8, 0 );

    ellipse(dst, position, cv::Size(3,3), 0, 0, 360, color_red, 8);

    QImage morph_img = QImage((uchar*) morph_mask.data, morph_mask.cols, morph_mask.rows, morph_mask.step, QImage::Format_Grayscale8);
    QPixmap morph_pix = QPixmap::fromImage(morph_img);
    QImage contour_img = QImage((uchar*) dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);
    QPixmap contour_pix = QPixmap::fromImage(contour_img);

//    ui->label_bottomright->setPixmap(contour_pix.scaled(ui->label_bottomright->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

void MainWindow::initMeanshiftTracker(QRect rect)
{
    qDebug() << "INIT MSTracker";
    cv::Mat curFrame = myPlayer->getCurrentFrame();

    float xScale = (float) curFrame.cols / ui->label_input->width();
    float yScale = (float) curFrame.rows / ui->label_input->height();

    qDebug() << curFrame.cols << curFrame.rows;
    qDebug() << ui->label_input->size();
    qDebug() << xScale << yScale;

    cv::Rect targetRect(rect.x() * xScale, rect.y() * yScale, rect.width() * xScale, rect.height() * yScale);
    msTracker = new MeanShiftTracker(curFrame, targetRect);
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
