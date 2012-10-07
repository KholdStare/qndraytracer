#ifndef _TEXTURE_H_
#define _TEXTURE_H_

/**
 * Abstract class used to access 2D data in [0,1]x[0,1] continuous coordinates
 */
template <class DataType>
class Texture {

public:
    typedef DataType data_type;

    Texture() { }
    virtual ~Texture() { }

    virtual DataType at(double u, double v) const = 0;

    // TODO: mipmaps?

};

#endif // _TEXTURE_H_
