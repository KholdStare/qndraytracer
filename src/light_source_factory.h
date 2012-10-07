#ifndef _LIGHT_SOUCE_FACTORY_H_
#define _LIGHT_SOUCE_FACTORY_H_

#include <string>
#include "math/math_types.h"

class LightSource;
class Colour;

/**
 * Factory function for making simple lights.
 */
LightSource* makeLight(std::string lightType, Point3D const& pos,
                                              Colour const& amb,
                                              Colour const& diffuse,
                                              Colour const& specular);

#endif // _LIGHT_SOUCE_FACTORY_H_
