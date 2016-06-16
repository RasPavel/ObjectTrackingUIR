#ifndef BGSUBTRACTOR_H
#define BGSUBTRACTOR_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <QDebug>
#include <vector>

class BgSubtractor
{
public:
    BgSubtractor();
    void processFrame(cv::Mat&);
    cv::Mat* getMask();
    cv::Rect getBoundingRect();
    cv::Point getPosition();
    int getMixtureCount();
private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor;
    cv::Mat fgMask;
};

#endif // BGSUBTRACTOR_H
