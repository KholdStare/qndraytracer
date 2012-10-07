#include "ray.h"

bool consolidateRayInter(Ray3D& r, Intersection& i) {
    if (i.none) return false;

    if (i.t_value < 15000*std::numeric_limits<double>::epsilon()) {
        // reject intersection if it happens in the backwards
        // direction
        // large "fudge factor" since secondary rays start at intersection
        // points, and may cause self-intersection. It is still ~10^-9
        i.none = true;
    }
    else if (r.intersection.none) {
        // ray has no intersection, so accept new intersection
        r.intersection = i;
    }
    else if (r.intersection.t_value > i.t_value) {
        // accept new intersection, since its closer
        r.intersection = i; 
    }
    else {
        i.none = true; // reject new intersection
    }

    return !i.none;
}

