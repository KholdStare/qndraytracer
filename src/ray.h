#ifndef _RAY_H
#define _RAY_H

#include "intersection.h"
#include "colour.h"
#include <functional>

// ===========================================
/**
 * Structure that describes a ray in space, that can 
 * intersect other objects and carry some radiance.
 */
struct Ray3D {

    /**
     * A closure that accepts a ray and performs an intersection check.
     */
    typedef std::function<void (Ray3D&)> intersection_func;

    /**
     * Create a new ray starting at point @a p, extending in direction @a v
     */
	Ray3D( Point3D p, Vector3D v ) : origin(p), dir(v) {}

	Point3D origin; ///< Starting point of the ray
	Vector3D dir; ///< Direction of the ray

	/**
     * Intersection status, should be computed by the intersection function.
     */
	Intersection intersection;

    /**
     * Current colour of the ray, should be computed by the shading function.
     */
	Colour col;

    /**
     * Renormalizes the ray direction, normals, and t_value accordingly
     */
    void renormalize() {
        intersection.t_value *= dir.normalize();
        intersection.normal.normalize();
    }
};
// ===========================================

/**
 * Given a Ray with potentially another intersection, and a new Intersection,
 * update Ray with new one if better, otherwise set none = true on new
 * Intersection. Return true if switch occured and new is better.
 */
bool consolidateRayInter(Ray3D& r, Intersection& i);


#endif // _RAY_H
