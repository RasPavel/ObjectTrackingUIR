#include "particle.h"

Particle::Particle() {

}

Particle::Particle(double w, cv::Point position)
{
   weight = w;
   pos = position;
}
