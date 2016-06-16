#include "bgsubtractor.h"

BgSubtractor::BgSubtractor()
{
    bgSubtractor = cv::createBackgroundSubtractorMOG2(1000, 16, false);
//    history	Length of the history.
//    varThreshold	Threshold on the squared Mahalanobis distance between the pixel and the model to decide whether a pixel is well described by the background model. This parameter does not affect the background update.
//    detectShadows	If true, the algorithm will detect shadows and mark them. It decreases the speed a bit, so if you do not need this feature, set the parameter to false.
    //http://docs.opencv.org/master/de/de1/group__video__motion.html#ga2beb2dee7a073809ccec60f145b6b29c&gsc.tab=0
}

void BgSubtractor::processFrame(cv::Mat &frame) {
    qDebug() << "begin";
    if (fgMask.empty()) {
        qDebug() << "empty";
        fgMask= cv::Mat(frame.rows, frame.cols, frame.type());
        qDebug() << frame.rows << " " << frame.cols << frame.type();
        qDebug() << fgMask.rows << fgMask.cols << fgMask.type();
    }
    bgSubtractor->apply(frame, fgMask);
    qDebug() << fgMask.rows << fgMask.cols << fgMask.type();

    qDebug() << "end";
}

cv::Mat* BgSubtractor::getMask() {
    return &fgMask;
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

     findContours( fgMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );

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

