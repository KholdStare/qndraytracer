#ifndef _RGBA_CONVERTER_H_
#define _RGBA_CONVERTER_H_

#include "rgba.h"
#include "../colour.h"
#include <algorithm>

/* This file defines conversion functions to and from different
 * types. This enables rgba images to be used as textures for
 * many different properties (colours, values, or even normals) */

/**
 * Templated class for conversions to and from RGBA format and
 * a custom DataType.
 *
 * Specialize the static functions for your DataType.
 */
template <class DataType>
struct RGBAConverter {

    double gamma; ///< a gamma value for this converter

    // default implementations are naive.
    // Will throw compiler errors if implicit conversion is not implemented
    // Specialize static functions to get full benefit

    RGBA     toRGBA  (DataType const& data) const {
        return toRGBA(data, gamma);
    }

    DataType fromRGBA(RGBA const& col) const {
        return fromRGBA(col, gamma);
    }

    static RGBA     toRGBA  (DataType const& data, double gamma) {
        return RGBA(data);
    }

    static DataType fromRGBA(RGBA const& col, double gamma) {
        return DataType(col);
    }

};

/**
 * A convenience function, that uses the appropriate RGBAConverter,
 * to @return an RGBA image from the given @a image.
 */
template <class DataType>
inline Image<RGBA> convertImageToRGBA(Image<DataType> const& image,
                               RGBAConverter<DataType> const& converter) {

    Image<RGBA> convertedImage(image.width(), image.height());

    // copy from one image to another through transformation
    std::transform(std::begin(image), std::end(image), std::begin(convertedImage),
                   [&converter](DataType const& d){ return converter.toRGBA(d); });

    return convertedImage;
}

/**
 * A convenience function, that uses the appropriate RGBAConverter,
 * to generate a DataType image (type up to the user)
 * from the given RGBA image.
 */
template <class DataType>
inline Image<DataType> convertImageFromRGBA(Image<RGBA> const& image,
                               RGBAConverter<DataType> const& converter) {

    Image<RGBA> convertedImage(image.width(), image.height());

    // copy from one image to another through transformation
    std::transform(std::begin(image), std::end(image), std::begin(convertedImage),
                   [&converter](RGBA const& c){ return converter.fromRGBA(c); });

    return convertedImage;
}

/***************************************************************************
 *                         Colour Specializations                          *
 ***************************************************************************/

template <>
inline RGBA RGBAConverter<Colour>::toRGBA (Colour const& data, double gamma) {
    RGBA col;
    Colour clampedData(data);
    clampedData.clamp();

    col.r = int(gammaCorrect(clampedData[0], gamma)*255);
    col.g = int(gammaCorrect(clampedData[1], gamma)*255);
    col.b = int(gammaCorrect(clampedData[2], gamma)*255);
    return col;
}

template <>
inline Colour RGBAConverter<Colour>::fromRGBA(RGBA const& col, double gamma) {

    double inverseGamma = 1/gamma;
    return Colour(gammaCorrect(double(col.r)/255, inverseGamma),
                  gammaCorrect(double(col.g)/255, inverseGamma),
                  gammaCorrect(double(col.b)/255, inverseGamma));
}

// =======================================================================


/***************************************************************************
 *                         Double Specializations                          *
 ***************************************************************************/

template <>
inline RGBA RGBAConverter<double>::toRGBA (double const& data, double gamma) {
    RGBA col;

    double d = data;
    // clamp double
    if (d < 0.0) {
        d = 0.0;
    }
    else if (d > 1.0) {
        d = 1.0;
    }

    // convert to [0,255] integer range
    unsigned char val = (unsigned char)(d*255);
    col.r = val;
    col.g = val;
    col.b = val;
    return col;
}

template <>
inline double RGBAConverter<double>::fromRGBA(RGBA const& col, double gamma) {
    // get average assuming double values range in [0,1]
    return (double(col.r) + double(col.g) + double(col.b))
           / (255 * 3);
}

// =======================================================================
#endif // _RGBA_CONVERTER_H_
