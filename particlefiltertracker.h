#ifndef PARTICLEFILTERTRACKER_H
#define PARTICLEFILTERTRACKER_H



#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include "particle.h"
#include "objecttracker.h"


#include <vector>
#include <QDebug>




class ParticleFilterTracker: ObjectTracker
{
public:
    static const int HAAR_COUNT = 4;
    std::vector<Particle> particle_set;

    cv::Vec3i template_haar[HAAR_COUNT];
    cv::Mat template_img;
    cv::Mat current_frame;
    cv::Mat template_hist;
    cv::Point current_pos;
    cv::Rect current_rect;
    int N = 100;
    double sigmax = 4;
    double sigmay = 4;
    double lambda = 100;
    float hranges[2] = { 0, 180 };
    float sranges[2] = { 0, 256 };
    float vranges[2] = { 0, 256 };
    Scalar lowThresh = Scalar(0, 0, 0);
    Scalar highThresh = Scalar(180, 255, 255);
    int channels[3] = {0, 1, 2};
    int hbins = 10, sbins = 10, vbins = 10;

    ParticleFilterTracker();
    void init(cv::Mat frame, cv::Rect roi);
    void processFrame(const cv::Mat& frame);
    void resample();
    void reweight();
    void estimateState();
    void transition();
    double likelihood(Particle p);

    cv::Vec3i haarVec(cv::Mat img, int type);



    cv::Point getPosition();
    cv::Rect getBoundingRect();

private:
    cv::Mat particle_roi_mask;
    cv::Mat particle_roi_hsv;
    cv::Mat particle_roi_hist;


    void printVec3i(cv::Vec3i v);
    void printParticleSet() {
        double sum = 0;

        for (auto & pa : particle_set) {
            qDebug() << "Particle" << pa.x << pa.y << "w" << pa.weight;
            sum += pa.weight;
        }
        qDebug() << "weights sum" << sum;


    }

    cv::RNG rng;
};

#endif // PARTICLEFILTERTRACKER_H
