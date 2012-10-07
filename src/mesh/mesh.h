#ifndef _MESH_H_
#define _MESH_H_

#include "../scene_object.h"
#include "../bounding_volume.h"
#include "../kdtree/kd_tree.h"

class ObjStore;
class Face;

/**
 * A mesh object that can be placed in a scene.
 */
class Mesh : public BoundedObject {

public:
    /**
     * Construct a mesh from an OBJ representation
     */
    Mesh(ObjStore* obj);
    ~Mesh();

    // TODO: determine solidness? perhaps argument of constructor?
    bool isSolid() const { return false; }

private:
    /**
     * Carry out the intersctin with the mesh in model space.
     */
    void doIntersect( Point3D origin,
                      Vector3D dir,
                      Intersection& intersection ) const;

private:
    ObjStore* obj_; ///< holds vertex/face data parsed from OBJ
    BoundingBox boxBound_; ///< internal tight model-space bound
    KDTree  kd_;    ///< kd tree built up from the faces
    bool smoothNormals_; ///< keeps track whether to smooth normals or not.
};

#endif // _MESH_H_
