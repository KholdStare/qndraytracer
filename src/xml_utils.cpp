#include "xml_utils.h"
#include "data_xml_parser.h"

#include "scene.h"
#include "camera.h"
#include "raytracer.h"
#include "scene_object_factory.h"
#include "light_source_factory.h"

#include "texture/material.h"
#include "texture/texture_parser.h"

#include "mesh/obj_store.h"
#include "mesh/obj_parse.h"
#include "mesh/mesh.h"

#include "tinyxml.h"


// Utility function Declarations ======================================
Colour parseColour( TiXmlElement* colourElement);
Point3D parsePoint( TiXmlElement* pointElement);
template <class TexStorage>
typename TexStorage::texture_type getTexture(TexStorage const& storage,
                TiXmlElement* element);


// ==================================================================== 

// Constructor
SceneXmlParser::SceneXmlParser( Raytracer& raytracer,
                                Scene& scene,
                                CameraContainer& cameras ) :
                                raytracer_(raytracer),
                                scene_(scene),
                                cameras_(cameras),
                                gamma_(1.0) {

}

SceneXmlParser::~SceneXmlParser() {

}

bool SceneXmlParser::parseSceneDefinition( std::string const& filename) {
	TiXmlDocument doc(filename);
	if (!doc.LoadFile()) {
        std::cerr << "Failed to load file: " << filename << std::endl;
        std::cerr << "Error on row " << doc.ErrorRow() << " column " << doc.ErrorCol() << std::endl;
        std::cerr << doc.ErrorDesc() << std::endl;
        return false;
	}

    std::cout << "Reading scene definition: " << filename << std::endl;
    TiXmlElement* rootElement = doc.RootElement();
    
    if (rootElement->ValueStr().compare("scene")) {
        std::cerr << "Invalid root node: " << rootElement->Value() << std::endl;
        return false;
    }

    // traverse all subelements to configure scene
    // TODO: use a map std::string => (TiXmlElement => bool)
    // perhaps use uniform initialialization to recursive map?
    FOREACH_ELEMENT_IN(rootElement)
    {
        IF_CHILD_IS("settings")
        {
            if(!parseSettings(pChild)) return false;
        }
        else IF_CHILD_IS("meshes")
        {
            if(!parseMeshes(pChild)) return false;
        }
        else IF_CHILD_IS("images")
        {
            if(!parseImages(pChild)) return false;
        }
        else IF_CHILD_IS("textures")
        {
            if(!parseTextures(pChild)) return false;
        }
        else IF_CHILD_IS("materials")
        {
            if(!parseMaterials(pChild)) return false;
        }
        else IF_CHILD_IS("node")
        {
            if(!parseNode(pChild)) return false;
        }
        else IF_CHILD_IS("cameras")
        {
            if(!parseCameras(pChild)) return false;
        }
        else IF_CHILD_IS("lights")
        {
            if(!parseLights(pChild)) return false;
        }
	}


    return true;
}

// Settings parsing ===================================================
bool SceneXmlParser::parseSettings( TiXmlElement* settingsElement) {

    FOREACH_ELEMENT_IN(settingsElement)
    {
        IF_CHILD_IS("bounces")
        {
            if(!parseBounces(pChild)) return false;
        }
        else IF_CHILD_IS("output")
        {
            if(!parseOutputSettings(pChild)) return false;
        }
        else IF_CHILD_IS("bounces")
        {
            if(!parseSamples(pChild)) return false;
        }
	}

    return true;
}

bool SceneXmlParser::parseOutputSettings( TiXmlElement* outputElement) {

    std::string text;
    if ( TIXML_SUCCESS == outputElement->QueryValueAttribute("dumpRaw", &text) ) {
        if (text.compare("true") == 0) {
            raytracer_.dumpRaw = true;
        }
    }

    text.clear();
    if ( TIXML_SUCCESS == outputElement->QueryValueAttribute("sceneSignature", &text) ) {
        if (text.compare("true") == 0) {
            raytracer_.sceneSignature = true;
        }
    }

    // TODO: assign gamma responsibility to either renderer or scene?
    double gamma = 1.0;
    if ( TIXML_SUCCESS == outputElement->QueryValueAttribute("gamma", &gamma) ) {
        gamma_ = gamma;
    }

    return true;
}

