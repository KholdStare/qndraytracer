#include "colour.h"


std::ostream& operator <<(std::ostream& s, const Colour& c)
{
    return s << "c(" << c[0] << "," << c[1] << "," << c[2] << ")";
}

