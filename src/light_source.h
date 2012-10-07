#ifndef _LIGHT_SOURCE_
#define _LIGHT_SOURCE_

#include "colour.h"
#include "math/math_types.h"
#include "ray.h"

/**
 * Abstract Base class for a simple light source.
 */
class LightSource {
public:
    virtual ~LightSource();

    /**
     * Given a ray and an intersection closure, figure out final colour of ray,
     * and assign to it.
     *
     * The closure allows checking whether a particular ray intersects another object
     */
    virtual void shade( Ray3D&, Ray3D::intersection_func intersectCheck ) const = 0;
};

/**
 * A point light is defined by its position in world space and its
 * colour. Specifically relies on separate diffuse, ambient and specular
 * output colours (not physically based).
 */
class PointLight : public LightSource {
public:
    // TODO: use delegating constructors from C++11
    PointLight( Point3D pos, Colour col ) : pos_(pos), col_ambient_(col), 
    col_diffuse_(col), col_specular_(col) {}

    PointLight( Point3D pos, Colour ambient, Colour diffuse, Colour specular ) 
        : pos_(pos), col_ambient_(ambient), col_diffuse_(diffuse), 
        col_specular_(specular) {}

    void shade( Ray3D&, Ray3D::intersection_func intersectCheck ) const ;

private:
    Point3D pos_;
    Colour col_ambient_;
    Colour col_diffuse_; 
    Colour col_specular_; 
};

#endif // _LIGHT_SOURCE_
