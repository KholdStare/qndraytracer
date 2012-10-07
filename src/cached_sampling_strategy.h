#ifndef _CACHED_SAMPLING_STRATEGY_H_
#define _CACHED_SAMPLING_STRATEGY_H_

#include "sampling_strategy.h"

/**
 * A Convenience class for working with a SamplingStrategy and
 * its associated cache.
 *
 * @see SamplingStrategy
 *
 * As mentioned in the SamplingStrategy documentation, a
 * cache must be used per ray, to avoid heap allocations.
 * To simplify this interface, a stack allocated
 * CachedSamplingStrategy can be used to encapsulate
 * that behaviour, without dealing with the cache directly.
 */
class CachedSamplingStrategy {

public:
    /**
     * Create a new CachedSamplingStrategy, using a specific
     * SamplingStrategy @a s with the specific Ray3D @a ray.
     */
    CachedSamplingStrategy(SamplingStrategy* s, Ray3D const& ray)
            : strategy(s), cache_(strategy->initCache(ray)) { }

    /**
     * Given unit samples @a u and @a v, set a direction through @a dir,
     * and @return the probability of such a direction
     *
     * The direction is a sample in the hemisphere around the intersection
     * point of the ray, sampled according to the SamplingStrategy
     */
    void getSample(double u, double v, Vector3D& dir) const {
        strategy->getSample(u, v, dir, cache_);
    }

    /**
     * @Return the probability of producing the particular direction,
     * under the regime of the SamplingStrategy.
     */
    double dirProbability(Vector3D const& dir) const {
        return strategy->dirProbability(dir, cache_);
    }

    SamplingStrategy* strategy; ///< The sampling strategy being used

private:
    strategy_cache cache_; ///< cache associated with the SamplingStrategy

};

#endif // _CACHED_SAMPLING_STRATEGY_H_