bool SceneXmlParser::parseBounces( TiXmlElement* bouncesElement) {

    int val;
    if ( TIXML_SUCCESS == bouncesElement->QueryValueAttribute("diffuse", &val) ) {
        raytracer_.setMaxDiffuse(val);
    }

    if ( TIXML_SUCCESS == bouncesElement->QueryValueAttribute("specular", &val) ) {
        raytracer_.setMaxSpecular(val);
    }

    return true;
}

bool SceneXmlParser::parseSamples( TiXmlElement* samplesElement) {

    int val;
    if ( TIXML_SUCCESS == samplesElement->QueryValueAttribute("light", &val) ) {
        raytracer_.setLightSamples(val);
    }

    if ( TIXML_SUCCESS == samplesElement->QueryValueAttribute("diffuse", &val) ) {
        raytracer_.setDiffuseSamples(val);
    }

    return true;
}
// ==================================================================== 


// Mesh parsing ====================================================
bool SceneXmlParser::parseMeshes( TiXmlElement* meshesElement) {

    FOREACH_ELEMENT_IN(meshesElement)
    {
        IF_CHILD_IS("mesh")
        {
            if(!parseMesh(pChild)) return false;
        }
	}

    return true;
}

bool SceneXmlParser::parseMesh( TiXmlElement* meshElement) {

    // get name and path
    std::string name;
    if ( TIXML_SUCCESS != meshElement->QueryValueAttribute("name", &name) ) {
        std::cerr << "No name given for mesh." << std::endl;
        return false;
    }
    std::string path;
    if ( TIXML_SUCCESS != meshElement->QueryValueAttribute("path", &path) ) {
        std::cerr << "No path given for mesh." << std::endl;
        return false;
    }

    // have to initialize a storage object for mesh data
    std::unique_ptr<ObjStore> obj(new ObjStore());

    std::string text;
    if ( TIXML_SUCCESS == meshElement->QueryValueAttribute("invertNormals", &text) ) {
        if (text.compare("true") == 0) {
            obj->invertNormals = true;
        }
    }

    if ( TIXML_SUCCESS == meshElement->QueryValueAttribute("smoothNormals", &text) ) {
        if (text.compare("true") == 0) {
            obj->smoothNormals = true;
        }
    }


    // read mesh from disk
    if (!ObjParser::parse(path, *obj) ) {
        std::cerr << "Could not create mesh \"" << name << "\" from " << path << std::endl;
        return false;
    }

    // add mesh to hashmap
    scene_.getMeshStorage().insert(name, obj.release());
    std::cout << "Adding mesh \"" << name << "\" from " << path << std::endl;

    return true;
}
// ==================================================================== 



// Image parsing ====================================================
bool SceneXmlParser::parseImages( TiXmlElement* imagesElement) {

    FOREACH_ELEMENT_IN(imagesElement)
    {
        IF_CHILD_IS("image")
        {
            if(!parseImage(pChild)) return false;
        }
	}

    return true;
}


bool SceneXmlParser::parseImage( TiXmlElement* imageElement) {

    // get name and path
    std::string name;
    if ( TIXML_SUCCESS != imageElement->QueryValueAttribute("name", &name) ) {
        std::cerr << "No name given for image." << std::endl;
        return false;
    }
    std::string path;
    if ( TIXML_SUCCESS != imageElement->QueryValueAttribute("path", &path) ) {
        std::cerr << "No path given for image." << std::endl;
        return false;
    }

    // read image from disk
    Image<RGBA>* image = readBmpFromFile(path);
    if (!image) {
        std::cerr << "Could not create image \"" << name << "\" from " << path << std::endl;
        return false;
    }

    // add image to hashmap
    scene_.getImageStorage().insert(name, image);
    std::cout << "Adding image \"" << name << "\" from " << path << std::endl;

    return true;
}
// ==================================================================== 


// Texture parsing ====================================================
bool SceneXmlParser::parseTextures( TiXmlElement* texturesElement) {

    FOREACH_ELEMENT_IN(texturesElement)
    {
        IF_CHILD_IS("texture")
        {
            if(!parseTexture(pChild)) return false;
        }
	}

    return true;
}


