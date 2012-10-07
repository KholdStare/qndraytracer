#ifndef _INTERSECTION_H
#define _INTERSECTION_H

#include "math/math_types.h"
#include "texture/uv_map.h"

// forward declarations
class Material;


// ===========================================
/**
 * The Intersection class carries all the information about an
 * instersection of a ray with a surface.
 *
 * It is used to shade and produce secondary rays.
 */
struct Intersection {
    Intersection() : mat(nullptr),
                     none(true),
                     inside(false),
                     isSolid(false) { std::fill_n(uv.begin(), 2, 0); }

	Point3D point; ///< Location of intersection.
	Vector3D normal; ///< Normal at the intersection.

    /**
     * UV coordinates of the intersection,
     * valid only if object has UV map (?)
     */
    UVPoint uv;

	Material* mat; ///< Material at the intersection.

	/**
     * Position of the intersection along the ray.
	 * (i.e. point = ray.origin + t_value * ray.dir)
     *
     * Easy to compare several intersections to decide which one
     * is closest
     */
	double t_value;	

	bool none; ///< Set to true when no intersection has occured.

    /**
     * Set to true if intersection occurs from the inside
     * of the object ( for refraction )
     */
    bool inside;

    /**
     * Set to true if the object being intersected is solid
     * or not (e.g. a plane is not solid ) so no refraction takes place
     */
    bool isSolid;

    /**
     * Transform a given surface intersection using an affine transformation
     * @a M and its inverse @a invM.
     */
    void transform(AffineTrans3D const& M, AffineTrans3D const& invM) {
        if (!none) {
            point = M.transformPoint(point.v);
            normal = invM.invTransNorm(normal.v);
        }
    }
};
// ===========================================

#endif // _INTERSECTION_H
