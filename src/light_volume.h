#ifndef _LIGHT_VOLUME_H_
#define _LIGHT_VOLUME_H_

#include "bounding_volume.h"

/**
 * A bounding volume around a light. Main purpose is
 * for getting sampling directions towards object, for 
 * lighting purposes. Can also be used for culling intersections
 */
class LightVolume : public BoundingVolume {

public:
    LightVolume();
    LightVolume(Point3D const& pos);
    virtual ~LightVolume();

    /**
     * Given view point, and unit unformly distributed random values x y,
     * return a direction towards the bounding volume
     */
    virtual void getSubtendedDir(Point3D const& viewPoint,
                                 double uniformX, double uniformY,
                                 Vector3D& dir) const = 0;

    /**
     * @Return whether a particular vector is subtended by the volume or not.
     *
     * If it is, then the probability associated with that direction is
     * non-zero and is specified by subtendedProbability().
     * Otherwise, probability is zero.
     * ASSUMPTION: dir is normalized
     */
    virtual bool isSubtended(Point3D const& viewPoint, 
                                        Vector3D const& dir) const = 0;

    /**
     * @Return probability of a vector from a view point in the solid angles 
     * subtended by the volume.
     *
     * Essentially just reciprocal of area on hemisphere. (or zero if outside area)
     */
    virtual double subtendedProbability(Point3D const& viewPoint) const = 0;

    /**
     * Fast intersection method that is equivalent to isSubtended
     */
    inline bool fastIntersect(Point3D const& viewPoint, 
                                        Vector3D const& dir) const {
        return isSubtended(viewPoint, dir);
    }

};

/**
 * A sphere that contains an emitting object.
 */
class LightSphere : public LightVolume {
// TODO: add position offset?

public:
    LightSphere(double radius);
    LightSphere(Point3D const& pos, double radius);

    void getSubtendedDir(Point3D const& viewPoint,
                         double uniformX, double uniformY,
                         Vector3D& dir) const;

    bool isSubtended(Point3D const& viewPoint, Vector3D const& dir) const;
    double subtendedProbability(Point3D const& viewPoint) const;

private:
    double getCosThetaMax(Vector3D& toCenter) const;

private:
    double radius_;

};

#endif // _LIGHT_VOLUME_H_
