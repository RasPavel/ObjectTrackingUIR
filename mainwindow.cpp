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
    msTracker = NULL;
    connect(myPlayer, SIGNAL(processedImage(QImage)), this, SLOT(updatePlayerUI(QImage)));
    qRegisterMetaType< cv::Mat >("cv::Mat");
    connect(myPlayer, SIGNAL(processedFrame(cv::Mat)), this, SLOT(processFrame(cv::Mat)));
    ui->setupUi(this);
    connect(ui->label_input, SIGNAL(selected(QRect)), this, SLOT(initMeanshiftTracker(QRect)));
}


void MainWindow::processFrame(Mat frame)
{
    Scalar color_blue( 0, 0, 255 );
    Scalar color_red(255, 0 , 0);


    if (msTracker) {
        cv::Mat roi = msTracker->getRoi();
        cv::Mat ms_mask_roi = msTracker->mask_roi;
        cv::Mat bp = msTracker->getBackProjection();

        cv::Mat botdown_mat = bp;

        QImage botdown_img = QImage((uchar*) botdown_mat.data, botdown_mat.cols, botdown_mat.rows, botdown_mat.step, QImage::Format_Grayscale8);
        QPixmap botdown_pix = QPixmap::fromImage(botdown_img);
        ui->label_bottom_down->setPixmap(botdown_pix.scaled(ui->label_bottom_down->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));


//        qDebug() << "mean shift is active";
        msTracker->processFrame(frame);
        backproj = msTracker->getBackProjection();

        cv::Rect msBounRect = msTracker->getBoundingRect();
//        cvtColor( backproj, backproj, COLOR_GRAY2RGB);
//        rectangle(backproj, msBounRect, color_blue);


        QImage backproj_img = QImage((uchar*) backproj.data, backproj.cols, backproj.rows, backproj.step, QImage::Format_Grayscale8);
        QPixmap backproj_pix = QPixmap::fromImage(backproj_img);
        ui->label_mog->setPixmap(backproj_pix.scaled(ui->label_mog->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    } else {
//        qDebug() << "mean shift is not active";
    }

    bgSubtractor->processFrame(frame);

    mask = *bgSubtractor->getMask();
    cv::Mat morph_mask= *bgSubtractor->getMorphMask();






    QImage bg_img = QImage((uchar*) mask.data, mask.cols, mask.rows, mask.step, QImage::Format_Grayscale8);
    QPixmap bg_pix = QPixmap::fromImage(bg_img);

    int N = bgSubtractor->getMixtureCount();
//    qDebug() << N << "mixtures";



    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;    

    Mat dst = Mat::zeros(frame.size(), CV_8UC3);











    cv::Rect boundRect = bgSubtractor->getBoundingRect();
    cv::Point position = bgSubtractor->getPosition();
    rectangle( dst, boundRect.tl(), boundRect.br(), color_blue, 2, 8, 0 );

    ellipse(dst, position, cv::Size(3,3), 0, 0, 360, color_red, 8);

    QImage morph_img = QImage((uchar*) morph_mask.data, morph_mask.cols, morph_mask.rows, morph_mask.step, QImage::Format_Grayscale8);
    QPixmap morph_pix = QPixmap::fromImage(morph_img);
    QImage contour_img = QImage((uchar*) dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);
    QPixmap contour_pix = QPixmap::fromImage(contour_img);

    ui->label_down->setPixmap(contour_pix.scaled(ui->label_down->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
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
