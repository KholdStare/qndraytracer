#include "scene_object_factory.h"
#include "scene_object.h"

#define CREATE_SHAPE_IF_MATCH( Shape ) if(shapeName.compare( #Shape ) == 0) { \
        return new Shape(); }

SceneObject* makeShape(std::string shapeName) {
    CREATE_SHAPE_IF_MATCH(UnitSquare)
    else CREATE_SHAPE_IF_MATCH(UnitSphere)
    else CREATE_SHAPE_IF_MATCH(UnitCube)
    return nullptr;
}
