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
    if (!msTracker) {
        msTracker = new MeanShiftTracker(frame, cv::Rect(0,0,10,10));
    }
    bgSubtractor->processFrame(frame);
    mask = *bgSubtractor->getMask();
    QImage bg_img = QImage((uchar*) mask.data, mask.cols, mask.rows, mask.step, QImage::Format_Grayscale8);
    QPixmap bg_pix = QPixmap::fromImage(bg_img);

    int N = bgSubtractor->getMixtureCount();
    qDebug() << N << "mixtures";


    //morph opening
    int morph_size = 2;
    Mat element = getStructuringElement( MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
    Mat frame_morph;
    morphologyEx(mask, frame_morph, cv::MORPH_OPEN, element);




    int niters = 3;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat temp;
    Mat dst;
    dilate(mask, temp, Mat(), Point(-1,-1), niters);
    erode(temp, temp, Mat(), Point(-1,-1), niters*2);
    dilate(temp, temp, Mat(), Point(-1,-1), niters);

    dst = Mat::zeros(frame.size(), CV_8UC3);


    Scalar color_blue( 0, 0, 255 );
    Scalar color_red(255, 0 , 0);
    cv::Rect boundRect = bgSubtractor->getBoundingRect();
    cv::Point position = bgSubtractor->getPosition();
    rectangle( dst, boundRect.tl(), boundRect.br(), color_blue, 2, 8, 0 );

    ellipse(dst, position, cv::Size(3,3), 0, 0, 360, color_red, 8);

    QImage morph_img = QImage((uchar*) frame_morph.data, frame_morph.cols, frame_morph.rows, frame_morph.step, QImage::Format_Grayscale8);
    QPixmap morph_pix = QPixmap::fromImage(morph_img);
    QImage contour_img = QImage((uchar*) dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);
    QPixmap contour_pix = QPixmap::fromImage(contour_img);

    ui->label_mog->setPixmap(bg_pix.scaled(ui->label_mog->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
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
