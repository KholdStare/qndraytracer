#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "material_parameter.h"
#include "../colour.h"
#include "../math/math_types.h"

/**
 * Describes a material of an object.
 *
 * Contains properties like diffuse colour, emission, 
 * and transmission properties.
 */
struct Material {
	Material( ) : specular_exp(0.0),
                  reflectance(0.0),
                  refractiveIndex(0.0),
                  isTransmissive(false) { }

	Material( Colour ambient, Colour diffuse, Colour specular, double exp ) :
		ambient(ambient), diffuse(diffuse),
        specular(specular), specular_exp(exp), reflectance(0.0),
        refractiveIndex(0.0), isTransmissive(false) {}

    MaterialParameter<Colour> ambient;  ///< Ambient components for Phong shading.
    MaterialParameter<Colour> diffuse;  ///< Diffuse components for Phong shading.
    MaterialParameter<Colour> specular; ///< Specular components for Phong shading.
	double specular_exp;                ///< Specular exponent.

    MaterialParameter<double> reflectance; ///< reflectance constant

    double refractiveIndex; ///< refractive index of the object material
    
    /** decribes strength/colour of light emitted by the particular material */
    MaterialParameter<Colour> emittance;

    /**
     * Set the colour of light that will pass through the
     * refractive medium.
     *
     * The inverse will be used as an absorption coefficient
     * e.g. if blue is specified then orange will be absorbed
     */
    inline void setAbsorptionColour(Colour const& col) {
        absorption = col.inverse();
    }
    Colour absorption; ///< absorption coefficient

    /**
     * A BRDF based on the phong reflectance model
     * returns a ratio of incoming to outgoing radiance for each RGB component.
     * Both incoming and outgoing vectors have to face away from the surface,
     * and be unit length
     *
     * TODO: make an assignable closure to use different BRDFs?
     */
    Colour phongBRDF(Vector3D const& incoming,
                     Vector3D const& outgoing,
                     Vector3D const& normal,
                     UVPoint uv) const;

    /** determines whether the object can transmit (refract etc.) light */
    bool isTransmissive;

    // TODO: add ability to have fresnel surface reflection without
    // refraction through medium, just diffuse colour underneath.

    // TODO: add normal map parameter.
    
};

std::ostream& operator <<(std::ostream& o, const Material& m); 

#endif // _MATERIAL_H_

