#ifndef _MATERIAL_PARAMETER_H_
#define _MATERIAL_PARAMETER_H_

#include "texture.h"
#include "uv_map.h"

/**
 * represents a parameter of a material, of a certain type
 * (eg. Colour, or double). Any material parameter could
 * then be parameterized by textures of the same type.
 *
 * A constant value or a texture can be assigned.
 * A texture takes precedence.
 */
template <class DataType>
class MaterialParameter {

public:
    typedef DataType data_type;

    MaterialParameter() : val_(), tex_(nullptr) { }

    /**
     * Initialize the material parameter with a single constant value
     */
    explicit MaterialParameter(DataType const& val) : val_(val), tex_(nullptr) { }
    ~MaterialParameter() { }

    // Methods for setting the constant value and the texture
    // If texture is not nullptr, it takes precedence over the constant

    void set(DataType const& val) { val_ = val; }
    void set(Texture<DataType>* tex) { tex_ = tex; }

    // Getter methods
    Texture<DataType>* getTexture() const { return tex_; }
    DataType getVal() const { return val_; }

    /**
     * Get the appropriate value at the position using uv coordinates.
     */
    DataType at(double u, double v) const {
        if (tex_) {
            return tex_->at(u, v);
        }
        return val_;
    }

    /**
     * Get the appropriate value at the position using uv coordinates.
     */
    DataType at(UVPoint uv) const { return at(uv[0], uv[1]); }

private:
    DataType val_;
    Texture<DataType>* tex_;

};

#endif // _MATERIAL_PARAMETER_H_
