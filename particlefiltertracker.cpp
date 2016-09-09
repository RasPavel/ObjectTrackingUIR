#include "particlefiltertracker.h"

ParticleFilterTracker::ParticleFilterTracker()
{
    Particle p1(0.8, cv::Point(100,100));



}

void ParticleFilterTracker::init(cv::Mat frame, cv::Rect roi) {
    qDebug() << "INIT called";
    template_img = frame(roi).clone();
    current_pos = roi.br() / 2 + roi.tl() / 2;
    current_rect = roi;
    for (int i = 0; i < HAAR_COUNT; i++) {
        template_haar[i] = haarVec(template_img, i);
    }
    printVec3i(template_haar[0]);

    particle_set.clear();
    srand(time(0));
    for (int i = 0; i < N; i++) {
        double xmin = roi.tl().x;
        double xmax = roi.br().x;
        double x = (xmax - xmin) * ( (double)rand() / (double)RAND_MAX ) + xmin;
        double ymin = roi.tl().y;
        double ymax = roi.br().y;
        double y = (ymax - ymin) * ( (double)rand() / (double)RAND_MAX ) + ymin;
        particle_set.push_back(Particle(1.0 / N, cv::Point(x, y)));
    }

}

void ParticleFilterTracker::processFrame(const cv::Mat& frame) {
    current_frame = frame.clone();
    resample();
    qDebug() << "before";
    printParticleSet();
    transition();
    printParticleSet();
    reweight();
    printParticleSet();
    estimateState();
    printParticleSet();
}

void ParticleFilterTracker::resample() {
    qDebug() << "resample called";
    std::map<double, Particle> cumulative;
    double sum = 0;

    for(std::vector<Particle>::const_iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        sum += (*i).weight;
        cumulative[sum] = (*i);
    }


    double r;
    std::vector<Particle> new_particle_set;
    new_particle_set.reserve(N);
    srand(time(0));
    for (int i = 0; i < N; i++) {
        r = ((double) rand() / (RAND_MAX));
        qDebug() << r;
        double cum = cumulative.upper_bound(r)->first;
        Particle p = cumulative[cum];
        qDebug() << p.weight;
        new_particle_set.push_back(Particle(p.weight, cv::Point(p.pos.x, p.pos.y)));
    }
    particle_set.clear();
    particle_set.shrink_to_fit();
    particle_set = new_particle_set;
}

void ParticleFilterTracker::reweight() {
    qDebug() << "reweight called";
    double weight_sum = 0;
    for(std::vector<Particle>::iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        double lik = likelihood(*i);
        qDebug() << "lik" << lik;
        (*i).weight = lik;
        weight_sum += lik;
    }
    for(std::vector<Particle>::iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        qDebug() << "before reweight:" << (*i).weight << weight_sum;
        (*i).weight /= weight_sum;
        qDebug() << "after reweight:" << (*i).weight;
    }

}

double ParticleFilterTracker::likelihood(Particle p) {
    int x = p.pos.x;
    int y = p.pos.y;
    int w = current_rect.width;
    int h = current_rect.height;
    cv::Rect region_rect(x - w/2, y - h/2, w, h);
    cv::Mat region = current_frame(region_rect);

    cv::Vec3i particle_haar[HAAR_COUNT];
    for (int i = 0; i < HAAR_COUNT; i++) {
        particle_haar[i] = haarVec(region, i);
    }

    int sum = 0;
    for (int i = 0; i < HAAR_COUNT; i++) {
        for (int channel = 0; channel < 3; channel++) {
            int c1 = particle_haar[i].val[channel];
            int c2 = template_haar[i].val[channel];
            sum += pow(c1 - c2, 2);
        }
    }
    double haar_score = sqrt(sum);


    qDebug() << "template haar 4";
    printVec3i(template_haar[0]);
    printVec3i(particle_haar[0]);
    qDebug() << "haar score" << haar_score;
//    qDebug() << haar_particle[0] << haar_particle[1] << haar_particle[2];
//    qDebug() << template_haar[0] << template_haar[1] << template_haar[2];

    double likelihood = exp(- haar_score*haar_score / lambda / lambda);
    return likelihood;

}

