#include "light_volume.h"


LightVolume::LightVolume(Point3D const& p) : BoundingVolume(p) { }
LightVolume::LightVolume() : BoundingVolume() { }

LightVolume::~LightVolume() { }


/***************************************************************************
 *                               LightSphere                               *
 ***************************************************************************/

LightSphere::LightSphere(Point3D const& p, double radius) :
    LightVolume(p), radius_(radius) { }

LightSphere::LightSphere(double radius) :
    LightVolume(), radius_(radius) { }


void LightSphere::getSubtendedDir(Point3D const& viewPoint,
                                 double uniformX, double uniformY,
                                 Vector3D& dir) const {
    // Vector from viewPoint to the center of sphere
    Vector3D toCenter(this->pos - viewPoint);

    /*
     * cosine of the maximum angle away from the toCenter line.
     * any vectors with a larger angle will not touch the sphere.
     */
    double cosThetaMax = getCosThetaMax(toCenter);

    double cosTheta = 1 - uniformX + uniformX * cosThetaMax;
    double sinTheta = sqrt(1 - cosTheta * cosTheta);
    double fi = 2 * M_PI * uniformY;

    // find dir from cosTheta and fi
    Vector3D tempDir(cos(fi) * sinTheta, sin(fi) * sinTheta, cosTheta);
    // then rotate with respect to the toCenter vector.
    dir = rotateVector(tempDir, toCenter);
}

// ASSUMPTION: dir is normalized
bool LightSphere::isSubtended(Point3D const& viewPoint, Vector3D const& dir) const {

    Point3D origin(this->pos[0] - viewPoint[0],
                   this->pos[1] - viewPoint[1],
                   this->pos[2] - viewPoint[2]);

    double a = dir.dot(origin);
    double trueRadius = radius_ * this->scale;
    double discriminant = a*a - origin.squaredNorm() + trueRadius*trueRadius;

    // TODO: return nearest t_value? if t_value is larger than current best,
    // can ignore object completely when used for acceleration

    // return true if discriminant is non-negative, ie. solutions exist
    return discriminant > 500*std::numeric_limits<double>::epsilon();
}

double LightSphere::subtendedProbability(Point3D const& viewPoint) const {

    Vector3D toCenter(this->pos - viewPoint);

    // will divide results from luminaires by this probability.
    // will be larger, the further away the light source
    return 1.0 / (1 - getCosThetaMax(toCenter));
}

/**
 * Given a vector toCenter from the viewpoint to the center of the sphere,
 * return the cosine of the maximal angle of any vector subtended by the
 * sphere, to the toCenter vector.
 */
double LightSphere::getCosThetaMax(Vector3D& toCenter) const {

    double distance = toCenter.normalize();
    double trueRadius = radius_ * this->scale;
    if (trueRadius > distance) {
        return 0.0;
    }

    double sinThetaMax = trueRadius / distance;

    return sqrt( 1 - sinThetaMax*sinThetaMax);
}

