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
    cv::Mat* getMorphMask();
    int getMixtureCount();
private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor;
    cv::Mat mask;
    cv::Mat morph_mask;
};

#endif // BGSUBTRACTOR_H