void ParticleFilterTracker::estimateState() {
    qDebug() << "estimate called";
    double sumx = 0;
    double sumy = 0;
    for(std::vector<Particle>::const_iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        Particle p = *i;
        sumx += p.pos.x * p.weight;
        sumy += p.pos.y * p.weight;
    }
    int meanx = sumx / N;
    int meany = sumy / N;

    current_pos = cv::Point(meanx, meany);
    int w = current_rect.width;
    int h = current_rect.height;
    current_rect = cv::Rect(meanx - w/2, meany - h/2, w, h);
}

void ParticleFilterTracker::transition() {
    qDebug() << "transition called";

    for(std::vector<Particle>::iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        qDebug() << "before point" << (*i).pos.x;
        int newx = (*i).pos.x + (int) rng.gaussian(sigmax);
        int newy = (*i).pos.y + (int) rng.gaussian(sigmay);
        (*i).pos = cv::Point(newx, newy);
        qDebug() << "after point" << (*i).pos.x;
    }
}

cv::Vec3i ParticleFilterTracker::haarVec(cv::Mat frame, int type) {
    cv::Mat integral_img;
    integral(frame, integral_img);

//    qDebug() << frame.rows << "x" <<frame.cols << frame.channels();

    cv::Mat mask = cv::Mat::ones(frame.rows, frame.cols, CV_8UC1);

//    cv::Mat topleft = mask(cv::Rect(0, 0, mask.rows/2, mask.cols / 2));
//    cv::Mat botleft = mask(cv::Rect(0, mask.cols/2, mask.rows/2, mask.cols / 2));
//    cv::Mat topright = mask(cv::Rect(mask.cols/2, 0, mask.rows/2, mask.cols / 2));
//    cv::Mat botright = mask(cv::Rect(mask.rows/2, mask.cols/2, mask.rows/2, mask.cols / 2));

    cv::Vec3i haar;
    int w = integral_img.rows;
    int h = integral_img.cols;
    cv::Vec3i v1 =  integral_img.at<cv::Vec3i>(w/2, h/2);
    cv::Vec3i v2 =  integral_img.at<cv::Vec3i>(w-1, h/2);
    cv::Vec3i v3 =  integral_img.at<cv::Vec3i>(w/2, h-1);
    cv::Vec3i v4 =  integral_img.at<cv::Vec3i>(w-1, h-1);
//    printVec3i(v1);
//    printVec3i(v2);
//    printVec3i(v3);
//    printVec3i(v4);
    cv::Vec3i topleft = integral_img.at<cv::Vec3i>(w/2, h/2) / (w/2 * h/2);
    cv::Vec3i botleft = (integral_img.at<cv::Vec3i>(w/2, h-1) - integral_img.at<cv::Vec3i>(w/2, h/2)) / (w/2 * h/2);
    cv::Vec3i topright = (integral_img.at<cv::Vec3i>(w-1, h/2) - integral_img.at<cv::Vec3i>(w/2, h/2)) / (w/2 * h/2);
    cv::Vec3i botright = (integral_img.at<cv::Vec3i>(w-1, h-1) + integral_img.at<cv::Vec3i>(w/2, h/2)
                          - integral_img.at<cv::Vec3i>(w-1, h/2) - integral_img.at<cv::Vec3i>(w/2, h-1)) / (w/2 * h/2);
//    printVec3i(topleft);
//    printVec3i(botleft);
//    printVec3i(topright);
//    printVec3i(botright);

    switch(type) {
    case 0:
        haar = (topleft + topright + botleft + botright) / 4;
        break;
    case 1:
        haar = -topleft - botleft + topright + botright;
        break;
    case 2:
        haar = topleft + topright - botleft - botright;
        break;
    case 3:
        haar = topright + botleft - topleft - botright;
        break;
    }

//    qDebug() << "Haar returns" ;
//    printVec3i(haar);

    return haar;
}

void ParticleFilterTracker::printVec3i(cv::Vec3i v) {
    qDebug()<< "Vec3i:" << v[0] << v[1] << v[2];
}

cv::Point ParticleFilterTracker::getPosition() {
    return current_pos;
}

cv::Rect ParticleFilterTracker::getBoundingRect() {
    return current_rect;
}

