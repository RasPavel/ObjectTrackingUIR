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
    cv::Point current_pos;
    cv::Rect current_rect;
    int N = 100;
    double sigmax = 4;
    double sigmay = 4;
    double lambda = 10;

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
    void printVec3i(cv::Vec3i v);
    void printParticleSet() {
        double sum = 0;

        for (auto & pa : particle_set) {
            qDebug() << "Particle" << pa.pos.x << pa.pos.y << "w" << pa.weight;
            sum += pa.weight;
        }
        qDebug() << "weights sum" << sum;


    }

    cv::RNG rng;
};

#endif // PARTICLEFILTERTRACKER_H
