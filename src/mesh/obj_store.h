#ifndef _OBJ_STORE_H_
#define _OBJ_STORE_H_

#include "face.h"

#include <vector>
#include <array>

/**
 * Stores data from an OBJ file
 *
 * Currently very limited subset.
 * TODO: rename to mesh store and have intersection method here?
 */
class ObjStore {

public:
    ObjStore();

    // insertion methods can be slow with lots of preprocessing,
    // so later requests are fast.

    void addVertex(Point3D const& v);

    /**
     * Add a (triangular) face to this mesh. 
     * 
     * The face is represented as indices of previously added vertices.
     */
    void addFace(TriangleIndices const& f);

    int numVertices() const { return vertices_.size(); }
    int numFaces()    const { return faceIndices_.size(); }

    /**
     * Generate final face objects.
     *
     * Since faces are passed in as arrays of vertex indices,
     * To generate a Face structure, some preprocessing
     * is required. Call this after al vertices/faces have been
     * added.
     */
    void generateFaces();

    /**
     * @return a vector of all faces (after being generated).
     */
    std::vector< Face > const& getFaces() { return faces_; }

    // Useful functions to determine bounds of the mesh
    Point3D const& minPoint() { return minPoint_; }
    Point3D const& maxPoint() { return maxPoint_; }

    /**
     * @return the mean position of all vertices
     */
    Point3D mean() const;

    /**
     * @return largest coordinate relative to the mean.
     *
     * Can be used for bounding sphere centered at mean.
     */
    double radiusFromMean() const;

    /**
     * @returns the value of the absolute largest coordinate.
     *
     * Can be used to define a naive bounding sphere around origin
     */
    double largestCoord() const { return largest_; }

    // Configuration fields.

    bool invertNormals; ///< controls whether normals are inverted or not
    bool smoothNormals; ///< controls whether normals are smoothed (phong)

private:

    void generateNormals(); ///< helper method that generates normals first

    /**
     * Holds a vertex and indeces to any faces
     * that it is part of
     */
    struct VertexInfo {
        VertexInfo(Point3D const& p) : point(p) { }

        Point3D point;
        std::vector< int > parentFaces; ///< indeces into faces that contain the vertex
    };

    /**
     * @return the vertex index of the ith vertex of face f
     * taking into account normal orientation.
     *
     * In this case, ith vertex in the face can vary depending
     * on normal orientation.
     */
    int getVertexIndex(TriangleIndices const& f, int i);

    std::vector< VertexInfo > vertices_; ///< collection of all vertices in mesh

    /** Collection of faces, where each face indexes into vertex collection */
    std::vector< TriangleIndices > faceIndices_;

    // members below are generated after all points and faces have been inserted

    std::vector< Vector3D > faceNormals_;   ///< collection of normals for each face
    std::vector< Vector3D > vertexNormals_; ///< interpolated normals of parent faces
    std::vector< Face > faces_;             ///< final face collection

    Point3D sum_;       ///< sum of all vertices
    Point3D minPoint_;  ///< a point in space with the smallest coefficients of all vertices
    Point3D maxPoint_;  ///< a point in space with the largest coefficients of all vertices
    double largest_;    ///< largest distance from origin


};


#endif // _OBJ_STORE_H_
