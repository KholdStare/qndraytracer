#include "material.h"
#include "../colour.h"
#include <ostream>

Colour Material::phongBRDF(Vector3D const& incoming,
                           Vector3D const& outgoing,
                           Vector3D const& normal,
                           UVPoint uv) const {
    
    // diffuse is just uniform distribution
    Colour ratio(diffuse.at(uv));

    // cos of angle from normal to incoming
    double cosIn = incoming.dot(normal);
    // direction of reflection of incoming vector
    Vector3D reflectDir = 2*cosIn*normal - incoming;
    reflectDir.normalize();
    // cos of angle from reflection to ray back to viewer
    double rv = reflectDir.dot(outgoing);

    // specular
    if (rv > 0) {
        ratio += specular.at(uv) * pow(rv, specular_exp);
    }

    return ratio;
}


std::ostream& operator <<(std::ostream& o, const Material& m) {
    o << "ambient: " << m.ambient.at(0, 0) << std::endl;
    o << "diffuse: " << m.diffuse.at(0, 0) << std::endl;
    o << "specular: " << m.specular.at(0, 0) << std::endl;
    o << "spec exp: " << m.specular_exp << std::endl;
    o << "reflectance: " << m.reflectance.at(0, 0);
    return o;
}

