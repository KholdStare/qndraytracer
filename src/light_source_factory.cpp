#include "light_source_factory.h"
#include "light_source.h"

#define CREATE_LIGHT_IF_MATCH( Light ) if(lightType.compare( #Light ) == 0) { \
        return new Light( pos, ambient, diffuse, specular ); }


LightSource* makeLight(std::string lightType, Point3D const& pos,
                                              Colour const& ambient,
                                              Colour const& diffuse,
                                              Colour const& specular) {
    CREATE_LIGHT_IF_MATCH( PointLight )

    return nullptr;
}

