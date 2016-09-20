#include "camshifttracker.h"

CamShiftTracker::CamShiftTracker(cv::Mat frame, cv::Rect roi_rect)
{
    cv::Mat hsv_roi;
    roi = cv::Mat(frame, roi_rect).clone();
    cv::cvtColor(roi, hsv_roi, CV_RGB2HSV);

    int histSize[] = {hbins, sbins, vbins};

    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    float vranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges, vranges};
    int channels[] = {0, 1, 2};

    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    calcHist( &hsv_roi, 1, channels, mask_roi, roi_hist, 3, histSize, ranges, true);



    double minVal, maxVal;
    Point minLoc, maxLoc;
//    minMaxLoc(roi_hist, &minVal, &maxVal, &minLoc, &maxLoc );
    qDebug() << "before" << minVal << maxVal;
//    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
//    minMaxLoc(roi_hist, &minVal, &maxVal, &minLoc, &maxLoc );
    qDebug() << "after" << minVal << maxVal;


//    threshold(roi_hist, roi_hist2, 100, 255, CV_THRESH_TOZERO);

    qDebug() << countNonZero(mask_roi) << countNonZero(hue_roi) << countNonZero(roi_hist);

    termCrit = TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    trackWindow = roi_rect;
}

void CamShiftTracker::processFrame(const cv::Mat& frame)
{
    cv::cvtColor(frame, hsv_frame, CV_RGB2HSV);
    inRange(hsv_frame, lowThresh, highThresh, mask);

    int mixCh[] = {0, 0, 1, 1, 2, 2};
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    float vranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges, vranges};

//    hue_sat.create(hsv_frame.size(), hsv_frame.depth());

    hue_sat = hsv_frame.clone();
    qDebug() << hsv_frame.depth() << "depth";
    mixChannels(&hsv_frame, 1, &hue_sat, 1, mixCh, 3);

    int channels[] = {0,1,2};
    calcBackProject(&hue_sat, 1, channels, roi_hist, backproj, ranges);
//    backproj &= mask;




//    MeanShift(backproj, trackWindow, termCrit);
    rotatedBox = CamShift(backproj, trackWindow, termCrit);


//    if( rotatedBox.area() <= 1 )
//    {
//        int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
//        rotatedBox = Rect(rotatedBox.x - r, rotatedBox.y - r,
//                          rotatedBox.x + r, rotatedBox.y + r) &
//                      Rect(0, 0, cols, rows);
//   }

}

void CamShiftTracker::processFrame(const cv::Mat& frame, cv::Mat bg_mask)
{
    cv::cvtColor(frame, hsv_frame, CV_RGB2HSV);
    inRange(hsv_frame, lowThresh, highThresh, mask);

    int mixCh[] = {0, 0, 1, 1, 2, 2};
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    float vranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges, vranges};

//    hue_sat.create(hsv_frame.size(), hsv_frame.depth());

    hue_sat = hsv_frame.clone();
    mixChannels(&hsv_frame, 1, &hue_sat, 1, mixCh, 3);

    int channels[] = {0,1,2};
    calcBackProject(&hsv_frame, 1, channels, roi_hist, backproj, ranges);
//    qDebug() << backproj << bg_mask.size() << backproj.depth() << bg_mask.depth();
    addWeighted(backproj, alpha, bg_mask, 1.0 - alpha, 0, heatmap);
    threshold(heatmap, heatmap, thresh, 0, cv::THRESH_TOZERO);

    //    calcBackProject(&hue_sat, 1, channels, roi_hist2, backproj2, ranges);
//    backproj &= mask;



//    MeanShift(heatmap, trackWindow, termCrit);
    rotatedBox = CamShift(heatmap, trackWindow, termCrit);


//    if( rotatedBox.area() <= 1 )
//    {
//        int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
//        rotatedBox = Rect(rotatedBox.x - r, rotatedBox.y - r,
//                          rotatedBox.x + r, rotatedBox.y + r) &
//                      Rect(0, 0, cols, rows);
//   }

}

void CamShiftTracker::updateHist(cv::Mat roi)
{
    cv::Mat hsv_roi;
    cv::cvtColor(roi, hsv_roi, CV_RGB2HSV);

    int histSize[] = {hbins, sbins, vbins};

    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    float vranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges, vranges};
    int channels[] = {0, 1, 2};

    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    calcHist(&hsv_roi, 1, channels, mask_roi, roi_hist, 3, histSize, ranges, true);
}

void CamShiftTracker::setHbins(int h) {
    hbins = h;
    updateHist(roi);
}


void CamShiftTracker::setSbins(int s) {
    sbins = s;
    updateHist(roi);
}


void CamShiftTracker::setVbins(int v) {
    vbins = v;
    updateHist(roi);
}

void CamShiftTracker::setAlpha(double a) {
    alpha = a;
}

void CamShiftTracker::setThreshold(int t) {
    thresh = t;
}

cv::Mat CamShiftTracker::getHeatmap()
{
    return heatmap;
}

cv::Rect CamShiftTracker::getBoundingRect()
{
//    return rotatedBox.boundingRect();
    return trackWindow;
}

cv::RotatedRect CamShiftTracker::getRotatedRect()
{
    return rotatedBox;
}

cv::Mat CamShiftTracker::getBackProjection()
{
    return backproj;
}

cv::Mat CamShiftTracker::getRoi()
{
    return roi;
}

cv::Point CamShiftTracker::getPosition()
{
    cv::Rect boundRect = getBoundingRect();
    return boundRect.br()/2 + boundRect.tl()/2;
}
