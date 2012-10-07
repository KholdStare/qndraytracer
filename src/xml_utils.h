#ifndef _XML_UTILS_H_
#define _XML_UTILS_H_

#include <vector>
#include <string>
#include <memory>
#include <boost/noncopyable.hpp>
#include <map>

#include "texture/texture.h"
#include "texture/bmp_image.h"

class Raytracer;
class Scene;
class Camera;
class TiXmlElement;
class SceneDagNode;
class Material;

typedef std::vector<std::shared_ptr<Camera> > CameraContainer;

/**
 * Class for parsing raytracer settings, materials, and scene data from
 * an xml file
 */
class SceneXmlParser : boost::noncopyable {

public:
    /**
     * Constructs a new SceneXmlParser, and modifies the raytracer, scene and cameras
     * in place, when parsing the scene definition XML.
     */
    SceneXmlParser( Raytracer& raytracer, Scene& scene, CameraContainer& cameras );
    ~SceneXmlParser();

    /**
     * Parses given xml file and fills the scene appropriately
     */
    bool parseSceneDefinition( std::string const& filename );

private:
    // Settings
    bool parseSettings( TiXmlElement* settingsElement);
    bool parseOutputSettings( TiXmlElement* outputElement);
    bool parseBounces( TiXmlElement* bouncesElement);
    bool parseSamples( TiXmlElement* samplesElement);

    // Meshes
    bool parseMeshes( TiXmlElement* meshesElement);
    bool parseMesh( TiXmlElement*   meshElement);

    // Images
    bool parseImages( TiXmlElement* imagesElement);
    bool parseImage( TiXmlElement*  imageElement);

    // Textures
    bool parseTextures( TiXmlElement* texturesElement);
    bool parseTexture( TiXmlElement*  textureElement);

    // Materials
    bool parseMaterials( TiXmlElement* materialsElement);
    bool parseMaterial( TiXmlElement* materialElement);
    template <class MatParam>
    void parseMaterialParameter( MatParam& param, TiXmlElement* parameterElement);

    // Lights
    bool parseLights( TiXmlElement* lightsElement );
    bool parseLight(  TiXmlElement* lightElement );

    // Cameras
    bool parseCameras( TiXmlElement* camerasElement );
    bool parseCamera( TiXmlElement* cameraElement );
    bool parseCameraSamples( TiXmlElement* lensElement,
                    std::shared_ptr<Camera> const& camera ); 
    bool parseLens( TiXmlElement* lensElement,
                    std::shared_ptr<Camera> const& camera ); 

    // Nodes
    bool parseNode( TiXmlElement* nodeElement, SceneDagNode* parent = nullptr);
    bool parseRotation( TiXmlElement* rotationElement, SceneDagNode* node);
    bool parseTranslation( TiXmlElement* translationElement, SceneDagNode* node);
    bool parseScaling( TiXmlElement* scalingElement, SceneDagNode* node);

private:
    /**
     * Adds a texture of a particular data type under the given @a name,
     * using information provided inthe XML @a textureElement,
     * into the texture storage of the scene.
     */
    template <class DataType>
    bool addTexture( std::string const& name, TiXmlElement* textureElement);

private:
    Raytracer& raytracer_;
    Scene& scene_;
    CameraContainer& cameras_;
    double gamma_; // TODO get rid of hack

};


#endif // _XML_UTILS_H_
