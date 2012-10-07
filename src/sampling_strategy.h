#ifndef _SAMPLING_STRATEGY_H_
#define _SAMPLING_STRATEGY_H_

#include "uv_sampler.h"
#include "math/math_types.h"
#include <utility>

class Ray3D;
class LightVolume;
class SamplingStrategy;
struct strategy_cache;

/**
 * A strategy for sampling the hemisphere of directions,
 * used for multiple importance sampling.
 *
 * Usage:
 * A sampling strategy is a general strategy algorithm and is
 * not associated with a particular ray. A strategy_cache
 * struct can be used to store data about a particular ray,
 * and any subsequent calculations by the SamplingStrategy
 * will be done using the cache.
 *
 * Rationale:
 * All of the methods are const, allowing lock-free usage of
 * a SamplingStrategy in concurrent programs. Usually one
 * instance of the class will be created for a particular
 * sampling strategy (e.g. uniformly sample the hemisphere),
 * so there has to be a way to use it for different situations
 * (e.g. depending on intersection normal, hemisphere is oriented
 * differently).
 * A strategy_cache struct is a cheap object that can be created
 * on the stack of the caller and passed to the strategy to
 * do the specific calculations. This avoids expensive allocations
 * of sampling strategies on the heap for each different ray.
 */
class SamplingStrategy {

public:
    SamplingStrategy()
            : sampler(nullptr) { }
    virtual ~SamplingStrategy();

    /**
     * Initializes the cache according to the ray.
     *
     * Pass this cache on all subsequent calls of methods
     * below.
     */
    virtual strategy_cache initCache(Ray3D const& ray) const = 0;

    /**
     * Given unit samples @a u and @a v, set a direction through @a dir,
     * and @return the probability of such a direction, using the @a cache.
     *
     * The direction is a sample in the hemisphere around the intersection
     * point of the ray, according to the sampling strategy
     */
    virtual void getSample(double u, double v,
                           Vector3D& dir, strategy_cache const& cache) const = 0;

    /**
     * @Return the probability of producing the particular direction,
     * using the @a cache.
     */
    virtual double dirProbability(Vector3D const& dir, strategy_cache const& cache) const = 0;

    /**
     * The sampler associated with the strategy. Can be used to
     * draw uniformly distributed samples (u,v) from [0,1]x[0,1]
     *
     * A single sampler can be shared between multiple Sampling strategies.
     */
    UVSampler* sampler; 

};

/**
 * A non-polymorphic object that can be stored on the stack, avoiding
 * many small heap allocations for strategies.
 */
struct strategy_cache {
    strategy_cache( Ray3D const& r, double p) : ray(&r), probability(p) { }

    Ray3D const* ray;
    double probability;
};

/**
 * A simple strategy- just sample unifrmaly in the hemisphere around
 * the intersection point
 */
class HemisphereStrategy : public SamplingStrategy {

public:
    HemisphereStrategy() { }

    strategy_cache initCache(Ray3D const& ray) const;
    void getSample(double u, double v,
                           Vector3D& dir, strategy_cache const& cache) const;

    double dirProbability(Vector3D const& dir, strategy_cache const& cache) const;

};


/**
 * A strategy associated with lights. Given a volume in world space that
 * contains a light source, this strategy generates uniformly distributed
 * samples of directions subtended by the volume.
 *
 * This is what enables accurate sampling of arbitrary light sources.
 * Mostly delegates calls to the LightVolume.
 */
class LightVolumeStrategy : public SamplingStrategy {

public:
    LightVolumeStrategy(LightVolume const& bound);

    strategy_cache initCache(Ray3D const& ray) const;
    void getSample(double u, double v,
                           Vector3D& dir, strategy_cache const& cache) const;

    double dirProbability(Vector3D const& dir, strategy_cache const& cache) const;

private:
    LightVolume const& bound_;

};

#endif // _SAMPLING_STRATEGY_H_
