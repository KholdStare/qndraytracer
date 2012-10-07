#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstddef>
#include <algorithm>

/**
 * Stores 2-dimensional pixel data.
 *
 * Pixels can be of any type PixelType- can be Colour with metadata,
 * or something completely different
 */
template <class PixelType>
class Image {

public:
    typedef Image<PixelType> SelfType;

    /**
     * Construct an image of dimensions @a width by @a height.
     *
     * Values of pixels are uninitialized
     */
    Image(size_t width, size_t height) : width_(width),
                                   height_(height),
                                   pixels_(new PixelType[width*height]) { }

    /**
     * Construct an invalid image.
     *
     * To get a valid image, do a copy/move assignment into this
     */
    Image() : width_(0), height_(0), pixels_(nullptr) { }

    /**
     * Deallocate the pixel data.
     */
    ~Image() {
        if (pixels_) {
            delete[] pixels_;
        }
    }

    /**
     * Copy another image's data into this image.
     */
    Image(SelfType const& other) : width_(other.width_),
                                   height_(other.height_) {

        delete[] pixels_; // TODO: use a RAII class for array
        pixels_ = new PixelType[width_*height_];
                                       
        // copy pixels
        std::copy(other.pixels_, other.pixels_ + width_*height_, pixels_);
    }

    /**
     * Copy another image's data into this image through assignment.
     */
    SelfType& operator=(SelfType const& other) {
        // free own resources
        delete[] pixels_;

        width_ = other.width_;
        height_ = other.height_;
        pixels_ = new PixelType[width_*height_];

        // copy pixels
        std::copy(other.pixels_, other.pixels_ + width_*height_, pixels_);
    }

    /**
     * Move construct this image from another.
     */
    Image(SelfType && other) : width_(other.width_),
                               height_(other.height_),
                               pixels_(other.pixels_) {
        other.invalidate();
    }

    /**
     * Move assign this image from another.
     */
    SelfType& operator=(SelfType && other) {
        // free own resources
        if (pixels_) { delete[] pixels_; }

        width_ = other.width_;
        height_ = other.height_;
        pixels_ = other.pixels_;
        other.invalidate();
    }

    /** @Return whether this image is valid */
    bool isValid() const { return pixels_; }
    /** Equivalent to isValid() */
    operator bool() const { return isValid(); }

    /** @Return the width of this image. */
    size_t width() const { return width_; }
    /** @Return the height of this image. */
    size_t height() const { return height_; }

    /**
     * Index into a row of the image.
     * 
     * Allows to access pixels like image[i][j]
     */
    PixelType* operator[]( int y ) const  { return pixels_ + y*width_; }

    typedef PixelType pixel_type;
    typedef pixel_type* iterator;
    typedef pixel_type const* const_iterator;

    // iterators over pixels
    iterator begin() { return pixels_; }
    iterator end()   { return pixels_ + width_*height_; }

    const_iterator begin() const { return pixels_; }
    const_iterator end()   const { return pixels_ + width_*height_; }

private:
    /**
     * Invalidate internal representation. Useful for move semantics
     */
    void invalidate() {
        width_ =  0;
        height_ = 0;
        pixels_ = nullptr; // don't delete
    }

    size_t width_; ///< image width
    size_t height_; ///< image height
    PixelType* pixels_; ///< 2D array of pixels

};

#endif // _IMAGE_H_
