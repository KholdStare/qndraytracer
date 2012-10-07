#ifndef _SCENE_H_
#define _SCENE_H_

#include "scene_object.h"
#include "light_source.h"
#include "texture/texture.h"
#include "texture/texture_storage.h"
#include "texture/bmp_image.h"
#include "template_utils.h"

#include <vector>
#include <set>

class SceneObject;
class LightVolume;
class ObjStore;

/**
 * A node of the scene tree, containing objects in the scene.
 *
 * Contains the object (optional), transformations relative to the
 * parent, as well as any bounding volumes
 */
class SceneDagNode {
    // TODO: can't really be a DAG... should rename

public:
    SceneDagNode();
    SceneDagNode( SceneObject* obj, Material* mat );

    ~SceneDagNode();

    /**
     * Apply rotation about axis 'x', 'y', 'z' angle degrees to node.
     */
    void rotate( char axis, double angle );

    /**
     * Apply translation in the direction of @a trans to node.
     */
    void translate( Vector3D const& trans );

    /**
     * Apply scaling about a fixed point origin.
     */
    void scale( Point3D const& origin, Eigen::Vector3d factorVec );

    /*********************************
     *  Following are used by scene  *
     *********************************/
    
    SceneDagNode* addChild( SceneObject* obj, Material* mat );

    // Traversal code for the scene graph, the ray is transformed into 
    // the object space of each node where intersection is performed.
    // Ray will contain the closest intersection if one exists
    void traverse( Ray3D& ray) const;

    /**
     * Preprocess this node and all children.
     *
     * This calculates the absolute transformations, given the
     * relative transformations
     */
    void preprocess();


    SceneObject* obj; ///< Geometry primitive, used for intersection.
    Material* mat; ///< Material of the object, used in shading.

    /**
     * Bounding volume used for calculating light emission.
     *
     * This bounding volume is usually no as tight, but makes 
     * light calculations easier.
     */
    LightVolume* lightBound; 

    /**
     * A bounding volume used to accelerate intersections.
     *
     * Is optional, but should be tighter than light bound,
     * as calculations are simpler. If no bounding volume
     * provided, lightBound is used.
     */
    BoundingVolume* bound;

private:
    /**
     *  Internal helper for recursing. ASSUMPTION: ray.dir is unit length.
     */
    void traverseHelper( Ray3D& ray) const; 

private:
    SceneDagNode* next; ///< points to next sibling in tree
    SceneDagNode* parent; ///< points to parent node
    SceneDagNode* child; ///< points to first child

    // Each node maintains a transformation matrix, which maps the 
    // geometry from object space to parent node space and the inverse.
    AffineTrans3D trans; ///< Affine transformation from parent node to this node
    AffineTrans3D invtrans; ///< Affine transformation from this node to parent node
    
    // Each node also maintains an absolute transformation matrix,
    // which maps the geometry from object space to absolute world space.
    AffineTrans3D modelToWorld;
    AffineTrans3D worldToModel;

    // Keeps track of the largest scaling factor applied to this node
    double maxFactor;
    // Same as above, but is the absolute value- the product of all
    // factors from the root to this node
    double absFactor;
};
// TODO: store tree in a vector to make objects local? Better for cache


/**
 * Stores objects, lights and their associated materials in a scene
 */
class Scene {

public:
    typedef std::vector<SceneDagNode*>::const_iterator emissive_iter;
    typedef std::vector<LightSource*>::const_iterator light_iter;

    Scene();
    ~Scene();

    /**
     * Add an object @a obj into the scene, with material @a mat. The method
     * returns a handle to the node just added, use the 
     * handle to apply transformations to the object.
     */
    SceneDagNode* addObject( SceneObject* obj, Material* mat ) {
        return addObject(root_, obj, mat);
    }

    /**
     * Add an object into the scene with a specific parent node.
     * Nodes with NULL obj and mat, represent just transformations.  
     */
    SceneDagNode* addObject( SceneDagNode* parent, SceneObject* obj, 
            Material* mat );

    /**
     * Add a simple light source to the scene.
     *
     * If area lights are desired, just add an object with an
     * emissive material.
     */
    void addLightSource( LightSource* light );

    // Following methods return maps from std::string s to pointers
    // of a particular type
    PointerMap<Image<RGBA> >& getImageStorage() { return images_; }
    PointerMap<Material >& getMaterialStorage() { return materials_; }
    PointerMap<ObjStore >& getMeshStorage() { return meshes_; }

    /**
     * A container of different TextureStorage types.
     *
     * To add more texture types, just append more types to the template below 
     * For each TextureStorage type, there is a getter method.
     *
     * e.g. get<Colour>() will return TextureStorage<Colour>&
     */
    typedef qnd::TypeWrapperTuple<TextureStorage, Colour, double> TextureStorageTuple;
    TextureStorageTuple textureStorage;


    /**
     * Calculate the absolute worldToModel and modelToWorld matrices for the scene.
     *
     * Please call this before rendering a frame.
     */
    void preprocess();
    
    /**
     * Traversal method for the scene.
     *
     * The ray is transformed into the object space of each node where the
     * intersection is performed. Ray will contain the closest intersection if one exists
     */
    void traverse( Ray3D& ray) const {
        root_->traverse(ray);
    }

    /**
     * Return a closure that can be used to check for intersections
     * with objects along a ray.
     */
    Ray3D::intersection_func getIntersectionFunction() const;

    // iterator to iterate through emissive nodes
    emissive_iter emissive_begin() const { return emissiveNodes_.begin(); }
    emissive_iter emissive_end() const { return emissiveNodes_.end(); }

    // iterator to iterate through simple lights
    light_iter light_begin() const  { return lights_.begin(); }
    light_iter light_end() const    { return lights_.end(); }

    /**
     * Return whether the scene contains any area lights.
     * 
     * Used to optimize raytracing
     */
    bool hasAreaLights() const { return areaLights_; }
    
private:
    typedef std::set<Material*>::const_iterator mat_iter;

    std::vector<LightSource*> lights_; ///< vector for managing simple lights

    SceneDagNode *root_; ///< Scene graph.

    PointerMap<Image<RGBA> > images_; ///< image database

    PointerMap<Material> materials_; ///< material database
    
    PointerMap<ObjStore> meshes_; ///< mesh database

    /**
     * vector for managing emissive objects, and their nodes
     */
    std::vector<SceneDagNode*> emissiveNodes_;

    bool areaLights_;
};

#endif // _SCENE_H_
