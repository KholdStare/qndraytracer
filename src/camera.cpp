#include "camera.h"
#include "texture/image_io.h"
#include "texture/rgba_converter.hpp"
#include "texture/bmp_image.h"

Camera::Camera(unsigned int width, unsigned int height,
       Point3D const& eye, Vector3D const& view, Vector3D const& up,
       double fov) :
            factor_((double(height)/2)/tan(fov*M_PI/360.0)),
            apertureRadius_(0),
            sensor_(width, height),
            gamma_(1.0),
            subSampler_(1),
            apertureSampler_(1),
            eye_(eye),
            view_(view),
            up_(up),
            viewToWorld_(initInvViewMatrix(eye, view, up)) {

    initDefault();

}


Camera::Camera(unsigned int width, unsigned int height,
       double fov) :
            factor_((double(height)/2)/tan(fov*M_PI/360.0)),
            apertureRadius_(0),
            sensor_(width, height),
            gamma_(1.0),
            subSampler_(1),
            apertureSampler_(1),
            eye_(0.0, 0.0, 0.0),
            view_(0.0, 0.0, -1.0),
            up_(0.0, 1.0, 0.0),
            viewToWorld_(initInvViewMatrix(eye_, view_, up_)) {

    initDefault();

}

Camera::~Camera() { }

void Camera::initDefault() {
    setFocalDistance(2);
}

void Camera::setApertureRadius(double r) {
    apertureRadius_ = r;
}

// distance to the focal plane
void Camera::setFocalDistance(double d) {
    focalDistance_ = d;
}

void Camera::setEye (Point3D const& eye) {
    eye_ = eye;
    viewToWorld_ = initInvViewMatrix(eye_, view_, up_);
}

void Camera::setView(Vector3D const& view) {
    view_ = view;
    viewToWorld_ = initInvViewMatrix(eye_, view_, up_);
}

void Camera::setUp  (Vector3D const& up) {
    up_ = up;
    viewToWorld_ = initInvViewMatrix(eye_, view_, up_);
}

// Take previously calculated data, and merge it with the current data.
bool Camera::mergeSensor(Image<SensorPixel> const& other) {
    return mergeSensors(sensor_, other);
}

bool Camera::dumpToBMP(std::string filename) const {
    Image<RGBA> convertedImage = convertImageToRGBA(sensor_,
                                                    RGBAConverter<SensorPixel>{gamma_});

    return writeBmpToFile(convertedImage, filename);
}

bool Camera::dumpRawData(std::string filename) const {
    return writeImageToFile(sensor_, filename);
}

// For a given pixel in the pixel buffer, generate
// image plane coordinates and pass those to the lens
// to be sampled
void Camera::computePixel(int i, int j) const {
    SensorPixel& pixel = sensor_[i][j];

    // if we are taking more than one sample from each pixel,
    // do antialiasing
    if (subSampler_.n() > 1) {
        // find image plane coordinates of pixel
        double xStart = (-double(sensor_.width())/2 + j);
        double yStart = (-double(sensor_.height())/2 + i);

        //typedef coordinate_traits<double, 2>::type sample_type;
        for (auto const& sample : subSampler_)
        {
            // for each sample, offset pixel coordinate so we are
            // sampling within the pixel
            double x = (xStart + sample[0]);
            double y = (yStart + sample[1]);

            // let the lens handle FOV and DOF
            pixel += sampleLens(x, y); 
        }

        // NOTE: here I do not divide by number of samples, instead the
        // pixel data structure keeps a count of how many samples have
        // been deposited (allowing for multiple passes).
        // the division happens at the very end when the image is dumped
        // to a BMP
        // see "texture/sensor.h" for details
    }
    // otherwise, sample the center of the pixel
    else {
        double x = (-double(sensor_.width())/2 + 0.5 + j);
        double y = (-double(sensor_.height())/2 + 0.5 + i);

        pixel += sampleLens(x, y); 
    }
}

// Uses image plane coordinates to construct rays from lens
Colour Camera::sampleLens(double x, double y) const {
    x = (x * focalDistance_) / factor_;
    y = (y * focalDistance_) / factor_;
    
    Colour col;

    // sample aperture disk for DOF, if not pinhole camera
    if ( apertureRadius_ > std::numeric_limits<double>::epsilon() && apertureSampler_.n() > 1) {

        for (auto const& sample : apertureSampler_)
        {
            // respect the jacobian when sampling disk
            double r = sqrt(sample[0])*apertureRadius_;
            double theta = 2*M_PI*sample[1];

            // find the aperture point that acts as the origin of the ray
            Point3D aperturePoint(r*cos(theta),
                                  r*sin(theta), 0);
            // get direction to focus point
            Vector3D toPixel(Point3D(x, y, -focalDistance_) - aperturePoint);

            // construct ray
            Ray3D ray(viewToWorld_.transformPoint(aperturePoint.v),
                      viewToWorld_.transformVector(toPixel.v));

            // sample scene with ray
            col += sceneSamplingFunc_(ray); 
        }

        // divide by number of samples taken by the sampler
        col /= apertureSampler_.n() ;
    }
    // otherwise it's a pinhole camera
    else {
        // shoot a single ray directly through center of aperture
        Ray3D ray(eye_, viewToWorld_.transformVector(Eigen::Vector3d(x, y, -focalDistance_)));
        col += sceneSamplingFunc_(ray); 
    }

    return col;
}

AffineTrans3D initInvViewMatrix( Point3D const& eye, Vector3D view, Vector3D up) {
    AffineTrans3D mat; 
    Vector3D w;
    view.normalize();
    up = up - up.dot(view)*view;
    up.normalize();
    w = view.cross(up);

    mat.A.col(0) = w.v;
    mat.A.col(1) = up.v;
    mat.A.col(2) = -view.v;

    mat.t = eye.v;

    return mat; 
}

