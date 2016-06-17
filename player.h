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

using namespace cv;
class Player : public QThread
{   Q_OBJECT
private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    Mat frame;
    int frameRate;
    VideoCapture capture;
    Mat RGBframe;
    QImage img;
protected:
    void run();
    void msleep(int ms);
public:
    Player(QObject *parent  = 0);
    ~Player();
    bool loadVideo(std::string filename);
    void Play();
    void Stop();
    bool isStopped() const;
    Mat getCurrentFrame();
signals:
    void processedFrame(cv::Mat frame);
    void processedImage(const QImage &image);
};

#endif // PLAYER_H
