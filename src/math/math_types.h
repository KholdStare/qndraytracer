#ifndef _MATH_TYPES_H
#define _MATH_TYPES_H

// floating point comparison discussed here:
// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
// 
// google implementation mentioned here:
// http://stackoverflow.com/a/3423299/436025
//
// google code repository:
// http://code.google.com/p/googletest/source/browse/trunk/include/gtest/internal/gtest-internal.h
#include "../floatingcomparison.h"

#include <cmath>
#include <algorithm>
#include <array>

#include "affine_transformation.hpp"
#include "coordinate_base.hpp"

// ===========================================
typedef qnd::AffineTransformation<double, 3> AffineTrans3D;
typedef qnd::Point<double, 3> Point3D;
typedef qnd::Vector<double, 3> Vector3D;

std::ostream& operator <<(std::ostream& o, const Point3D& p); 
std::ostream& operator <<(std::ostream& o, const Vector3D& v); 
// ===========================================

/** Given a t_value and a Ray, calculate the intersection point */
inline Point3D getInterPoint(double t_value, Point3D const& o, Vector3D const& d) {
    return Point3D(o + t_value * d);
}

/**
 * Given incoming vector and surface normal, get the reflected vector direction.
 * Assumes both vectors are normalized.
 * */
inline Vector3D reflectedDir(const Vector3D& incomingDir, const Vector3D& surfaceNormal) {
    auto ln = incomingDir.dot(surfaceNormal);
    return incomingDir - (2*ln*surfaceNormal); 
}

/**
 * Comparison of two floats/doubles that takes numerical instability 
 * into account
 */
template <typename T>
inline bool areSame(T a, T b) {
    return fabs(a - b) < 5*std::numeric_limits<T>::epsilon();
}

/**
 * Given a vector @a v relative to a (0, 0, 1) normal vector n,
 * find the corresponding vector v' relative to given normal n'.
 * 
 * Since there are many solutions (rotations around n'), this finds
 * the vector given by the rotation in the plane defined by n and n'.
 */
Vector3D rotateVector(Vector3D v, Vector3D transformedNormal);

const FloatingPoint<double> FloatingZero(0.0); ///< easy to reference zero when doing floating point comparisons


#endif // _MATH_TYPES_H
