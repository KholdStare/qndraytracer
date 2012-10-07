#include "ray.h"

#include "scene.h"
#include "scene_object.h"
#include "light_source.h"
#include "raytracer.h"
#include "bounding_volume.h"
#include "sampling_strategy.h"
#include "fresnel.h"
#include "texture/material.h"

#include <omp.h>
#include <cmath>
#include <functional>

Raytracer::Raytracer() : sceneSignature(false), 
                         dumpRaw(false), 
                         maxDiffuse_(2),
                         maxSpecular_(3),
                         lightStrategies_(9),
                         diffuseStrategies_(16),
                         scene_(nullptr) {
}

Raytracer::~Raytracer() { }

// given params, calculate outgoing light after a reflection from surface
Colour Raytracer::calculateRadiance( Ray3D const& rayFromSurface, Ray3D const& rayFromViewer) const {

    double cosIn = rayFromSurface.dir.dot(rayFromViewer.intersection.normal);

    // only calculate radiance if light is not arriving from behind the
    // surface
    if (cosIn > 0) {
        // multiplication by 2 is normalization for cos factor.
        // this is the integrand of the rendering equation
        return 2 * cosIn * rayFromSurface.col *
               rayFromViewer.intersection.mat->phongBRDF(rayFromSurface.dir,
                                                         -rayFromViewer.dir,
                                                         rayFromViewer.intersection.normal,
                                                         rayFromViewer.intersection.uv);
    }

    return Colour();
}

void Raytracer::lightShading( Ray3D& ray, int diffuseBounces, int specularBounces) const {
    // go through lights
    for (Scene::light_iter curLight = scene_->light_begin();
            curLight != scene_->light_end(); ++curLight) {
        // Each lightSource provides its own shading function.

        // pass intersection function so light can test for shadows
        (*curLight)->shade(ray, scene_->getIntersectionFunction());
    }

}

// main function that handles recursive raytracing, and choosing
// appropriate techniques based on material and scene
Colour Raytracer::shadeRay( Ray3D& ray, int diffuseBounces, int specularBounces ) const {
    Colour col(0.0, 0.0, 0.0); 

    // if we reach a certain recursion depth, stop.
    if (diffuseBounces < 0 || specularBounces < 0) {
        return col;
    }

    // get an intersection with the scene objects
    scene_->traverse(ray); 

    // Don't bother shading if the ray didn't hit 
    // anything.
    if (!ray.intersection.none) {
        if (sceneSignature) { // no need to shade if just scene signature
            return ray.intersection.mat->diffuse.at(0, 0);
        }

        // normalize the direction for lighting calculations
        ray.renormalize();

        // if material emits light, colour the ray with that colour
        Colour emittance = ray.intersection.mat->emittance.at(ray.intersection.uv);
        if (!emittance.isBlack()) {
            col += emittance;
        }
        // if material refracts
        else if (ray.intersection.mat->isTransmissive) {

            // get the indeces of the media along the refractive boundary
            double index1 = 1.0;
            double index2 = ray.intersection.mat->refractiveIndex;

            // if intersection occurs from the inside of an object
            // refractive indeces of media is swapped
            if (ray.intersection.inside) {
                std::swap(index1, index2);
            }

            // precompute factors for snell's and fresnel's laws
            Fresnel refraction(index1, index2,
                    ray.intersection.normal, ray.dir);

            // Compute reflected colour
            Ray3D reflectedRay(ray.intersection.point,
                    reflectedDir(ray.dir, ray.intersection.normal));
            Colour reflectedColour = shadeRay(reflectedRay, diffuseBounces, specularBounces-1);

            // if total internal reflection, just use reflected colour
            if (refraction.totalReflection()) {
                col += reflectedColour;
            }
            // otherwise send another ray in transmitted direction
            else {
                // get the reflection/transmission coefficients
                double rCoeff = refraction.reflectionCoefficient();
                double tCoeff = 1 - rCoeff;

                // get the transmitted direction
                Vector3D transDir = ray.dir;
                if ( ray.intersection.isSolid ) {
                    // if the object we hit is solid
                    // (i.e. not an infinitesimal plane),
                    // then calculate the appropriate direction
                    transDir = refraction.transmittedDir();
                }

                // form new transmission ray
                Ray3D transmittedRay(ray.intersection.point, transDir);

                // mix transmitted and reflected colours
                col += rCoeff*reflectedColour +
                    tCoeff*shadeRay(transmittedRay, diffuseBounces, specularBounces-1);

                // absorption of medium is handled at bottom of function
            }
        }
        // finally if the material neither emits nor refracts,
        // do the 
        else if (diffuseBounces > 0) {

            // gather sampling strategies
            std::vector< CachedSamplingStrategy > strategies;

            // get light source strategies
            for (SamplingStrategy* strategy : lightStrategies_) {
                strategies.push_back(CachedSamplingStrategy(strategy, ray));
            }

            // strategies for sampling the hemisphere (uniform or BRDFs)
            if (diffuseBounces > 1) {
                for (SamplingStrategy* strategy : diffuseStrategies_) {
                    strategies.push_back(CachedSamplingStrategy(strategy, ray));
                }
            }

            // calculate estimate using all strategies
            lightWithStrategies(ray, strategies, diffuseBounces, specularBounces);

            // shade with point lights
            lightShading(ray, diffuseBounces, specularBounces); 

            // if surface reflects light like a perfect mirror
            const double reflectance = ray.intersection.mat->reflectance.at(ray.intersection.uv);
            col = ray.col * (1.0 - reflectance);

            if (reflectance > 0.0) {
                // do reflection
                Ray3D reflectedRay(ray.intersection.point,
                        reflectedDir(ray.dir, ray.intersection.normal));

                col += reflectance * shadeRay(reflectedRay, diffuseBounces, specularBounces-1);

            }
        }


        // if we are inside a medium, and it absorbs light
        // have to attenuate
        if (ray.intersection.inside 
                && !ray.intersection.mat->absorption.isBlack() ) {

            col = attenuateByAbsorption(col,
                    ray.intersection.t_value,
                    ray.intersection.mat->absorption);

        }

    }

    return col; 
}