#define IF_DATA_TYPE_ADD_TEXTURE( DataType ) if ( dataType.compare( #DataType ) == 0 ) { \
        if ( !addTexture< DataType >(name, textureElement) ) { return false; } \
    }

bool SceneXmlParser::parseTexture( TiXmlElement* textureElement) {

    std::string name(textureElement->Attribute("name"));
    std::string dataType(textureElement->Attribute("data"));

    IF_DATA_TYPE_ADD_TEXTURE( Colour )
    else IF_DATA_TYPE_ADD_TEXTURE( double )
    else {
        std::cerr << "No support for textures of data type \"" << dataType << "\"." << std::endl;
        return false;
    }

    std::cout << "Adding " << dataType << " texture \"" << name << "\"" << std::endl;

    return true;
}

#undef IF_DATA_TYPE_ADD_TEXTURE

template <class DataType>
bool SceneXmlParser::addTexture( std::string const& name, TiXmlElement* textureElement) {
    static RGBAConverter<DataType> converter;
    converter.gamma = gamma_;
    // create new texture from information provided in the xml
    Texture<DataType>* tex = TextureXmlParser<DataType>::parse(textureElement,
                                                               scene_.getImageStorage(),
                                                               converter );

    // add texture to appropriate storage
    if (tex) {
        scene_.textureStorage.get<DataType>().insert(name, tex);
    }
    else {
        std::cerr << "Could not create texture. Please make sure texture type is available." << std::endl;
        return false;
    }

    return true;
}
// ==================================================================== 



// Materials parsing ================================================== 
bool SceneXmlParser::parseMaterials( TiXmlElement* materialsElement) {

    FOREACH_ELEMENT_IN(materialsElement)
    {
        IF_CHILD_IS("material")
        {
            if(!parseMaterial(pChild)) return false;
        }
	}

    return true;
}

bool SceneXmlParser::parseMaterial( TiXmlElement* materialElement) {

    std::string name(materialElement->Attribute("name"));
    std::cout << "Adding material \"" << name << "\"" << std::endl;
    Material* mat = new Material();
    scene_.getMaterialStorage().insert( name, mat );

    FOREACH_ELEMENT_IN(materialElement)
    {
        IF_CHILD_IS("ambient")
        {
            parseMaterialParameter(mat->ambient, pChild);
        }
        else IF_CHILD_IS("diffuse")
        {
            parseMaterialParameter(mat->diffuse, pChild);
        }
        else IF_CHILD_IS("specular")
        {
            parseMaterialParameter(mat->specular, pChild);
            pChild->QueryValueAttribute("exponent", &(mat->specular_exp));
        }
        else IF_CHILD_IS("emittance")
        {
            parseMaterialParameter(mat->emittance, pChild);
        }
        else IF_CHILD_IS("reflectance")
        {
            parseMaterialParameter(mat->reflectance, pChild);
        }
        else IF_CHILD_IS("absorption")
        {
            mat->setAbsorptionColour(parseColour(pChild));
        }
        else IF_CHILD_IS("refraction")
        {

            double val;
            if ( TIXML_SUCCESS == pChild->QueryValueAttribute("index", &val) ) {
                mat->refractiveIndex = val;
            }

            std::string text;
            if ( TIXML_SUCCESS == pChild->QueryValueAttribute("transmissive", &text) ) {
                if (text.compare("true") == 0) {
                    mat->isTransmissive = true;
                }
            }
        }

	}


    return true;
}

template <class MatParam>
void SceneXmlParser::parseMaterialParameter( MatParam& param,
                                             TiXmlElement* parameterElement) {
    typedef typename MatParam::data_type DataType;
    param.set(DataXmlParser<DataType>::parse(parameterElement));
    param.set(getTexture(scene_.textureStorage.get<DataType>(), parameterElement));
}

// ==================================================================== 


