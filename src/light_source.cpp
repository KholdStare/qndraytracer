#include <cmath>
#include "light_source.h"
#include "texture/material.h"

LightSource::~LightSource() { }

void PointLight::shade( Ray3D& ray, Ray3D::intersection_func intersectCheck ) const {

    Material* mat = ray.intersection.mat;
    
    // compute direction towards light
    Vector3D lightDir = pos_ - ray.intersection.point;
    double distance = lightDir.normalize();
    ray.col += col_ambient_ * mat->ambient.at(ray.intersection.uv);

    // cos of angle from normal to lightDir
    double cosAngle = lightDir.dot(ray.intersection.normal);

    if (cosAngle < 0) {
        return;
    }
    // shadow check
    else {
        Ray3D lightRay(ray.intersection.point, lightDir);
        intersectCheck(lightRay);

        // if we intersect an object between the light and point
        // we are considering, then it is in shadow
        if (!lightRay.intersection.none && lightRay.intersection.t_value < distance ) {
            return;
        }
    }

    // diffuse
    ray.col += col_diffuse_ * mat->diffuse.at(ray.intersection.uv) * cosAngle;

    // direction of reflection of light ray
    Vector3D reflectDir = 2*cosAngle*ray.intersection.normal - lightDir;
    reflectDir.normalize();
    // cos of angle from reflection to ray back to viewer
    double rv = -reflectDir.dot(ray.dir);

    // specular
    if (rv > 0) {
        ray.col += col_specular_ * mat->specular.at(ray.intersection.uv)
                                 * pow(rv, mat->specular_exp);
    }
}

