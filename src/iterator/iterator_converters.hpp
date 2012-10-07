
#ifndef _QND_ITERATOR_CONVERTERS_HPP
#define _QND_ITERATOR_CONVERTERS_HPP

#include "../math/math_traits.hpp"

namespace qnd {

    /**
     * An iterator converter that converts a 2D sample into
     * a 3D direction on the hemisphere.
     */
    struct hemisphere_converter {
        typedef coordinate_traits<double, 2>::type argument_type;
        typedef coordinate_traits<double, 3>::type result_type;

        static result_type convert(argument_type const& arg) {

            // adapted from sphere sampling method from shirley et al 1996
            double const& psi1 = arg[0];
            double const& psi2 = arg[1];

            double term1 = 2*sqrt(psi1 * (1 - psi1));
            double term2 = 2*M_PI*psi2;

            result_type result = { { cos(term2)*term1,
                                     sin(term2)*term1,
                                     fabs(1.0 - 2.0*psi1) } };
            return result;
        }
    };
}

#endif // _QND_ITERATOR_CONVERTERS_HPP
