#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "image.h"
#include "../colour.h"
#include "rgba_converter.hpp"

/**
 * A structure that stores an accumulated Colour and
 * the number of sample taken.
 */
struct SensorPixel {
    SensorPixel() : col(), samples(0) { }
    Colour col;
    unsigned int samples;

    /**
     * Add a sample of color to the sensor pixel
     */
    SensorPixel& operator +=(Colour const& other) {
        col += other;
        // increment sample count
        samples++;
        return *this;
    }

    /**
     * Combine two sets of samples
     */
    SensorPixel& operator +=(SensorPixel const& other) {
        col += other.col;
        // sum number of samples
        samples += other.samples;
        return *this;
    }

    /**
     * Returns final colour based on number of samples and
     * accumulated Colour.
     */
    Colour normalizedColour() const {
        if (samples == 0) { return Colour(); }

        return col / double(samples);
    }

};

/**
 * Combine the samples gathered on two sensors.
 *
 * @return true on success.
 */
bool mergeSensors(Image<SensorPixel>& accumulator, Image<SensorPixel> const& other);


// Sensor specializations for RGBAConverter ==============================
template <>
inline RGBA RGBAConverter<SensorPixel>::toRGBA (SensorPixel const& data, double gamma) {
    return RGBAConverter<Colour>::toRGBA(data.normalizedColour(), gamma);
}

template <>
inline SensorPixel RGBAConverter<SensorPixel>::fromRGBA(RGBA const& col, double gamma) {
    SensorPixel pixel;
    pixel += RGBAConverter<Colour>::fromRGBA(col, gamma);
    return pixel;
}

// =======================================================================

#endif // _SENSOR_H_
