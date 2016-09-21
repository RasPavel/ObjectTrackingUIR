#include "particlefiltertracker.h"

ParticleFilterTracker::ParticleFilterTracker()
{}

void ParticleFilterTracker::init(cv::Mat frame, cv::Rect roi)
{
    qDebug() << "INIT called";
    template_img = frame(roi).clone();
    current_pos = roi.br() / 2 + roi.tl() / 2;
    current_rect = roi;
    for (int i = 0; i < HAAR_COUNT; i++) {
        template_haar[i] = haarVec(template_img, i);
    }

    int histSize[] = {hbins, sbins, vbins};

    const float* ranges[] = { hranges, sranges, vranges};

    cv::Mat mask_roi;
    cv::Mat hsv_roi;
    cv::cvtColor(template_img, hsv_roi, CV_RGB2HSV);
    inRange(hsv_roi, lowThresh, highThresh, mask_roi);
    calcHist( &hsv_roi, 1, channels, mask_roi, template_hist, 3, histSize, ranges, true);

    particle_set.clear();
    srand(time(0));
    for (int i = 0; i < N; i++) {
        double xmin = roi.tl().x;
        double xmax = roi.br().x;
        double x = (xmax - xmin) * ( (double)rand() / (double)RAND_MAX ) + xmin;
        double ymin = roi.tl().y;
        double ymax = roi.br().y;
        double y = (ymax - ymin) * ( (double)rand() / (double)RAND_MAX ) + ymin;
        particle_set.push_back(Particle(1.0 / N, x, y));
    }

}

void ParticleFilterTracker::processFrame(const cv::Mat& frame)
{
    current_frame = frame.clone();
    resample();
    transition();
    reweight();
    estimateState();
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
        double cum = cumulative.upper_bound(r)->first;
        Particle p = cumulative[cum];
        new_particle_set.push_back(Particle(p.weight, p.x, p.y));
    }
    particle_set.clear();
    particle_set.shrink_to_fit();
    particle_set = new_particle_set;
}

void ParticleFilterTracker::reweight()
{
    qDebug() << "reweight called";
    double weight_sum = 0;
    for(std::vector<Particle>::iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        double lik = likelihood(*i);
        (*i).weight = lik;
        weight_sum += lik;
    }
    for(std::vector<Particle>::iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        (*i).weight /= weight_sum;
    }
}

double ParticleFilterTracker::likelihood(Particle p)
{

    int w = current_rect.width;
    int h = current_rect.height;

    cv::Rect roi_rect = cv::Rect(p.x - w/2, p.y - h/2, w, h) & cv::Rect(0, 0, current_frame.cols, current_frame.rows);
    cv::Mat particle_roi = current_frame(roi_rect);

    cv::Vec3i particle_haar[HAAR_COUNT];
    for (int i = 0; i < HAAR_COUNT; i++) {
        particle_haar[i] = haarVec(particle_roi, i);
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

    cv::cvtColor(particle_roi, particle_roi_hsv, CV_RGB2HSV);
    inRange(particle_roi_hsv, lowThresh, highThresh, particle_roi_mask);
    int histSize[] = {hbins, sbins, vbins};
    const float* ranges[] = { hranges, sranges, vranges};
    calcHist( &particle_roi_hsv, 1, channels, particle_roi_mask, particle_roi_hist, 3, histSize, ranges, true);

    double hist_score = cv::compareHist(template_hist, particle_roi_hist, CV_COMP_CORREL);
//    qDebug() << "haar score" << haar_score;
//    qDebug() << "hist_score" << hist_score;
    double haar_likelihood = exp(- haar_score*haar_score / lambda / lambda);
    double alpha = 0; // haar features are fast to compute but not powerful enough
    double final_score = alpha * haar_likelihood + (1 - alpha) * hist_score;

    return final_score;
}

void ParticleFilterTracker::estimateState()
{
    double sumx = 0;
    double sumy = 0;
    for(std::vector<Particle>::const_iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        Particle p = *i;
        sumx += p.x * p.weight;
        sumy += p.y * p.weight;
    }
    int meanx = sumx / N;
    int meany = sumy / N;

    current_pos = cv::Point(meanx, meany);
    int w = current_rect.width;
    int h = current_rect.height;
    current_rect = cv::Rect(meanx - w/2, meany - h/2, w, h);
}

void ParticleFilterTracker::transition()
{
    qDebug() << "transition called";

    for(std::vector<Particle>::iterator i = particle_set.begin(); i != particle_set.end(); ++i) {
        (*i).x = (*i).x + (int) rng.gaussian(sigmax);
        (*i).y = (*i).y + (int) rng.gaussian(sigmay);
    }
}

cv::Vec3i ParticleFilterTracker::haarVec(cv::Mat frame, int type)
{
    cv::Mat integral_img;
    integral(frame, integral_img);

    cv::Mat mask = cv::Mat::ones(frame.rows, frame.cols, CV_8UC1);

    cv::Vec3i haar;
    int w = integral_img.rows;
    int h = integral_img.cols;
    cv::Vec3i v1 =  integral_img.at<cv::Vec3i>(w/2, h/2);
    cv::Vec3i v2 =  integral_img.at<cv::Vec3i>(w-1, h/2);
    cv::Vec3i v3 =  integral_img.at<cv::Vec3i>(w/2, h-1);
    cv::Vec3i v4 =  integral_img.at<cv::Vec3i>(w-1, h-1);

    cv::Vec3i topleft = integral_img.at<cv::Vec3i>(w/2, h/2) / (w/2 * h/2);
    cv::Vec3i botleft = (integral_img.at<cv::Vec3i>(w/2, h-1) - integral_img.at<cv::Vec3i>(w/2, h/2)) / (w/2 * h/2);
    cv::Vec3i topright = (integral_img.at<cv::Vec3i>(w-1, h/2) - integral_img.at<cv::Vec3i>(w/2, h/2)) / (w/2 * h/2);
    cv::Vec3i botright = (integral_img.at<cv::Vec3i>(w-1, h-1) + integral_img.at<cv::Vec3i>(w/2, h/2)
                          - integral_img.at<cv::Vec3i>(w-1, h/2) - integral_img.at<cv::Vec3i>(w/2, h-1)) / (w/2 * h/2);

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

    return haar;
}

void ParticleFilterTracker::printVec3i(cv::Vec3i v)
{
    qDebug()<< "Vec3i:" << v[0] << v[1] << v[2];
}

void ParticleFilterTracker::printParticleSet()
{
    double sum = 0;

    for (auto & pa : particle_set) {
        qDebug() << "Particle" << pa.x << pa.y << "w" << pa.weight;
        sum += pa.weight;
    }
    qDebug() << "weights sum" << sum;
}

cv::Point ParticleFilterTracker::getPosition() { return current_pos; }

cv::Rect ParticleFilterTracker::getBoundingRect() { return current_rect; }

