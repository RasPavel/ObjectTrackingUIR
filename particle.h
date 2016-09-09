#ifndef PARTICLE_H
#define PARTICLE_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <QDebug>

class Particle
{
public:
    Particle();
    Particle(double, cv::Point);
    cv::Point pos;
    double weight;
};

#endif // PARTICLE_H
