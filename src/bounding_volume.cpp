#include "bounding_volume.h"


BoundingVolume::BoundingVolume(Point3D const& p) : pos(p), scale(1.0) { }
BoundingVolume::BoundingVolume() : pos(), scale(1.0) { }
BoundingVolume::~BoundingVolume() { }


BoundingSphere::BoundingSphere(double radius) : radius_(radius) { }
BoundingSphere::BoundingSphere(Point3D const& pos, double radius) : BoundingVolume(pos),
    radius_(radius) { }

bool BoundingSphere::fastIntersect(Point3D const& viewPoint, Vector3D const& dir) const {

    Point3D origin(this->pos[0] - viewPoint[0],
                   this->pos[1] - viewPoint[1],
                   this->pos[2] - viewPoint[2]);

    double a = dir.dot(origin);
    double trueRadius = radius_ * this->scale;
    double discriminant = a*a - origin.squaredNorm() + trueRadius*trueRadius;

    // TODO: return nearest t_value? if t_value is larger than current best,
    // can ignore object completely when used for acceleration

    // return true if discriminant is non-negative, ie. a solutions exist
    return discriminant > 500*std::numeric_limits<double>::epsilon();
}

BoundingBox::BoundingBox(Point3D const& minPoint, Point3D const& maxPoint) 
    : minPoint_(minPoint), maxPoint_(maxPoint) { }
BoundingBox::BoundingBox(Point3D const& pos, Point3D const& minPoint, Point3D const& maxPoint)
    : BoundingVolume(pos) , minPoint_(minPoint), maxPoint_(maxPoint) { }

bool BoundingBox::fastIntersect(Point3D const& viewPoint, Vector3D const& dir) const {

    double near, far;
    return fastIntersect(viewPoint, dir, near, far);
}

bool BoundingBox::fastIntersect(Point3D const& viewPoint, Vector3D const& dir,
                       double& tNear, double&tFar) const {

    Point3D origin( viewPoint[0] - this->pos[0],
                    viewPoint[1] - this->pos[1],
                    viewPoint[2] - this->pos[2]);
    // Box ray intersection with slabs adapted from:
    // http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

    tNear = -std::numeric_limits<double>::infinity();
    tFar  = std::numeric_limits<double>::infinity();

    // TODO: much of the code is common with UnitCube scene object

    // go through each dimension and intersect with the
    // associated slabs
    for (int dim = 0; dim < 3; ++dim) {
        // find the minimum and maximum values of box for this dimension
        double minDimVal = minPoint_[dim]*scale;
        double maxDimVal = maxPoint_[dim]*scale;

        if (areSame(dir[dim], 0.0)) {
            // if ray parallel to slab and starts outside of slab,
            // then intersection cannot happen
            if (origin[dim] > maxDimVal || origin[dim] < minDimVal) {
                return false;
            }

            // if ray is parallel but starts inside slab, there are no
            // intersections with this specific slab, so continue
            continue;
        }

        // get intersections of both sides of the slab
        double t1 = (maxDimVal - origin[dim])/dir[dim];
        double t2 = (minDimVal - origin[dim])/dir[dim];

        if (t1 > t2) {
            std::swap(t1, t2);
            /* since t1 intersection with near plane */
        }

        if (t1 > tNear) { tNear = t1; } // want largest tNear 

        if (t2 < tFar) { tFar = t2; } // want smallest tfar

        if (tNear > tFar)  { return false; } // ray outside

        if (tFar < 0) { return false; } // all intersections behind ray
    }

    // if we reach this point, then a valid intersection has been found.
    return true;
}

