#ifndef _BOUNDING_VOLUME_H_
#define _BOUNDING_VOLUME_H_

#include "math/math_types.h"

/**
 * A bounding volume around an object, to be be used for culling intersections
 *
 * The volume can be positioned and scaled uniformly using public member variables,
 * before calling the fastIntersect method.
 */
class BoundingVolume {

public:
    BoundingVolume();
    BoundingVolume(Point3D const& pos);
    virtual ~BoundingVolume();

    /**
     * Fast intersection method
     * ASSUMPTION: dir is unit length */
    virtual bool fastIntersect(Point3D const& viewPoint, Vector3D const& dir) const = 0;

    // publicly accessible members for positioning volume.
    Point3D pos; ///< position in absolute world space relative to which this bounding volume will be defined
    double scale; ///< the scaling of the bounding volume relative to the position pos

};


/**
 * A simple spherical bounding volume
 */
class BoundingSphere : public BoundingVolume {

public:
    BoundingSphere(double radius);
    BoundingSphere(Point3D const& pos, double radius);

    bool fastIntersect(Point3D const& viewPoint, Vector3D const& dir) const;

private:
    double radius_; ///< radius of sphere

};

/**
 * An axis-aligned bounding box
 */
class BoundingBox : public BoundingVolume {

public:
    BoundingBox(Point3D const& minPoint, Point3D const& maxPoint);
    BoundingBox(Point3D const& pos,
                Point3D const& minPoint, Point3D const& maxPoint);

    bool fastIntersect(Point3D const& viewPoint, Vector3D const& dir) const;

    // fast intersection that returns nearest and furthest intersection points
    bool fastIntersect(Point3D const& viewPoint, Vector3D const& dir,
                       double& tNear, double&tFar) const;

    Point3D minPoint() const { return minPoint_; }
    Point3D maxPoint() const { return maxPoint_; }

private:
    Point3D minPoint_; ///< the 3D coordniates of the corner with the smallest coefficients of the box
    Point3D maxPoint_; ///< the 3D coordniates of the corner with the largest coefficients of the box

};

#endif // _BOUNDING_VOLUME_H_
