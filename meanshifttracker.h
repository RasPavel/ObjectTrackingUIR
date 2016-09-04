#ifndef MEANSHIFTTRACKER_H
#define MEANSHIFTTRACKER_H

#include "objecttracker.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <QDebug>
#include <vector>

using namespace cv;

class MeanShiftTracker: public ObjectTracker
{
public:
    MeanShiftTracker(cv::Mat frame, cv::Rect roi_rect);
    void processFrame(const cv::Mat& frame);
    void processFrame(const cv::Mat& frame, cv::Mat bg_mask);

    cv::Rect getBoundingRect();
    cv::Mat getBackProjection();


    cv::Point getPosition();
    cv::Mat getRoi();
    cv::Mat getHeatmap();

    void setHbins(int);
    void setSbins(int);
    void setVbins(int);
    void setAlpha(double);


    cv::Mat mask, mask_roi, hue_roi, roi_hist2;
private:
    void updateHist(cv::Mat roi);
    cv::Rect trackWindow;
    cv::Mat roi_hist, roi;
    cv::Mat hsv_frame, backproj, hue_sat;
    cv::Mat heatmap;
    double alpha = 0.5;
    int hsize = 16;
    int hbins = 10, sbins = 10, vbins = 10;
    float hranges[2] = {0,180};
    Scalar lowThresh = Scalar(0, 0, 0);
    Scalar highThresh = Scalar(180, 255, 255);
    TermCriteria termCrit;
};


#endif // MEANSHIFTTRACKER_H
