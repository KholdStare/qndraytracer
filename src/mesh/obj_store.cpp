#include "obj_store.h"

namespace {
    const double inf = std::numeric_limits<double>::infinity();
}

ObjStore::ObjStore() : invertNormals(false),
                       smoothNormals(false),
                       minPoint_(inf, inf, inf),
                       maxPoint_(-inf, -inf, -inf),
                       largest_(0) {

}

void ObjStore::addVertex(Point3D const& v) {
    vertices_.push_back(VertexInfo(v));

    // update sum for mean calculation
    sum_ += v;

    // check if largest coordinate
    double norm = sqrt(v.squaredNorm());
    if (norm > largest_) {
        largest_ = norm;
    }

    // update min and max points for box bound
    for (int dim = 0; dim < 3; ++dim) {
        // check min
        if (v[dim] < minPoint_[dim]) {
            minPoint_[dim] = v[dim];
        }

        // check max
        if (v[dim] > maxPoint_[dim]) {
            maxPoint_[dim] = v[dim];
        }
    }
}

void ObjStore::addFace(TriangleIndices const& f) {
    int faceIndex = faceIndices_.size();
    faceIndices_.push_back(f);

    // go through vertex indeces, and add this face as
    // a parent.
    for (auto vertexIndex : f) {
        vertices_[vertexIndex].parentFaces.push_back(faceIndex);
    }

}

int ObjStore::getVertexIndex(TriangleIndices const& f, int i) {

    // reverse indeces if normals are the other way
    if (invertNormals) {
        i = (2-i) % 3;
    }

    return f[i];
}

void ObjStore::generateFaces() {

    // generate the normals for each vertex
    generateNormals();

    // clear any faces that were generated previously
    faces_.clear();

    // given that we know normals for each face as well as
    // normals for each point, it should be trivial to combine
    // the data into the final Face struct

    // go through indeces that define each face
    for (unsigned int faceIndex = 0; faceIndex < faceIndices_.size(); ++faceIndex) {
        TriangleIndices const& f = faceIndices_[faceIndex];

        Face face;
        // fetch the normal precalculated for that face
        face.normal = faceNormals_[faceIndex];

        // and find the points and normals for the vertices
        // of the face
        for (int i = 0; i < 3; ++i) {
            int vertexIndex = getVertexIndex(f, i);
            face.vertices[i].point = vertices_[vertexIndex].point;
            face.vertices[i].normal = vertexNormals_[vertexIndex];
        }

        faces_.push_back(face);
    }
}

void ObjStore::generateNormals() {
    faceNormals_.clear(); vertexNormals_.clear();

    // first generate the normals for each face
    for (TriangleIndices const& f : faceIndices_) {

        // determine face normal if vertices are defined counter clock-wise

        Point3D a = vertices_.at(f[0]).point;
        Point3D b = vertices_.at(f[1]).point;
        Point3D c = vertices_.at(f[2]).point;
        // invert the normal if needed
        if (invertNormals) {
            a = vertices_.at(f[2]).point;
            b = vertices_.at(f[1]).point;
            c = vertices_.at(f[0]).point;
        }
        Vector3D edgeA(c - a);
        Vector3D edgeB(b - a);

        // compute normal for this face
        Vector3D normal = edgeA.cross(edgeB);
        normal.normalize();

        // add the normal to a list for faces
        faceNormals_.push_back(normal);
    }


    // then generate an interpolated normal for each vertex.
    // interpolate between the normals of each parent face of the vertex
    for (VertexInfo const& vInfo : vertices_) {

        Vector3D interpolatedNormal(0, 0, 0);

        // go through the normals of parent faces
        typedef std::vector< int >::const_iterator iter; 
        for (int parentIndex : vInfo.parentFaces) {
            // add them up
            interpolatedNormal += faceNormals_[parentIndex];
        }
        // and average them
        interpolatedNormal.normalize();

        // use this normal for the vertex normal
        vertexNormals_.push_back(interpolatedNormal);
    }

}

Point3D ObjStore::mean() const {
    if (vertices_.size() == 0) {
        return Point3D();
    }

    return sum_ / double(vertices_.size());
}

double ObjStore::radiusFromMean() const {
    Point3D m = this->mean();

    double radius = 0;

    // go through all vertices, check the distance to mean
    // and find the furthest radius
    for (VertexInfo const& vInfo : vertices_) {
        double dist = (vInfo.point - m).norm();
        if (dist > radius) {
            radius = dist;
        }
    }

    return radius;
}