// Lights parsing =====================================================
bool SceneXmlParser::parseLights( TiXmlElement* lightsElement ) {

    FOREACH_ELEMENT_IN(lightsElement)
    {
        IF_CHILD_IS("light")
        {
            if(!parseLight(pChild)) return false;
        }
	}

    return true;
}
bool SceneXmlParser::parseLight(  TiXmlElement* lightElement ) {

    std::string lightType;
    if ( lightElement->Attribute("type") ) {
        lightType = lightElement->Attribute("type");
    }
    else {
        std::cerr << "Could not create light with no type." << std::endl;
        return false;
    }

    Point3D pos = parsePoint(lightElement);

    Colour ambient;
    Colour diffuse;
    Colour specular;
    // position camera
    FOREACH_ELEMENT_IN(lightElement)
    {
        IF_CHILD_IS("colour")
        {
            ambient = parseColour(pChild);
            diffuse = ambient;
            specular = ambient;
            break;
        }
        else IF_CHILD_IS("ambient")
        {
            ambient = parseColour(pChild);
        }
        else IF_CHILD_IS("diffuse")
        {
            diffuse = parseColour(pChild);
        }
        else IF_CHILD_IS("specular")
        {
            specular = parseColour(pChild);
        }
	}

    scene_.addLightSource(makeLight(lightType, pos, ambient, diffuse, specular));

    return true;
}
// ==================================================================== 



// Camera parsing =====================================================
bool SceneXmlParser::parseCameras( TiXmlElement* camerasElement ) {

    FOREACH_ELEMENT_IN(camerasElement)
    {
        IF_CHILD_IS("camera")
        {
            if(!parseCamera(pChild)) return false;
        }
	}

    return true;

}


bool SceneXmlParser::parseCamera( TiXmlElement* cameraElement ) {

    std::shared_ptr<Camera> camera;

    int width, height;
    cameraElement->QueryValueAttribute("width", &width);
    cameraElement->QueryValueAttribute("height", &height);

    double fov=60.0;
    cameraElement->QueryValueAttribute("fov", &fov);

    // create camera
    camera.reset( new Camera(width, height, fov) );

    // set gamma
    camera->setGamma(gamma_);

    // give name
    if ( cameraElement->Attribute("name") ) {
        camera->name = cameraElement->Attribute("name");
        std::cout << "Creating camera \"" << camera->name << "\"" << std::endl;
    }
    else {
        std::cerr << "Camera missing a name, could not create camera." << std::endl;
        return false;
    }

    // position camera
    FOREACH_ELEMENT_IN(cameraElement)
    {
        IF_CHILD_IS("lens")
        {
            if(!parseLens(pChild, camera)) return false;
        }
        else IF_CHILD_IS("samples")
        {
            if(!parseCameraSamples(pChild, camera)) return false;
        }
        else IF_CHILD_IS("eye")
        {
            camera->setEye(parsePoint(pChild));
        }
        else IF_CHILD_IS("view")
        {
            camera->setView(Vector3D(parsePoint(pChild)));
        }
        else IF_CHILD_IS("up")
        {
            camera->setUp(Vector3D(parsePoint(pChild)));
        }
	}

    cameras_.push_back(camera);

    return true;
}

bool SceneXmlParser::parseCameraSamples( TiXmlElement* samplesElement,
                                std::shared_ptr<Camera> const& camera ) {

    int val;
    if ( TIXML_SUCCESS == samplesElement->QueryValueAttribute("aperture", &val) ) {
        camera->setApertureSamples(val);
    }

    if ( TIXML_SUCCESS == samplesElement->QueryValueAttribute("antialias", &val) ) {
        camera->setAntialiasSamples(val);
    }

    return true;
}

bool SceneXmlParser::parseLens( TiXmlElement* lensElement,
                                std::shared_ptr<Camera> const& camera ) {

    double val;
    if ( TIXML_SUCCESS == lensElement->QueryValueAttribute("aperture", &val) ) {
        camera->setApertureRadius(val);
    }

    if ( TIXML_SUCCESS == lensElement->QueryValueAttribute("focalDistance", &val) ) {
        camera->setFocalDistance(val);
    }

    return true;
}
// ==================================================================== 


