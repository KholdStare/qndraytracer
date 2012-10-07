#ifndef _RGBA_H_
#define _RGBA_H_

/**
 * Simple pixel struct with 8bit rgba components
 */
struct RGBA {
    RGBA() : r(0), g(0), b(0), a(0) { }

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#endif // _RGBA_H_
