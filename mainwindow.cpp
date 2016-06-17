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
    QObject::connect(myPlayer, SIGNAL(processedImage(QImage)), this, SLOT(updatePlayerUI(QImage)));
    qRegisterMetaType< cv::Mat >("cv::Mat");
    connect(myPlayer, SIGNAL(processedFrame(cv::Mat)), this, SLOT(processFrame(cv::Mat)));
    ui->setupUi(this);
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

void MainWindow::processFrame(Mat frame)
{
    qDebug() << "mw process frame";
    if (!msTracker) {
        msTracker = new MeanShiftTracker(frame, cv::Rect(0,0,10,10));
    }

    bgSubtractor->processFrame(frame);
    msTracker->processFrame(frame);

    mask = *bgSubtractor->getMask();
    cv::Mat morph_mask= *bgSubtractor->getMorphMask();
    backproj = msTracker->getBackProjection();

    //    bool backprojMode = true;
    //    if( backprojMode )
    Scalar color_blue( 0, 0, 255 );
    Scalar color_red(255, 0 , 0);
    cv::Rect msBounRect = msTracker->getBoundingRect();
    cvtColor( backproj, backproj, COLOR_GRAY2RGB);
    rectangle(backproj, msBounRect, color_blue);
    /*trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,*/
                                                                      //                          trackWindow.x + r, trackWindow.y + r) &
                                                                      //                      Rect(0, 0, cols, rows);
    //   ellipse( image, trackBox, Scalar(0,0,255), 3, LINE_AA );

    QImage backproj_img = QImage((uchar*) backproj.data, backproj.cols, backproj.rows, backproj.step, QImage::Format_RGB888);
    QPixmap backproj_pix = QPixmap::fromImage(backproj_img);

    ui->label_mog->setPixmap(backproj_pix.scaled(ui->label_mog->size(), Qt::KeepAspectRatio, Qt::FastTransformation));


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

    ui->label_down->setPixmap(contour_pix.scaled(ui->label_down->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

void MainWindow::updatePlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->label_input->setAlignment(Qt::AlignCenter);
        ui->label_input->setPixmap(QPixmap::fromImage(img).scaled(ui->label_input->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
//        QPixmap pix = QPixmap::fromImage(img);
//        ui->label_input->setPixmap();
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
