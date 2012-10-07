#include "fresnel.h"

Fresnel::Fresnel(double index1, double index2,
                 Vector3D const& normal, Vector3D const& incident) :
                    index1_(index1),
                    index2_(index2),
                    normal_(normal),
                    incident_(incident),
                    cosIncident_(-(normal.dot(incident))),
                    sinIncidentSquared_(1 - cosIncident_*cosIncident_),
                    sinTransmittedSquared_( (index1*index1*sinIncidentSquared_) / (index2*index2) ),
                    cosTransmitted_( 0.0 ) {
                    
    // before computing the cosine, check for total internal reflection
    // otherwise square root is imaginary
    if (sinTransmittedSquared_ <= 1.0) {
        cosTransmitted_ = sqrt(1 - sinTransmittedSquared_);
    }

}

Vector3D Fresnel::transmittedDir() {

    // adapted from 2006 degreve reflection/refraction pdf
    double ratio = index1_/index2_;
    double normalFactor = ratio*cosIncident_ - cosTransmitted_;
    return ratio*incident_ + normalFactor*normal_;
}


double Fresnel::reflectionCoefficient() {
    // compute coefficients for both polarization of light
    double i1 = index1_*cosIncident_;
    double i2 = index2_*cosTransmitted_;
    double sPolarized = (i1 - i2) / (i1 + i2);
    i1 = index1_*cosTransmitted_;
    i2 = index2_*cosIncident_;
    double pPolarized = (i1 - i2) / (i1 + i2);

    // average them
    return (sPolarized*sPolarized + pPolarized*pPolarized) / 2.0;
}


