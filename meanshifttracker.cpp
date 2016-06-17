#include "meanshifttracker.h"

MeanShiftTracker::MeanShiftTracker(cv::Mat frame, cv::Rect roi_rect)
{
    cv::Mat hsv_roi;
    roi = cv::Mat(frame, roi_rect).clone();
    cv::cvtColor(roi, hsv_roi, CV_RGB2HSV);

    int hbins = 18, sbins = 8;
    int histSize[] = {hbins, sbins};

    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    int channels[] = {0, 1};

//    int channels[] = {0, 0};
//    hue_roi.create(hsv_roi.size(), hsv_roi.depth());
//    mixChannels(&hsv_roi, 1, &hue_roi, 1, channels, 1);
//    const float* phranges = hranges;
//    calcHist(&hue_roi, 1, 0, mask_roi, roi_hist, 1, &hsize, &phranges);

    calcHist( &hsv_roi, 1, channels, mask_roi, roi_hist, 2, histSize, ranges, true);
    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);

    qDebug() << countNonZero(mask_roi) << countNonZero(hue_roi) << countNonZero(roi_hist);

    termCrit = TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    trackWindow = roi_rect;
}

void MeanShiftTracker::processFrame(const cv::Mat& frame)
{
    cv::cvtColor(frame, hsv_frame, CV_RGB2HSV);
    inRange(hsv_frame, lowThresh, highThresh, mask);

    int mixCh[] = {0, 0, 1, 1};
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };

//    hue_sat.create(hsv_frame.size(), hsv_frame.depth());
    hue_sat = hsv_frame.clone();
    qDebug() << hsv_frame.depth() << "depth";
    mixChannels(&hsv_frame, 1, &hue_sat, 1, mixCh, 2);

    int channels[] = {0,1};
    calcBackProject(&hue_sat, 1, channels, roi_hist, backproj, ranges);
//    backproj &= mask;




    RotatedRect trackBox = CamShift(backproj, trackWindow, termCrit);
//    if( trackWindow.area() <= 1 )
//    {
//        int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
//        trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
//                          trackWindow.x + r, trackWindow.y + r) &
//                      Rect(0, 0, cols, rows);
//   }


}

const cv::Rect& MeanShiftTracker::getBoundingRect()
{
    return trackWindow;
}

cv::Mat MeanShiftTracker::getBackProjection()
{
    return backproj;
}

cv::Mat MeanShiftTracker::getRoi()
{
    return roi;
}
