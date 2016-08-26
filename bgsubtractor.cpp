#include "bgsubtractor.h"

BgSubtractor::BgSubtractor()
{
//    bgSubtractor = cv::createBackgroundSubtractorMOG2(1000, 16, false);
    bgSubtractor = cv::createBackgroundSubtractorMOG2(1000,16,false);
//    history	Length of the history.
//    varThreshold	Threshold on the squared Mahalanobis distance between the pixel and the model to decide whether a pixel is well described by the background model. This parameter does not affect the background update.
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

    int morph_size = 1;
    int close_size = 2;
    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size));
    cv::Mat close_element = getStructuringElement(cv::MORPH_RECT, cv::Size( 2*close_size + 1, 2*close_size+1 ), cv::Point( close_size, close_size));
    morphologyEx(mask, morph_mask, cv::MORPH_OPEN, element);
    morphologyEx(mask, mask_open, cv::MORPH_OPEN, element);
    morphologyEx(mask_open, mask_close, cv::MORPH_CLOSE, close_element);

//    int niters = 3;
//    Mat temp;
//    dilate(mask, temp, Mat(), Point(-1,-1), niters);
//    erode(temp, temp, Mat(), Point(-1,-1), niters*2);
//    dilate(temp, temp, Mat(), Point(-1,-1), niters);

}

cv::Mat* BgSubtractor::getMask() {

    return &mask;
}

cv::Mat* BgSubtractor::getMorphMask() {
    return & morph_mask;
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

     findContours( mask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

     if( contours.size() == 0 )
         return Rect();
     // iterate through all the top-level contours,
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



     /// Draw polygonal contour + bonding rects + circles
     /*Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
     for( int i = 0; i< contours.size(); i++ )
        {
          Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
          drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
          rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        }*/


     return  boundRect;

}

cv::Point BgSubtractor::getPosition() {
    cv::Rect boundRect = getBoundingRect();
    return boundRect.br()/2 + boundRect.tl()/2;
}

int BgSubtractor::getMixtureCount() {
    return bgSubtractor->getNMixtures();
}

