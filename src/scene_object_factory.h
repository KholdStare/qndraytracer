#ifndef _SCENE_OBJECT_FACTORY_H_
#define _SCENE_OBJECT_FACTORY_H_

#include "scene_object.h"
#include <string>

class SceneObject;

/**
 * Factory function that generates a SceneObject given a shape string
 */
SceneObject* makeShape(std::string shapeName);

#endif // _SCENE_OBJECT_FACTORY_H_
