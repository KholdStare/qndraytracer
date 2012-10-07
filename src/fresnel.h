#ifndef _FRESNEL_H_
#define _FRESNEL_H_

#include "math/math_types.h"

/**
 * A class for computing various directions and coefficients
 * associated with refraction
 */
class Fresnel {

public:
    Fresnel(double index1, double index2,
            Vector3D const& normal, Vector3D const& incident);

    inline bool totalReflection() {
        return sinTransmittedSquared_ > 1;
    }

    Vector3D transmittedDir();
    double reflectionCoefficient();
    
private:
    double index1_;
    double index2_;
    Vector3D const& normal_;
    Vector3D const& incident_;
    double cosIncident_;
    double sinIncidentSquared_;
    double sinTransmittedSquared_;
    double cosTransmitted_;

};

#endif // _FRESNEL_H_
