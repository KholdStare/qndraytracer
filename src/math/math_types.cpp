#include "math_types.h"
#include <iostream>

std::ostream& operator <<(std::ostream& s, const Point3D& p)
{
    return s << "p(" << p[0] << "," << p[1] << "," << p[2] << ")";
}

std::ostream& operator <<(std::ostream& s, const Vector3D& v)
{
    return s << "v(" << v[0] << "," << v[1] << "," << v[2] << ")";
}


// Implemented according to http://en.wikipedia.org/wiki/Axis_angle#Rotating_a_vector
Vector3D rotateVector(Vector3D v, Vector3D transformedNormal) {
    const Vector3D normal(0.0, 0.0, 1.0);
    double cosAngle = normal.dot(transformedNormal);

    // check if normals are colinear
    if (areSame(cosAngle, 1.0)) {
        return v;
    }
    else if (areSame(cosAngle, -1.0)) {
        return Vector3D(v[0], v[1], -v[2]);
    }

    Vector3D axisOfRotation(normal.cross(transformedNormal));
    double sinAngle = axisOfRotation.normalize();

    return (cosAngle*v)
         + (sinAngle*(axisOfRotation.cross(v)))
         + ((1.0 - cosAngle) * axisOfRotation.dot(v) * axisOfRotation);
}

