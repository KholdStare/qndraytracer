#ifndef _IMAGE_TEXTURE_H_
#define _IMAGE_TEXTURE_H_

#include "texture.h"
#include "rgba_converter.hpp"
#include "bmp_image.h"

/**
 * A Texture class that infers its values from an underlying
 * RGBA Image (e.g. that was read from disc)
 *
 * The texture itself can represent any DataType (e.g. double or Vector3D),
 * as long as a converter from RGBA to the DataType is provided.
 */
template <class DataType>
class ImageTexture : public Texture<DataType> {

public:
    ImageTexture(Image<RGBA>& image, RGBAConverter<DataType> const& converter)
        : image_(image), converter_(converter) { }

    // TODO: bilinear filtering
    /**
     * Return the data at (u,v) in the texture.
     *
     * (u,v) in range [0,1]x[0,1]
     */
    DataType at(double u, double v) const {
        RGBA const& col = image_[int(v*image_.height())][int(u*image_.width())];
        // use converter to go from rgba to data type
        return converter_.fromRGBA(col);
    }

private:
    Image<RGBA>& image_;
    RGBAConverter<DataType> const& converter_;

};

// specialization for Colour images. Do full conversion to linear space
// at construction.

// interesting NOTE: seems below specialization does not give any benefits
// in speed. Any gain from not using the pow() function for gamma conversion
// is offset by having each pixel take up 6 words instead of 1. (it seems)

// template <>
// class ImageTexture<Colour> : public Texture<Colour> {
// 
// public:
//     ImageTexture(Image<RGBA>& image) : image_(image.width(), image.height()) {
//         // convert RGBA image to Colour
//         for (int i = 0; i < image_.height(); ++i) {
//             for (int j = 0; j < image_.width(); ++j) {
//                 image_[i][j] = RGBAConverter<Colour>::fromRGBA(image[i][j]);
//             }
//         }
//     }
// 
//     Colour at(double u, double v) const {
//         return image_[int(v*image_.height())][int(u*image_.width())];
//     }
// 
// private:
//     Image<Colour> image_;
// 
// };

#endif // _IMAGE_TEXTURE_H_
