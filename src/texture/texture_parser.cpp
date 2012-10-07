#include "texture_parser.h"
#include "tinyxml.h"
#include "../data_xml_parser.h"

#include "checker_texture.h"
#include "image_texture.h"

#include <string>

// individual parsers for different types follow ========
template <class DataType>
CheckerTexture<DataType>* parseCheckerTexture(TiXmlElement* textureElement) {

    int checkersPerSide = 2;
    textureElement->QueryValueAttribute("checkersPerSide", &checkersPerSide);

    DataType first = DataType();
    DataType second = DataType();
    FOREACH_ELEMENT_IN(textureElement)
    {
        IF_CHILD_IS("first")
        {
            first = DataXmlParser<DataType>::parse(pChild);
        }
        else IF_CHILD_IS("second")
        {
            second = DataXmlParser<DataType>::parse(pChild);
        }
    }

    return new CheckerTexture<DataType>(checkersPerSide, first, second);
}

template <class DataType>
ImageTexture<DataType>* parseImageTexture(TiXmlElement* textureElement,
                                    PointerMap< Image<RGBA> > const& images,
                                    RGBAConverter<DataType> const& converter) {
    
    ImageTexture<DataType>* tex = NULL;

    // get name of image
    std::string imageName;
    if ( TIXML_SUCCESS != textureElement->QueryValueAttribute("image", &imageName) ) {
        std::cerr << "No name given for image." << std::endl;
        return tex;
    }

    // get appropriate image from database using name
    Image<RGBA>* image = images.get(imageName);
    if (!image) {
        std::cerr << "Could not find image \"" << imageName << "\"" << std::endl;
        return tex;
    }

    // create texture from image
    tex = new ImageTexture<DataType>(*image, converter);

    return tex;
}
// ======================================================

// factory method
template <class DataType>
Texture<DataType>* TextureXmlParser<DataType>::parse(TiXmlElement* textureElement,
                                    PointerMap< Image<RGBA> > const& images,
                                    RGBAConverter<DataType> const& converter) {

    Texture<DataType>* tex = NULL;
    std::string type;
    // if no type is given, cannot create appropriate texture type, return null
    if ( TIXML_SUCCESS != textureElement->QueryValueAttribute("type", &type) ) {
        return tex;
    }

    if (type.compare("CheckerTexture") == 0) {
        // parse checker texture params
        tex = parseCheckerTexture<DataType>(textureElement);
    }
    else if (type.compare("ImageTexture") == 0) {
        // parse checker texture params
        tex = parseImageTexture<DataType>(textureElement, images, converter);
    }
    else {
        std::cerr << "\"" << type << "\" type is unavailable for textures." << std::endl;
    }

    return tex;
}

// instantiate class
template class TextureXmlParser<Colour>;
template class TextureXmlParser<double>;

