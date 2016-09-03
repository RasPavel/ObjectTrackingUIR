#ifndef BGSUBTRACTOR_H
#define BGSUBTRACTOR_H

#include "objecttracker.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <QDebug>
#include <vector>

class BgSubtractor: public ObjectTracker
{
public:
    BgSubtractor();
    void processFrame(const cv::Mat&);
    cv::Mat* getMask();
    cv::Rect getBoundingRect();
    cv::Point getPosition();
    cv::Mat* getMorphMask();
    int getMixtureCount();
    void setHistory(int);
    void setVarThreshold(int);
    void setOpenElementSize(int);
    void setCloseElementSize(int);
    cv::Mat mask_open, mask_close;
private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor;
    cv::Mat mask;
    cv::Mat morph_mask;
    int closeElementSize, openElementSize;
};

#endif // BGSUBTRACTOR_H