// Node parsing =======================================================
bool SceneXmlParser::parseNode( TiXmlElement* nodeElement, SceneDagNode* parent) {

    // create new shape
    std::unique_ptr<SceneObject> shape = nullptr;
    std::string shapeName;
    std::string meshName;
    if ( nodeElement->Attribute("shape") ) {
        shapeName = nodeElement->Attribute("shape");
        shape.reset(makeShape(shapeName));

        if (!shape) {
            std::cerr << "Could not create \"" << shapeName << "\" shape." << std::endl;
            return false;
        }
    }
    else if ( nodeElement->Attribute("mesh") ) {
        meshName = nodeElement->Attribute("mesh");
        ObjStore* obj = scene_.getMeshStorage().get(meshName);

        if (!obj) {
            std::cerr << "Could not find \"" << meshName << "\" mesh in the database." << std::endl;
            return false;
        }

        shape.reset(new Mesh(obj));
        shapeName = "Mesh";
    }

    // retrieve material
    Material* mat = 0;
    if ( nodeElement->Attribute("material") ) {
        std::string materialName( nodeElement->Attribute("material") );

        mat = scene_.getMaterialStorage().get(materialName);

        if (!mat) {
            std::cerr << "Could not find \"" << materialName << "\" material." << std::endl;
            return false;
        }
    }

    if (shape && !mat) {
        std::cerr << "Could not create node with object but no material." << std::endl;
        return false;
    }
    
    if (shape) {
        std::cout << "Creating \"" << shapeName << "\" shape." << std::endl;
    }
    else {
        std::cout << "Creating empty node." << std::endl;
    }

    // create node with shape/material
    SceneDagNode* newNode = nullptr;
    if (!parent) {
        // attach to new node to root of Scene
        newNode = scene_.addObject(shape.release(), mat);
    }
    else {
        newNode = scene_.addObject(parent, shape.release(), mat);
    }

    // parse transformations and recurse through child nodes
    FOREACH_ELEMENT_IN(nodeElement)
    {
        IF_CHILD_IS("rotate")
        {
            if(!parseRotation(pChild, newNode)) return false;
        }
        else IF_CHILD_IS("translate")
        {
            if(!parseTranslation(pChild, newNode)) return false;
        }
        else IF_CHILD_IS("scale")
        {
            if(!parseScaling(pChild, newNode)) return false;
        }
        else IF_CHILD_IS("node")
        {
            if(!parseNode(pChild, newNode)) return false;
        }
	}

    return true;
}

bool SceneXmlParser::parseRotation( TiXmlElement* rotationElement, SceneDagNode* node) {

    // get axis
    char axis = 'x';
    if ( rotationElement->Attribute("axis") ) {
        axis = rotationElement->Attribute("axis")[0];
    }

    // get angle
    double angle = 0.0;
    rotationElement->QueryValueAttribute("angle", &angle);

    // perform rotation
    std::cout << "Rotating around " << axis << " axis by " << angle << " degrees." << std::endl;
    node->rotate( axis, angle );

    return true;
}

bool SceneXmlParser::parseTranslation( TiXmlElement* translationElement, SceneDagNode* node) {

    // perform translation
    Vector3D offset(parsePoint(translationElement));
    std::cout << "Translating by " << offset << std::endl;
    node->translate( offset );

    return true;
}

bool SceneXmlParser::parseScaling( TiXmlElement* scalingElement, SceneDagNode* node) {

    // get the scaling factor
    Point3D scalingFactor = parsePoint(scalingElement);

    // get the point around which to scale
    double pointx = 0.0;
    double pointy = 0.0;
    double pointz = 0.0;
    scalingElement->QueryValueAttribute("pointx", &pointx);
    scalingElement->QueryValueAttribute("pointy", &pointy);
    scalingElement->QueryValueAttribute("pointz", &pointz);

    // perform the scaling
    std::cout << "Scaling by factor " << scalingFactor << std::endl;
    node->scale( Point3D(pointx, pointy, pointz), scalingFactor.v );

    return true;
}
// ==================================================================== 

template <class TexStorage>
typename TexStorage::texture_type
getTexture(TexStorage const& storage, TiXmlElement* element) {
    std::string text;
    if ( TIXML_SUCCESS == element->QueryValueAttribute("texture", &text) ) {
        return storage.get( text );
    }

    return nullptr;
}

// Utility function Definitions  ======================================
inline Colour parseColour( TiXmlElement* colourElement) {
    return DataXmlParser<Colour>::parse(colourElement);
}


inline Point3D parsePoint( TiXmlElement* pointElement) {
    return DataXmlParser<Point3D>::parse(pointElement);
}

// ==================================================================== 
