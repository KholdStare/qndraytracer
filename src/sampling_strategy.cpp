#include "sampling_strategy.h"
#include "light_volume.h"
#include "ray.h"

SamplingStrategy::~SamplingStrategy() { }

/***************************************************************************
 *                           HemisphereStrategy                            *
 ***************************************************************************/

strategy_cache HemisphereStrategy::initCache(Ray3D const& ray) const {
    return strategy_cache( ray, 1.0 );
}

void HemisphereStrategy::getSample(double u, double v,
                                Vector3D& dir, strategy_cache const& cache) const {

    double sinTheta = sqrt(1 - u * u);
    double fi = 2*M_PI*v;

    // create a temporary sample in the hemisphere
    // relative to (0, 0, 1) normal
    Vector3D tempDir(cos(fi)*sinTheta,
                     sin(fi)*sinTheta,
                     u);

    // rotate direction to be ralative to actual normal
    dir = rotateVector(tempDir, cache.ray->intersection.normal);
}

double HemisphereStrategy::dirProbability(Vector3D const& dir, strategy_cache const& cache) const {
    return 1.0;
}


/***************************************************************************
 *                           LightVolumeStrategy                           *
 ***************************************************************************/

LightVolumeStrategy::LightVolumeStrategy( LightVolume const& bound)
                : bound_(bound) { }
                
strategy_cache LightVolumeStrategy::initCache(Ray3D const& ray) const {
    return strategy_cache( ray,
                          bound_.subtendedProbability(ray.intersection.point)  );
}

void LightVolumeStrategy::getSample(double u, double v,
                                Vector3D& dir, strategy_cache const& cache) const {
    bound_.getSubtendedDir(cache.ray->intersection.point, u, v, dir);
}

double LightVolumeStrategy::dirProbability(Vector3D const& dir, strategy_cache const& cache) const {
    // check whether direction points towards light
    if (bound_.isSubtended(cache.ray->intersection.point, dir)) {
        return cache.probability;
    }

    return 0.0;
}
