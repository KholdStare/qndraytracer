#ifndef _TEXTURE_STORAGE_H_
#define _TEXTURE_STORAGE_H_

#include "../pointer_map.hpp"
#include "texture.h"

/**
 * Convenience class that represents
 * A Map from strings to to pointers of Texture objects.
 */
template <class DataType>
class TextureStorage : public PointerMap< Texture<DataType> > {

public:
    typedef Texture<DataType>* texture_type;

};


#endif // _TEXTURE_STORAGE_H_
