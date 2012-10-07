
#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include "camera.h"
#include "sampling_strategy_group.h"
#include "sampling_strategy.h"
#include "cached_sampling_strategy.h"

class Ray3D;
class Scene;

/**
 * Represents a raytracer that can render scenes
 * using a camera.
 */
class Raytracer {

public:

    Raytracer();
    ~Raytracer();

    /**
     * Set scene to be rendered
     */
    void setScene(Scene const* scene);

    /**
     * Render the scene from the point of view of the @a camera
     */
    void render( Camera& cam );

    /**
     * Set the maximum number diffuse bounces to perform for rays.
     *
     * 1 : only direct lighting
     * 2 : difuse interreflection
     * 3+ : good luck waiting for the raytracer to finish :P
     */
    void setMaxDiffuse(int maxDiffuse) { maxDiffuse_ = maxDiffuse; }

    /**
     * Set the maximum number specular bounces
     *
     * A larger number will not hurt performance too much, unless
     * there are a lot of refractive objects (spawns reflecting and refractive rays)
     */
    void setMaxSpecular(int maxSpecular) { maxSpecular_ = maxSpecular; }

    /**
     * Set the number of samples to take from an area light source
     *
     * With antialiasing, 4 seems a good number
     */
    void setLightSamples(int num) { 
        lightStrategies_.sampler = UVSampler(num); 
    }

    /**
     * Set the number of samples to take when boucing diffuse rays.
     * These spread out in the hemisphere around the intersection point.
     *
     * Should be more than light samples, as sampling domain is larger.
     * 9 seems a good number.
     *
     * TODO: adding BRDF importance sampling should improve diffuse quality
     */
    void setDiffuseSamples(int num) { 
        diffuseStrategies_.sampler = UVSampler(num); 
    }

    /**
     * Return a closure to sample the colour for a ray using this raytracer
     */
    Camera::sampling_func getSamplingFunction() const;

    // public flags that can be set:

    bool sceneSignature; ///< Whether we want just want the scene signature

    // TODO: dump raw isnt used by raytracer. make output module to handle
    // all conversion of raw data to images
    bool dumpRaw; ///< Whether to dump the raw image file after rendering

private:

    /**
     * Return the colour of the ray after intersection and shading.
     *
     * Called recursively for reflection and refraction
     */
    Colour shadeRay( Ray3D& ray, int diffuseBounces = 1, int specularBounces = 3) const; 

    Colour calculateRadiance( Ray3D const& rayFromSurface, Ray3D const& rayFromViewer) const;

    /**
     * After intersection, calculate the colour of the ray by shading it
     * with all light sources in the scene.
     */
    void lightShading( Ray3D& ray, int diffuseBounces, int specularBounces ) const;

    /**
     * Use multiple-importance sampling with sampling strategies to compute
     * estimate of the ray colour
     */
    void lightWithStrategies( Ray3D& ray,
                              std::vector< CachedSamplingStrategy > const& strategies,
                              int diffuseBounces, int specularBounces ) const ;

    void setupStrategies();

    // How many bounces to do for reflections
    int maxDiffuse_;
    int maxSpecular_;

    // How many samples to take from light source
    SamplingStrategyGroup lightStrategies_;
    SamplingStrategyGroup diffuseStrategies_;

    Scene const* scene_; ///< scene to be rendered

};

#endif // _RAYTRACER_H_
