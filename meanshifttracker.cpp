#include "meanshifttracker.h"

MeanShiftTracker::MeanShiftTracker(cv::Mat frame, cv::Rect roi_rect)
{
    cv::Mat hsv_roi;
    roi = cv::Mat(frame, roi_rect).clone();
    cv::cvtColor(roi, hsv_roi, CV_RGB2HSV);

    int hbins = 12, sbins = 10, vbins = 10;
    int histSize[] = {hbins, sbins, vbins};

    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    float vranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges, vranges};
    int channels[] = {0, 1, 2};

//    int channels[] = {0, 0};
//    hue_roi.create(hsv_roi.size(), hsv_roi.depth());
//    mixChannels(&hsv_roi, 1, &hue_roi, 1, channels, 1);
//    const float* phranges = hranges;
//    calcHist(&hue_roi, 1, 0, mask_roi, roi_hist, 1, &hsize, &phranges);

    calcHist( &hsv_roi, 1, channels, mask_roi, roi_hist, 3, histSize, ranges, true);
    inRange(hsv_roi, lowThresh, highThresh, mask_roi);



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

void MeanShiftTracker::processFrame(const cv::Mat& frame)
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

    //    calcBackProject(&hue_sat, 1, channels, roi_hist2, backproj2, ranges);
//    backproj &= mask;



    meanShift(backproj, trackWindow, termCrit);
//    RotatedRect trackBox = CamShift(backproj, trackWindow, termCrit);


//    if( trackWindow.area() <= 1 )
//    {
//        int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
//        trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
//                          trackWindow.x + r, trackWindow.y + r) &
//                      Rect(0, 0, cols, rows);
//   }

}

void MeanShiftTracker::processFrame(const cv::Mat& frame, cv::Mat bg_mask)
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
    double beta = 1.0 - alpha;
//    qDebug() << backproj << bg_mask.size() << backproj.depth() << bg_mask.depth();
    addWeighted(backproj, alpha, bg_mask, beta, 0.5, heatmap);

    //    calcBackProject(&hue_sat, 1, channels, roi_hist2, backproj2, ranges);
//    backproj &= mask;



    meanShift(heatmap, trackWindow, termCrit);
//    RotatedRect trackBox = CamShift(backproj, trackWindow, termCrit);


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
