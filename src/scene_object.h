#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#include "math/math_types.h"


class Intersection;
class Ray3D;
class BoundingVolume;
class LightVolume;
class UVMap;

/**
 * All primitives should provide an intersection function.  
 *
 * To create more primitives, inherit from SceneObject,
 * and implement the doIntersect method.
 */
class SceneObject {
public:
    virtual ~SceneObject();

    /**
     * @Return true if an intersection occured, false otherwise.
     */
    bool intersect( Ray3D&, const AffineTrans3D&, const AffineTrans3D& ) const;

    virtual BoundingVolume* getBoundingVolume() const { return nullptr; }
    virtual LightVolume*    getLightVolume()    const { return nullptr; }

    /**
     * @return whether the object has any volume (when considering transmission)
     */
    virtual bool isSolid() const = 0;

private:
    /**
     * Private method that operates strictly in model space.
     *
     * Modify the Intersection object that is passed in accordingly.
     * All transformations and comparisons to other intersections along
     * the ray will be done in SceneObject::intersect, so only worry
     * about finding an intersection.
     */
    virtual void doIntersect( Point3D origin,
                              Vector3D dir,
                              Intersection& intersection ) const = 0;
};

/**
 * Class for ease of management of bounding volume
 */
class BoundedObject : public SceneObject {

public:
    // take ownership of the bounding volume object
    BoundedObject(LightVolume* lightBound, BoundingVolume* bound);
    BoundedObject(LightVolume* lightBound);
    virtual ~BoundedObject();

    BoundingVolume* getBoundingVolume() const { return bound_; }
    LightVolume*    getLightVolume() const { return lightBound_; }

private:
    LightVolume*    lightBound_;
    BoundingVolume* bound_;
};


/**
 * A simple unit square on the x-y plane.
 */
class UnitSquare : public BoundedObject {

public:
    UnitSquare();
    bool isSolid() const { return false; }

private:
    void doIntersect( Point3D origin,
                      Vector3D dir,
                      Intersection& intersection ) const;
};

/**
 * Unit cube centered at the origin
 */
class UnitCube : public BoundedObject {

public:
    UnitCube();
    bool isSolid() const { return true; }

private:
    void doIntersect( Point3D origin,
                      Vector3D dir,
                      Intersection& intersection ) const;
};

/**
 * Unit Sphere centered at the origin
 */
class UnitSphere : public BoundedObject {
public:
    UnitSphere();

    bool isSolid() const { return true; }

private:
    void doIntersect( Point3D origin,
                      Vector3D dir,
                      Intersection& intersection ) const;
};

#endif // _SCENE_OBJECT_H_
