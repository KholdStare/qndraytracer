#include "scene.h"
#include "light_volume.h"
#include "texture/material.h"
#include "mesh/obj_store.h"

#include <functional>
#include <algorithm>

SceneDagNode::SceneDagNode() : 
    obj(nullptr), mat(NULL), lightBound(NULL), bound(NULL),
    next(nullptr), parent(NULL), child(NULL),
    maxFactor(1.0), absFactor(1.0) {
    }	

SceneDagNode::SceneDagNode( SceneObject* obj, Material* mat ) : 
    obj(obj), mat(mat), lightBound(obj ? obj->getLightVolume() : nullptr),
    bound(obj ? obj->getBoundingVolume() : nullptr),
    next(nullptr), parent(NULL), child(NULL),
    maxFactor(1.0), absFactor(1.0) {
    }

SceneDagNode::~SceneDagNode() {
    // do not delete material here. Being a pointer
    // it could be shared by several nodes, so multiple deletes could occur.
    if (!obj) delete obj;
}

SceneDagNode* SceneDagNode::addChild( SceneObject* obj, Material* mat ) {
    SceneDagNode* node = new SceneDagNode( obj, mat );
    node->parent = this;

    // Add the object to the parent's child list, this means
    // whatever transformation applied to the parent will also
    // be applied to the child.
    if (!child) {
        child = node;
    }
    else {
        SceneDagNode* sibling = child;
        while (sibling->next != nullptr) {
            sibling = sibling->next;
        }
        sibling->next = node;
    }

    return node;
}

void SceneDagNode::rotate( char axis, double angle ) {
    AffineTrans3D rotation;
    const double toRadian = 2*M_PI/360.0;

    double cosAngle = cos(angle*toRadian);
    double sinAngle = sin(angle*toRadian);

    size_t firstIndex;
    size_t secondIndex;
    switch(axis) {
        case 'y':
            firstIndex = 2;
            secondIndex = 0;
            break;
        case 'z':
            firstIndex = 0;
            secondIndex = 1;
            break;
        default:
            // case x
            firstIndex = 1;
            secondIndex = 2;
    }

    rotation.A(firstIndex,firstIndex) = cosAngle;
    rotation.A(secondIndex,secondIndex) = cosAngle;
    rotation.A(firstIndex,secondIndex) = -sinAngle;
    rotation.A(secondIndex,firstIndex) = sinAngle;
    trans = trans*rotation;

    rotation.A(firstIndex,secondIndex) = sinAngle;
    rotation.A(secondIndex,firstIndex) = -sinAngle;
    invtrans = rotation*invtrans; 
}

void SceneDagNode::translate( Vector3D const& t ) {
    AffineTrans3D translation;

    translation.t = t.v;
    trans = trans*translation; 	

    translation.t = -t.v;
    invtrans = translation*invtrans; 
}

void SceneDagNode::scale( Point3D const& origin, Eigen::Vector3d factorVec ) {
    AffineTrans3D scale;

    // TODO: use eigen decomposition of matrix to find these
    maxFactor *= factorVec.maxCoeff();

    scale.A.diagonal() = factorVec;
    scale.t = origin.v - factorVec.cwiseProduct(origin.v);
    trans = trans*scale;

    factorVec << 1/factorVec(0), 1/factorVec(1), 1/factorVec(2);
    scale.A.diagonal() = factorVec;
    scale.t = origin.v - factorVec.cwiseProduct(origin.v);
    invtrans = scale*invtrans; 
}

void SceneDagNode::preprocess() {

    if (parent) {
        // Applies transformation of the current node to the absolute
        // transformations of the parents.
        // Stores these absolute transformations in the nodes
        modelToWorld = parent->modelToWorld*trans;
        worldToModel = invtrans*parent->worldToModel;
        absFactor = parent->absFactor * maxFactor;

        // adjust the bounding volume to contain object in world space
        if(lightBound) {
            lightBound->pos = modelToWorld.t;
            lightBound->scale = absFactor;
        }
        if(bound) {
            bound->pos = modelToWorld.t;
            bound->scale = absFactor;
        }
    }

    // Traverse the children.
    SceneDagNode* childPtr = child;
    while (childPtr != nullptr) {
        childPtr->preprocess();
        childPtr = childPtr->next;
    }
}

void SceneDagNode::traverse( Ray3D& ray ) const {
    // make sure ray.dir is unit length for bounding volume intersections
    ray.renormalize();
    traverseHelper(ray);
}

void SceneDagNode::traverseHelper( Ray3D& ray) const {
    SceneDagNode *childPtr;

    if (obj) {

        // Perform intersection. First check the bound
        // TODO: quit early if a closer intersection exists
        if ( !bound || bound->fastIntersect(ray.origin, ray.dir) ) {
            if (obj->intersect(ray, worldToModel, modelToWorld)) {
                ray.intersection.mat = mat;
            }
        }
    }

    // Traverse the children.
    childPtr = child;
    while (childPtr != nullptr) {
        childPtr->traverseHelper(ray);
        childPtr = childPtr->next;
    }
}


/***************************************************************************
 *                                  Scene                                  *
 ***************************************************************************/

Scene::Scene() : root_(new SceneDagNode()), areaLights_(false) {

}

Scene::~Scene() {
    delete root_;
    for (light_iter i = lights_.begin(); i != lights_.end(); ++i) {
        delete (*i);
    }
}

SceneDagNode* Scene::addObject( SceneDagNode* parent, 
        SceneObject* obj, Material* mat ) {
    SceneDagNode* node = parent->addChild(obj, mat);

    // check if we can importance sample some bounded objects
    if (mat && obj && node->lightBound) {

        // if the object itself can emit light, add it to the set of lights
        if (mat->emittance.getTexture() || !mat->emittance.getVal().isBlack()) {
            areaLights_ = true;
            emissiveNodes_.push_back(node);
        }
        // if the object can refract, sample it for light too (for caustics)
        else if (mat->isTransmissive && obj->isSolid()) {
            emissiveNodes_.push_back(node);
        }
    }

    return node;
}

Ray3D::intersection_func Scene::getIntersectionFunction() const {
    return std::bind(&SceneDagNode::traverse, root_, std::placeholders::_1);
}

void Scene::preprocess() {
    root_->preprocess();
}

void Scene::addLightSource( LightSource* light ) {
    if (!light) {
        return;
    }

    lights_.push_back(light);
}

