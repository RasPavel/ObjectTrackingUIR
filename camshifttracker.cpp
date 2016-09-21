#include "camshifttracker.h"

CamShiftTracker::CamShiftTracker(cv::Mat frame, cv::Rect roi_rect)
{
    cv::Mat hsv_roi;
    roi = cv::Mat(frame, roi_rect).clone();
    cv::cvtColor(roi, hsv_roi, CV_RGB2HSV);

    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    calcHist( &hsv_roi, 1, channels, mask_roi, roi_hist, 3, histSize, ranges, true);

    sigma = (roi_rect.width > roi_rect.height) ? roi_rect.width * 2 : roi_rect.height * 2;

//    double minVal, maxVal;
//    Point minLoc, maxLoc;
//    minMaxLoc(roi_hist, &minVal, &maxVal, &minLoc, &maxLoc );
//    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
    threshold(roi_hist, roi_hist2, 100, 255, CV_THRESH_TOZERO);

    qDebug() << countNonZero(mask_roi) << countNonZero(hue_roi) << countNonZero(roi_hist);

    termCrit = TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    trackWindow = roi_rect;
}

void CamShiftTracker::processFrame(const cv::Mat& frame)
{
    cv::cvtColor(frame, hsv_frame, CV_RGB2HSV);
    inRange(hsv_frame, lowThresh, highThresh, mask);

    const float* ranges[] = { hranges, sranges, vranges};
    hue_sat = hsv_frame.clone();
    mixChannels(&hsv_frame, 1, &hue_sat, 1, mixCh, 3);

    calcBackProject(&hue_sat, 1, channels, roi_hist, backproj, ranges);
//    backproj &= mask;
    rotatedBox = CamShift(backproj, trackWindow, termCrit);
}

void CamShiftTracker::processFrame(const cv::Mat& frame, cv::Mat bg_mask)
{
    cv::cvtColor(frame, hsv_frame, CV_RGB2HSV);
    inRange(hsv_frame, lowThresh, highThresh, mask);

    hue_sat = hsv_frame.clone();
    mixChannels(&hsv_frame, 1, &hue_sat, 1, mixCh, 3);

    calcBackProject(&hsv_frame, 1, channels, roi_hist, backproj, ranges);

    addWeighted(backproj, alpha, bg_mask, 1.0 - alpha, 0, heatmap);
    int last_x = trackWindow.x;
    int last_y = trackWindow.y;
    for(int i = 0; i < heatmap.rows; i++) {
        for (int j = 0; j < heatmap.cols; j++) {
            double r = sqrt(pow(last_x - j, 2) + pow(last_y - i, 2));
            double gaussian = exp(- r * r / 2.0 / sigma / sigma);
            heatmap.at<uchar>(i,j) = heatmap.at<uchar>(i,j) * gaussian;
        }
    }


    threshold(heatmap, heatmap, thresh, 0, cv::THRESH_TOZERO);
//    backproj &= mask;

    rotatedBox = CamShift(heatmap, trackWindow, termCrit);
}

void CamShiftTracker::updateHist(cv::Mat roi)
{
    cv::Mat hsv_roi;
    cv::cvtColor(roi, hsv_roi, CV_RGB2HSV);

    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    calcHist(&hsv_roi, 1, channels, mask_roi, roi_hist, 3, histSize, ranges, true);
}

void CamShiftTracker::setHbins(int h)
{
    hbins = h;
    updateHist(roi);
}


void CamShiftTracker::setSbins(int s)
{
    sbins = s;
    updateHist(roi);
}


void CamShiftTracker::setVbins(int v)
{
    vbins = v;
    updateHist(roi);
}

void CamShiftTracker::setAlpha(double a) { alpha = a; }

void CamShiftTracker::setThreshold(int t) { thresh = t; }

cv::Mat CamShiftTracker::getHeatmap() { return heatmap; }

cv::Rect CamShiftTracker::getBoundingRect() { return trackWindow; }

cv::RotatedRect CamShiftTracker::getRotatedRect() { return rotatedBox; }

cv::Mat CamShiftTracker::getBackProjection() { return backproj; }

cv::Mat CamShiftTracker::getRoi() { return roi; }

cv::Point CamShiftTracker::getPosition()
{
    cv::Rect boundRect = getBoundingRect();
    return boundRect.br()/2 + boundRect.tl()/2;
}
