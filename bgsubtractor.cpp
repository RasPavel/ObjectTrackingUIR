#include "bgsubtractor.h"

BgSubtractor::BgSubtractor()
{
//    bgSubtractor = cv::createBackgroundSubtractorMOG2(1000, 16, false);
    bgSubtractor = cv::createBackgroundSubtractorMOG2(1000,16,false);
    openElementSize = 1;
    closeElementSize = 2;
//    history	Length of the history.
//    varThreshold	Threshold on the squared Mahalanobis distance between the pixelthe model toel whether a pixel is wellelbed belackground model. This parameter del affect the background update.
//    detectShadows	If true, the algorithm will detect shadows and mark them. It decreases the speed a bit, so if you do not need this feature, set the parameter to false.
    //http://docs.opencv.org/master/de/de1/group__video__motion.html#ga2beb2dee7a073809ccec60f145b6b29c&gsc.tab=0
}

void BgSubtractor::processFrame(const cv::Mat &frame) {
    if (mask.empty()) {
        qDebug() << "bgSubtractor got an empty frame";
        mask= cv::Mat(frame.rows, frame.cols, frame.type());
        qDebug() << frame.rows << " " << frame.cols << frame.type();
        qDebug() << mask.rows << mask.cols << mask.type();
    }
    bgSubtractor->apply(frame, mask);

    cv::Mat open_el = getStructuringElement(cv::MORPH_RECT, cv::Size( 2*openElementSize + 1, 2*openElementSize+1 ), cv::Point( openElementSize, openElementSize));
    cv::Mat close_el = getStructuringElement(cv::MORPH_RECT, cv::Size( 2*closeElementSize + 1, 2*closeElementSize+1 ), cv::Point( closeElementSize, closeElementSize));
//    morphologyEx(mask, morph_mask, cv::MORPH_OPEN, el);
    morphologyEx(mask, mask_open, cv::MORPH_OPEN, open_el);
    morphologyEx(mask_open, mask_close, cv::MORPH_CLOSE, close_el);
}

cv::Mat* BgSubtractor::getMask() {

    return &mask;
}

cv::Mat* BgSubtractor::getMorphMask() {
    return &morph_mask;
}

cv::Rect BgSubtractor::getBoundingRect() {

    using namespace std;
    using namespace cv;
    cv::Mat threshold_output;
     vector<vector<Point> > contours;
     vector<Vec4i> hierarchy;


     /// Find contours
//     findContours( fgMask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

     /// Approximate contours to polygons + get bounding rects and circles
     vector<Point> contour_poly;
     Rect boundRect;

     cv::Mat mask_copy;
     mask.copyTo(mask_copy);
     findContours( mask_copy, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

     if( contours.size() == 0 )
         return Rect();
     // iterate through all the top-levelours,
     // draw each connected component with its own random color
     int largestComp = 0;
     double maxArea = 0;
     for(int idx = 0 ; idx >= 0; idx = hierarchy[idx][0] )
     {
         const vector<Point>& c = contours[idx];
         double area = fabs(contourArea(Mat(c)));
         if( area > maxArea )
         {
             maxArea = area;
             largestComp = idx;
         }
     }
     approxPolyDP( Mat(contours[largestComp]), contour_poly, 3, true );
     boundRect = boundingRect( Mat(contour_poly) );

     return  boundRect;

}

cv::Point BgSubtractor::getPosition() {
    cv::Rect boundRect = getBoundingRect();
    return boundRect.br()/2 + boundRect.tl()/2;
}

int BgSubtractor::getMixtureCount() {
    return bgSubtractor->getNMixtures();
}

void BgSubtractor::setHistory(int h) {
    bgSubtractor->setHistory(h);
}

void BgSubtractor::setVarThreshold(int vt) {
    bgSubtractor->setVarThreshold(vt);
}

void BgSubtractor::setOpenElementSize(int s) {
    openElementSize = s;
}

void BgSubtractor::setCloseElementSize(int s) {
    closeElementSize = s;
}

