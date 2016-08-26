#ifndef OBJECTTRACKER_H
#define OBJECTTRACKER_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;

class ObjectTracker
{
public:
    virtual void processFrame(const cv::Mat& frame) = 0;
    virtual cv::Point getPosition() = 0;
    virtual cv::Rect getBoundingRect() = 0;
};

#endif // OBJECTTRACKER_H
