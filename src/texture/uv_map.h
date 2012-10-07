#ifndef _UV_MAP_H_
#define _UV_MAP_H_

#include "../math/math_traits.hpp"
#include "../math/math_types.h"

typedef coordinate_traits<double, 2>::type UVPoint;

// An abstract class defining a way to go between points
// in texture uv space to points in model space and vice versa.
// currently UNUSED
class UVMap {

public:

    UVMap();
    virtual ~UVMap();

    virtual Point3D uvToModel(UVPoint uv) const = 0;
    virtual UVPoint modelToUv(Point3D p) const = 0;

};


#endif // _UV_MAP_H_
