#include "sensor.h"
#include "../utils/algorithm_utils.hpp"

bool mergeSensors(Image<SensorPixel>& accumulator, Image<SensorPixel> const& other) {
    // make sure dimensions are correct
    if (accumulator.width() != other.width() || accumulator.height() != other.height()) {
        return false;
    }

    // add all the values into the accumulator
    qnd::elementWiseAdd(accumulator, other);

    return true;
}
