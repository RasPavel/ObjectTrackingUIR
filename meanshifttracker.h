#ifndef MEANSHIFTTRACKER_H
#define MEANSHIFTTRACKER_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <QDebug>
#include <vector>

using namespace cv;

class MeanShiftTracker
{
public:
    MeanShiftTracker(cv::Mat frame, cv::Rect roi_rect);
    void processFrame(const cv::Mat&);
    const cv::Rect& getBoundingRect();
    cv::Point getPosition();
    cv::Mat getBackProjection();
//    void update_hist(cv::Mat roi);
    cv::Mat getRoi();
    cv::Mat mask, mask_roi, hue_roi;
private:
    cv::Rect trackWindow;
    cv::Mat roi_hist, roi;
    cv::Mat hsv_frame, backproj, hue_sat;
    int hsize = 16;
    float hranges[2] = {0,180};
    Scalar lowThresh = Scalar(0, 0, 0);
    Scalar highThresh = Scalar(180, 255, 255);
    TermCriteria termCrit;
};


#endif // MEANSHIFTTRACKER_H
