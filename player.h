#ifndef PLAYER_H
#define PLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QString>

class Player : public QThread
{   Q_OBJECT
private:
    QMutex mutex;
    QWaitCondition condition;
    cv::VideoCapture capture;
    cv::Mat frame;
    cv::Mat RGBframe;
    QImage img;
    int frameRate;
    bool stopped;
protected:
    void run();
    void msleep(int ms);
public:
    Player(QObject *parent  = 0);
    ~Player();
    bool loadVideo(std::string filename);
    bool useWebcam();
    void play();
    void stop();
    bool isStopped();
    cv::Mat getCurrentFrame();
signals:
    void processedFrame(cv::Mat frame);
    void processedImage(const QImage &image);
};

#endif // PLAYER_H
