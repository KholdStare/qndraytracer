#include "face.h"

bool intersectFace(Face const& face,
                   Point3D const& origin,
                   Vector3D const& dir,
                   FaceIntersection& intersection) {

    // backface culling
    if (dir.dot(face.normal) >= 0) {
        return false;
    }

    // intersect with plane to get point
    Point3D planeOrigin = face.vertices[0].point;

    double t_value = - (face.normal.dot(origin - planeOrigin)/face.normal.dot(dir));

    // don't check bounds, if intersection is further than the current best or
    // behind ray origin
    if (t_value > intersection.t_value || t_value < 0) {
        return false;
    }

    // ray-triangle instersection adapted from
    // http://www.softsurfer.com/Archive/algorithm_0105/algorithm_0105.htm
    Point3D p = getInterPoint(t_value, origin, dir);

    Vector3D u(face.vertices[1].point - face.vertices[0].point);
    Vector3D v(face.vertices[2].point - face.vertices[0].point);
    // position of p relative to first vertex
    Vector3D w(p - face.vertices[0].point);

    auto uu = u.squaredNorm();
    auto uv = u.dot(v);
    auto vv = v.squaredNorm();
    auto wu = w.dot(u);
    auto wv = w.dot(v);
    double denominator = uv*uv - uu*vv;

    // calculate s,t: parameterization of point p in terms of u,v
    // if values outside [0,1], then p is outside of triangle bounds
    double s = (uv*wv - vv*wu) / denominator;
    if (s < 0.0 || s > 1.0) {
        return false;
    }
    double t = (uv*wu - uu*wv) / denominator;
    if (t < 0.0 || t+s > 1.0) {
        return false;
    }

    intersection.face = &face;
    intersection.t_value = t_value;
    intersection.s = s;
    intersection.t = t;
    return true;
}
