#include "player.h"
#include <QDebug>

Player::Player(QObject *parent) : QThread(parent), stopped(true)
{}

bool Player::loadVideo(std::string filename) {
    if (capture.isOpened()) {
        return false;
    }

    capture.open(filename);
    if (capture.isOpened())
    {
        frameRate = (int) capture.get(CV_CAP_PROP_FPS);
        qDebug() << "framerate" << frameRate;
        return true;
    }
    else
        return false;
}

bool Player::useWebcam()
{
    if (capture.isOpened()) {
        return false;
    }

    capture.open(0);
    if (capture.isOpened())
    {
        frameRate = (int) capture.get(CV_CAP_PROP_FPS);
        qDebug() << "framerate" << frameRate;
        return true;
    }
    else
        return false;
}

void Player::play()
{
    if (!isRunning()) {
        if (isStopped()){
            stopped = false;
        }
        start(LowPriority);
    }
}

void Player::run()
{
    int delay = (1000/frameRate);
    while(!stopped){
        if (!capture.read(frame))
        {
            stopped = true;
            qDebug() << "read empty frame";
            break;
        }
        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows, RGBframe.step, QImage::Format_RGB888);
            emit processedFrame(RGBframe);
        }
        else
        {
            qDebug() << "1-channel video!!";
            img = QImage((const unsigned char*)(frame.data), frame.cols,frame.rows,QImage::Format_Indexed8);
            emit processedFrame(frame);
        }


        emit processedImage(img);
        this->msleep(delay);
    }
}

Player::~Player()
{
    mutex.lock();
    stopped = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void Player::stop() { stopped = true; }


bool Player::isStopped() { return this->stopped; }

cv::Mat Player::getCurrentFrame() { return RGBframe; }

void Player::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}
