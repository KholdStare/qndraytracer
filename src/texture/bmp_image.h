#ifndef _BMP_IMAGE_H_
#define _BMP_IMAGE_H_

#include "image.h"
#include "rgba.h"
#include <string>

Image<RGBA>* readBmpFromFile(std::string const& filename);
bool writeBmpToFile(Image<RGBA> const& image, std::string const& filename);

#endif // _BMP_IMAGE_H_
