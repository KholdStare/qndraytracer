#ifndef _TEXTURE_PARSER_H_
#define _TEXTURE_PARSER_H_

#include "texture.h"
#include "../pointer_map.hpp"
#include "bmp_image.h"
#include "rgba_converter.hpp"

class TiXmlElement;

/**
 * An object that can parse an XML element and
 * return a new texture object
 */
template <class DataType>
struct TextureXmlParser {

    static Texture<DataType>* parse(TiXmlElement* textureElement,
                                    PointerMap< Image<RGBA> > const& images,
                                    RGBAConverter<DataType> const& converter);

};

#endif // _TEXTURE_PARSER_H_
