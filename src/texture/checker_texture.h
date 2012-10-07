#ifndef _CHECKER_TEXTURE_H_
#define _CHECKER_TEXTURE_H_

#include "texture.h"

/**
 * A Procedurally generated texture that is a checkerboard of two
 * specified colours and subdivision.
 *
 * This is templated on a DataType, so the "colours" of the checkerboard
 * texture can be constants of any DataType, be it a Colour, or double
 * or Vector3D.
 */
template <class DataType>
class CheckerTexture : public Texture<DataType> {

public:
    /**
     * Construct a texture that has @a checkersPerSide squares along one side,
     * That alternate between two constants of DataType: @a first and @a second.
     */
    CheckerTexture(int checkersPerSide, DataType const& first, DataType const& second) :
            num_(checkersPerSide) {
        colors_[0] = first;
        colors_[1] = second;
    }

    /**
     * Return a value at position (u,v) in the texture,
     * where (u,v) is in range [0,1]x[0,1]
     */
    DataType at(double u, double v) const {
        // determine what color checker we are in
        int i = (int(u*num_) + int(v*num_)) % 2;
        return colors_[i];
    }

private:
    DataType colors_[2]; ///< array of the two "colour" constants
    int num_; ///< number of squares per side of texture

};

#endif // _CHECKER_TEXTURE_H_
