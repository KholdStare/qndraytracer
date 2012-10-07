#include "data_xml_parser.h"
#include "tinyxml.h"
#include "colour.h"
#include "math/math_types.h"

template <>
Colour DataXmlParser<Colour>::parse( TiXmlElement* colourElement) {

    // construct greyscale colour from a single value
    double val;
    if(colourElement->QueryValueAttribute("val", &val) == TIXML_SUCCESS) {
        return Colour(val);
    }

    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    colourElement->QueryValueAttribute("r", &r);
    colourElement->QueryValueAttribute("g", &g);
    colourElement->QueryValueAttribute("b", &b);

    return Colour(r, g, b);
}

template <>
Point3D DataXmlParser<Point3D>::parse( TiXmlElement* pointElement) {

    // construct point from a single value
    double val;
    if(pointElement->QueryValueAttribute("val", &val) == TIXML_SUCCESS) {
        return Point3D(val, val, val);
    }

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    pointElement->QueryValueAttribute("x", &x);
    pointElement->QueryValueAttribute("y", &y);
    pointElement->QueryValueAttribute("z", &z);

    return Point3D(x, y, z);
}

template <>
double DataXmlParser<double>::parse( TiXmlElement* element) {

    double val = 0.0;
    element->QueryValueAttribute("val", &val);

    return val;
}
