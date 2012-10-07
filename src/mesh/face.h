#ifndef _FACE_H_
#define _FACE_H_

#include "../math/math_types.h"
#include "../intersection.h"
#include <array>

/**
 * A single vertex on a mesh face.
 */
struct Vertex {
    Point3D point;   ///< coordinate of vertex
    Vector3D normal; ///< normal to the surface at vertex
};

/**
 * A collection of indeces referring to indeces
 * of vertices in some other container
 */
typedef std::array<size_t, 3> TriangleIndices;
typedef std::array<Vertex, 3> TriangleVertices;

/**
 * A structure that represents a triangular face on a mesh.
 */
struct Face {
    TriangleVertices vertices;  ///< array of vertices
    Vector3D normal;            ///< outward normal to the plane of the triangle
};

/**
 * Intersection struct used for faces. After the best is found,
 * there is enough information to do extra calculations, like smooth
 * normals, instead of doing it for every face.
 */
struct FaceIntersection {
    // Using C++11 default member initialization.

    /**
     * Face that has beeen successfully intersected.
     * Is nullptr, if no intersection exists.
     */
    Face const* face = nullptr;

    /** Parameter that determines length of ray to intersection */
    double t_value = std::numeric_limits<double>::infinity();
    double s = 0, t = 0;    ///< Parameters giving position of intersection within the triangle
};

/**
 * Intersect ray starting at @a origin, extending in direction @a dir,
 * with a triangle @a face.
 *
 * @return true if an intersection exist, and populate the @a intersection structure.
 * @return false otherwise.
 */
bool intersectFace(Face const& face,
                   Point3D const& origin,
                   Vector3D const& dir,
                   FaceIntersection& intersection);

#endif // _FACE_H_