// Computes the probability of a direction in the regime of each sampling
// strategy, and sums to get the normalization term.
double strategyNormalization ( std::vector< CachedSamplingStrategy > const& strategies,
                               Vector3D const& dir) {
    double normalization = 0.0;
    for( auto const& cachedStrategy : strategies) {
        // have to multiply by the number of samples to be taken from
        // each strategy
        normalization += cachedStrategy.dirProbability(dir)*cachedStrategy.strategy->sampler->n();
    }

    return normalization;
}

// Using the different sampling techniques provided by the sampling strategies,
// use multiple importance sampling to compute an estimate of the radiance in
// the direction of the ray.
void Raytracer::lightWithStrategies( Ray3D& ray,
                              std::vector< CachedSamplingStrategy > const& strategies,
                              int diffuseBounces, int specularBounces ) const {

    Vector3D sampleDir;
    // sample from each strategy
    for( auto const& cachedStrategy : strategies) {
        
        // set up a sampler from [0,1]x[0,1] and use it to obtain
        // samples from the strategy
        UVSampler const& sampler = *(cachedStrategy.strategy->sampler);
        for (auto const& sample : sampler)
        {
            // obtain a direction in the hemisphere from the strategy
            cachedStrategy.getSample(sample[0], sample[1], sampleDir);
            // get normalization factor across all strategies
            // (balance heuristic for multiple importance sampling)
            double normalization = strategyNormalization(strategies, sampleDir);

            // shade the ray in the sampled direction
            Ray3D rayFromSurface(ray.intersection.point, sampleDir);
            rayFromSurface.col = shadeRay( rayFromSurface, diffuseBounces - 1, specularBounces);

            // calculate final outgoing radiance towards eye
            ray.col += calculateRadiance(rayFromSurface, ray)/normalization;
        }
    }

}

Camera::sampling_func Raytracer::getSamplingFunction() const {
    return std::bind(&Raytracer::shadeRay, this, std::placeholders::_1, maxDiffuse_, maxSpecular_);
}

void Raytracer::setScene(Scene const* scene) {
    scene_ = scene;
}

void Raytracer::setupStrategies() {
    lightStrategies_.clearStrategies();

    // strategies for sampling directions where most light could come from
    // only use these if area lights are present, otherwise have no use
    if (scene_->hasAreaLights()) {
        for (Scene::emissive_iter i = scene_->emissive_begin(); i != scene_->emissive_end(); ++i) {
            lightStrategies_.addStrategy(new LightVolumeStrategy(*(*i)->lightBound));
        }
    }

    // strategy for uniformly sampling the hemisphere
    diffuseStrategies_.clearStrategies();
    diffuseStrategies_.addStrategy(new HemisphereStrategy());
}

void Raytracer::render( Camera& cam ) {
    // let the camera know to use this raytracer for probing the scene
    cam.setSamplingFunc(getSamplingFunction());
    
    // based on settings, set up sampling strategies
    setupStrategies();

    // initialize random state for all threads
    qnd::init_threadrand();

    // initialize some variables used for tracking progress (notifies every 5%)
    const int increment = cam.height()*cam.width()/20;
    int i, j, progress;

    #pragma omp parallel private(i, j, progress)
    {
        if(omp_get_thread_num() == 0) {
            std::cout << "Threads: " << omp_get_num_threads() << std::endl;
        }

        // Construct a ray for each pixel. Split the jobs among threads
        #pragma omp for schedule(dynamic, 8)
        for (i = 0; i < cam.height(); ++i) {
            for (j = 0; j < cam.width(); ++j) {

                cam.computePixel(i, j);

                // report progress
                progress = i*cam.width() + j;
                if (progress % increment == 0) {
                    std::cout << (progress/increment)*5 << "\% Complete thread:" << omp_get_thread_num() << std::endl;
                }
            }
        }
    }
}

