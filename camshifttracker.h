#ifndef CAMSHIFTTRACKER_H
#define CAMSHIFTTRACKER_H

#include "objecttracker.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <QDebug>
#include <vector>

using namespace cv;

class CamShiftTracker: public ObjectTracker
{
public:
    CamShiftTracker(cv::Mat frame, cv::Rect roi_rect);
    void processFrame(const cv::Mat& frame);
    void processFrame(const cv::Mat& frame, cv::Mat bg_mask);

    cv::Rect getBoundingRect();
    cv::RotatedRect getRotatedRect();
    cv::Mat getBackProjection();

    cv::Point getPosition();
    cv::Mat getRoi();
    cv::Mat getHeatmap();

    void setHbins(int);
    void setSbins(int);
    void setVbins(int);
    void setAlpha(double);
    void setThreshold(int);

    cv::Mat mask, mask_roi, hue_roi, roi_hist2;
private:
    void updateHist(cv::Mat roi);
    cv::Rect trackWindow;
    cv::RotatedRect rotatedBox;
    cv::Mat roi_hist, roi;
    cv::Mat hsv_frame, backproj, hue_sat;
    cv::Mat heatmap;
    double alpha = 0.3;
    double sigma;
    int hsize = 16;
    int hbins = 10, sbins = 10, vbins = 10;
    Scalar lowThresh = Scalar(0, 0, 0);
    Scalar highThresh = Scalar(180, 255, 255);
    int thresh = 100;
    TermCriteria termCrit;

    int mixCh[6] = {0, 0, 1, 1, 2, 2};
    int channels[3] = {0, 1, 2};
    float hranges[2] = { 0, 180 };
    float sranges[2] = { 0, 256 };
    float vranges[2] = { 0, 256 };
    int histSize[3] = {hbins, sbins, vbins};
    const float* ranges[3] = { hranges, sranges, vranges};
};


#endif // CAMSHIFTTRACKER_H
