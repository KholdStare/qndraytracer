
#include "scene_object.h"
#include "light_volume.h"

#include "math/math_types.h"
#include "ray.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <algorithm>

SceneObject::~SceneObject() { }

// A method that transforms the ray into object space
// and calls the derived class' intersect implementation that
// works strictly in model space. Checks are done to select
// the closest t_value
bool SceneObject::intersect( Ray3D& ray, const AffineTrans3D& worldToModel,
        const AffineTrans3D& modelToWorld ) const {
    Intersection intersection;

    // delegate to specific implementation, in object space
    // TODO: also ensure dir is unit length, and compensate here
    doIntersect(worldToModel.transformPoint(ray.origin.v),
                worldToModel.transformVector(ray.dir.v), intersection);

    // transform the intersection back into world space
    intersection.transform(modelToWorld, worldToModel);

    // if the object is solid, check that flag in the intersection,
    // which is needed for the refraction
    intersection.isSolid = this->isSolid();

    // given new intersection see if it's better than one already present
    return consolidateRayInter(ray, intersection);
}

// ==========================

BoundedObject::BoundedObject(LightVolume* bound) : lightBound_(bound), bound_(bound) { }
BoundedObject::BoundedObject(LightVolume* lightBound, BoundingVolume* bound) 
                                : lightBound_(lightBound), bound_(bound) { }
BoundedObject::~BoundedObject() {
    delete bound_;
    // delete other bound if pointing to a different object
    if (bound_ != lightBound_) {
        delete lightBound_;
    }
}

// ==========================

void UnitSquare::doIntersect( Point3D origin, Vector3D dir, Intersection& intersection ) const {

    // if ray is parallel, no solution exists
    // also, only one side can be intersected
    // useful for see-through walls
    if (areSame(dir[2], 0.0) || origin[2] < 0.0) {
        return;
    }

    // intersect
    intersection.t_value = - origin[2] / dir[2];

    intersection.point = getInterPoint(intersection.t_value, origin, dir);
    double x = intersection.point[0];
    double y = intersection.point[1];

    // check if intersection point is within bounds of square
    if (x <= 0.5 && x >= -0.5 &&
        y <= 0.5 && y >= -0.5) {
        intersection.none = false;
        // populate uv coordinates
        intersection.uv[0] = x+0.5;
        intersection.uv[1] = y+0.5;
    }
    else { // quit early
        return;
    }

    // update the normal direction 
    intersection.normal[2] = 1.0;
}

UnitSquare::UnitSquare() : BoundedObject(new LightSphere(sqrt(2)/2.0)) { }

// ==========================
// UnitCube::UnitCube(): BoundedObject(new LightSphere(sqrt(3)/2.0),
//                                     new BoundingBox(Point3D(-0.5, -0.5, -0.5) * sqrt(3),
//                                                     Point3D(0.5, 0.5, 0.5)    * sqrt(3))) { }

UnitCube::UnitCube(): BoundedObject(new LightSphere(sqrt(3)/2.0) ) { }

void UnitCube::doIntersect( Point3D origin,
                      Vector3D dir,
                      Intersection& intersection ) const {

    // Box ray intersection with slabs adapted from:
    // http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

    double lambdaNear = -std::numeric_limits<double>::infinity();
    double lambdaFar  = std::numeric_limits<double>::infinity();
    int intersectionDim = 0;

    // go through each dimension and intersect with the
    // associated slabs
    for (int dim = 0; dim < 3; ++dim) {

        if (areSame(dir[dim], 0.0)) {
            // if ray parallel to slab and starts outside of slab,
            // then intersection cannot happen
            if (origin[dim] > 0.5 || origin[dim] < -0.5) {
                return;
            }
            // if ray is parallel but starts inside slab, there are no
            // intersections with this specific slab, so continue
            continue;
        }

        // get intersections of both sides of the slab
        double lambda1 = (0.5 - origin[dim])/dir[dim];
        double lambda2 = -(0.5 + origin[dim])/dir[dim];

        if (lambda1 > lambda2) {
            std::swap(lambda1, lambda2);
            /* since lambda1 intersection with near plane */
        }

        if (lambda1 > lambdaNear) { // want largest lambdaNear
            lambdaNear = lambda1;
            intersectionDim = dim; // save dimension with intersection
        }

        if (lambda2 < lambdaFar) lambdaFar = lambda2; /* want smallest lambdafar */

        if (lambdaNear > lambdaFar) return; // ray outside

        if (lambdaFar < 0) return; // all intersections behind ray

    }

    // if we reach this point, then a valid intersection has been found.
    intersection.none = false;

    // calculate normal
    // TODO: do some bittwidling, to just use sign bit
    if (dir[intersectionDim] < 0.0) {
        intersection.normal[intersectionDim] = 1.0;
    }
    else {
        intersection.normal[intersectionDim] = -1.0;
    }

    if (lambdaNear < 15*std::numeric_limits<double>::epsilon()) {
        intersection.t_value = lambdaFar;
        intersection.inside = true;
    }
    else {
        intersection.t_value = lambdaNear;
    }

    intersection.point = getInterPoint(intersection.t_value, origin, dir);

    // get UV coordinates
    // go through dimensions, and for those that are not
    // the dimension of the slab, use values from intersection point
    // for uv coord
    int index = 0;
    for (int d = 0; d < 3; ++d) {
        // skip the intersection dimension
        if (d == intersectionDim) {
            continue;
        }

        intersection.uv[index] = intersection.point[d] + 0.5;
        ++index;
    }
}

// ==========================
UnitSphere::UnitSphere() : BoundedObject(new LightSphere(1)) { }

void UnitSphere::doIntersect( Point3D origin, Vector3D dir, Intersection& intersection ) const {

    double l = dir.normalize();
    double a = -dir.dot(origin);
    double discriminant = a*a - origin.squaredNorm() + 1; // 1 is r^2

    // check the discriminant

    // no solutions, therefore no intersections
    if (discriminant < std::numeric_limits<double>::epsilon()) {
        return;
    }
    // single solution
    else if (FloatingPoint<double>(discriminant).AlmostEquals(FloatingZero)) {
        intersection.t_value = a;
    }
    // two solutions, so pick the correct one
    else {
        double d = sqrt(discriminant);

        // if furthest intersection is behind the origin then
        // we can stop
        if (a+d < 300*std::numeric_limits<double>::epsilon()) {
            return;
        }
        
        double t = a - d;
        // if closest intersection is behind origin, then 
        // we are inside the sphere
        if (t < 300*std::numeric_limits<double>::epsilon()) {
            // set t to the intersection in front of origin
            t = a + d;
            intersection.inside = true;
        }

        intersection.t_value = t;
    }

    intersection.none = false;
    intersection.point = getInterPoint(intersection.t_value, origin, dir);

    // set uv coordinates
    intersection.uv[0] = (std::atan2(intersection.point[1], intersection.point[0])/M_PI + 1) / 2;
    intersection.uv[1] = (intersection.point[2] + 1) / 2;

    // set normal
    if (intersection.inside) {
        // if inside the sphere, intersection normal is inverted
        intersection.normal = -intersection.point;
    }
    else {
        intersection.normal = intersection.point;
    }

    // convert t_value to be used with original ray dir, for comparison
    // of intersections
    intersection.t_value /= l;
}
