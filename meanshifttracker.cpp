#include "meanshifttracker.h"

MeanShiftTracker::MeanShiftTracker(cv::Mat frame, cv::Rect roi_rect)
{
    cv::Mat hsv_roi, mask_roi, hue_roi;
    cv::cvtColor(cv::Mat(frame, roi_rect), hsv_roi, CV_RGB2HSV);

    int ch[] = {0, 0};
    hue_roi.create(hsv_roi.size(), hsv_roi.depth());
    mixChannels(&hsv_roi, 1, &hue_roi, 1, ch, 1);


    const float* phranges = hranges;

    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    calcHist(&hue_roi, 1, 0, mask_roi, roi_hist, 1, &hsize, &phranges);
    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);

    termCrit = TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
}

void MeanShiftTracker::processFrame(cv::Mat frame)
{
    cv::cvtColor(frame, frame, CV_RGB2HSV);
    inRange(frame, lowThresh, highThresh, mask);
    int ch[] = {0, 0};
    hue.create(frame.size(), frame.depth());
    mixChannels(&frame, 1, &hue, 1, ch, 1);

    const float* phranges = hranges;
    calcBackProject(&hue, 1, 0, roi_hist, backproj, &phranges);
    backproj &= mask;




    RotatedRect trackBox = CamShift(backproj, trackWindow, termCrit);
//    if( trackWindow.area() <= 1 )
//    {
//        int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
//        trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
//                          trackWindow.x + r, trackWindow.y + r) &
//                      Rect(0, 0, cols, rows);
//   }


}

cv::Mat* MeanShiftTracker::getBackProjection()
{
    return &backproj;
}
