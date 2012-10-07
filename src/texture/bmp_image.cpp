#include "bmp_image.h"
#include "../bmp_io.h"
#include <iostream>

Image<RGBA>* readBmpFromFile(std::string const& filename) {
    unsigned char* rbuffer;
    unsigned char* gbuffer;
    unsigned char* bbuffer;

    unsigned long int width;
    long int height;

    Image<RGBA>* image = NULL;

    if ( bmp_read( filename.c_str(), &width, &height, &rbuffer, &gbuffer, &bbuffer ) ) {
        std::cerr << "Failed to read bmp file." << std::endl;
        return image;
    }

    image = new Image<RGBA>(width, height);

    for (int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            RGBA& col = (*image)[i][j];
            col.r = rbuffer[i*width+j];
            col.g = gbuffer[i*width+j];
            col.b = bbuffer[i*width+j];
        }
    }

    // delete buffers
    delete[] rbuffer;
    delete[] gbuffer;
    delete[] bbuffer;

    return image;
}

bool writeBmpToFile(Image<RGBA> const& image, std::string const& filename) {
    // allocate bytes to be written out to disk as bmp data
    int numbytes = image.height() * image.width() * sizeof(unsigned char);
    unsigned char* rbuffer = new unsigned char[numbytes];
    std::fill_n(rbuffer, numbytes, 0);
    unsigned char* gbuffer = new unsigned char[numbytes];
    std::fill_n(gbuffer, numbytes, 0);
    unsigned char* bbuffer = new unsigned char[numbytes];
    std::fill_n(bbuffer, numbytes, 0);

    // put color data for each pixel into buffers
    for (size_t i = 0; i < image.height(); ++i) {
        for (size_t j = 0; j < image.width(); ++j) {
            RGBA const& col = image[i][j];

            rbuffer[i*image.width()+j] = col.r;
            gbuffer[i*image.width()+j] = col.g;
            bbuffer[i*image.width()+j] = col.b;
        }
    }

    // write out to file and delete buffers
    bool success = !bmp_write( filename.c_str(), image.width(), image.height(), rbuffer, gbuffer, bbuffer );
    delete[] rbuffer;
    delete[] gbuffer;
    delete[] bbuffer;

    return success;
}
