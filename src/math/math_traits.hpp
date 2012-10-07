
#ifndef _QND_MATH_TRAITS_HPP
#define _QND_MATH_TRAITS_HPP

#include <array>

// TODO: use coordinate_base? this is in a transition state.

    template <typename T, int Dim >
    struct coordinate_traits {
        typedef std::array<T, Dim> type;
    };

    template <typename T >
    struct coordinate_traits<T, 1> {
        typedef T type;
    };

#endif // _QND_MATH_TRAITS_HPP
