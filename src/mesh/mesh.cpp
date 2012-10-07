#include "mesh.h"
#include "obj_store.h"
#include "../light_volume.h"
#include <iostream>

Mesh::Mesh(ObjStore* obj) : BoundedObject(new LightSphere(obj->largestCoord())),
                            obj_(obj),
                            boxBound_(obj->minPoint(), obj->maxPoint()),
                            smoothNormals_(obj->smoothNormals) {

    // preprocess OBJ data to generate face structs
    obj_->generateFaces();

    // build a KD tree from the faces of the mesh
    kd_.build(obj->getFaces(), boxBound_);

    // TODO: assert?
    std::cout << "Total faces: " << obj->numFaces()
              << " kd faces: " << kd_.countTotalFaces()
              << " depth: " << kd_.depth()
              << " max leaf faces: " << kd_.maxLeafObjects()
              << std::endl;
                            
}
Mesh::~Mesh() { }


void Mesh::doIntersect( Point3D origin, Vector3D dir, Intersection& intersection ) const {

    // check model-space tight bound
    double length = dir.normalize();
    if ( !boxBound_.fastIntersect(origin, dir) ) {
        return;
    }
    
    FaceIntersection faceInter;

    // traverse the kd tree to find suitable intersections
    kd_.traverse(origin, dir, faceInter);

    // since no intersection, exit early
    if (!faceInter.face) {
        return;
    }

    // Populate intersection
    intersection.none = false;
    intersection.t_value = faceInter.t_value;
    intersection.point = getInterPoint(intersection.t_value, origin, dir);

    // TODO: normals for each face could be stored separately so

    // Find normal at intersection point
    Face const& face = *faceInter.face;
    if (smoothNormals_) {
        Vector3D const& n0 = face.vertices[0].normal;
        Vector3D const& n1 = face.vertices[1].normal;
        Vector3D const& n2 = face.vertices[2].normal;

        // will be renormalized later when needed
        intersection.normal = n0 + faceInter.s * (n1 - n0) + faceInter.t * (n2 - n0);
    }
    else {
        intersection.normal = face.normal;
    }

    // future TODO: UV coordinated from s,t? need uv coords of vertices

    // readjust t_value since we normalized the dir vector
    intersection.t_value /= length;
}

