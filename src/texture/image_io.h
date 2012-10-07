#ifndef _IMAGE_IO_H_
#define _IMAGE_IO_H_

#include <fstream>

/* Functions that help with reading writing image files
 * to and from disk. The file format is very simple. */


/**
 * Given an input file stream, and a POD struct,
 * fill the struct from the file.
 */
template <typename T>
void readFromStream(std::ifstream& file, T& data) {
    file.read(reinterpret_cast<char*>( &data ), sizeof(data) );
}

/**
 * Given an output file stream, and a POD struct,
 * write the binary data of the struct to the file
 */
template <typename T>
void writeToStream(std::ofstream& file, T const& data) {
    file.write(reinterpret_cast<char const*>( &data ), sizeof(data) );
}

/**
 * A struct that stores attributes necessary to reconstruct an image
 */
struct ImageHeader {
    size_t width;
    size_t height;
    size_t pixelSize; ///< size of each pixel in bytes
};

/**
 * Write the @a image to an output file given by @a path.
 */
template <class ImageType>
bool writeImageToFile(ImageType const& image, std::string path) {

    std::ofstream file;
    file.open ( path.c_str(), std::ios::out | std::ios::binary );

    if ( !file )
    {
        std::cerr << "Image write error" << std::endl;
        std::cerr << "Could not open the output file." << std::endl;
        return false;
    }

    bool success = writeImageToFile(image, file);

    file.close();

    return success;
}


/**
 * Write the @a image to an output @a file stream.
 */
template <class ImageType>
bool writeImageToFile(ImageType const& image, std::ofstream& file) {
    
    ImageHeader header = { image.width(),
                           image.height(),
                           sizeof(typename ImageType::pixel_type) };
    
    // write out header
    writeToStream(file, header);

    // write out each pixel to file
    typedef typename ImageType::const_iterator iter;
    for (iter it = image.begin(); it != image.end(); ++it) {
        writeToStream(file, *it);
    }

    return true;
}

/**
 * Read and @return an image from a file given by @a path.
 *
 * Templated on image type.
 */
template <class ImageType>
ImageType readImageFromFile(std::string path) {

    std::ifstream file;
    ImageType image;

    file.open ( path.c_str(), std::ios::in | std::ios::binary );

    if ( !file )
    {
        std::cerr << "Image read error" << std::endl;
        std::cerr << "Could not open the input file." << std::endl;
        return image;
    }

    image = readImageFromFile<ImageType>(file);

    file.close();

    return image;
}


/**
 * Read an image from an input file stream.
 *
 * Templated on image type.
 */
template <class ImageType>
ImageType readImageFromFile(std::ifstream& file) {
    
    ImageHeader header;
    
    // read header
    readFromStream(file, header);

    // check that pixel byte size matches
    if (header.pixelSize != sizeof(typename ImageType::pixel_type) ) {
        std::cerr << "Image read error" << std::endl;
        std::cerr << "Pixel size mismatch. Pixel byte size in the file header differs from image type's pixel size." << std::endl;
        return ImageType();
    }

    ImageType image(header.width, header.height);

    // read each pixel from file
    typedef typename ImageType::iterator iter;
    for (iter it = image.begin(); it != image.end(); ++it) {
        readFromStream(file, *it);
    }

    return image;
}

#endif // _IMAGE_IO_H_
