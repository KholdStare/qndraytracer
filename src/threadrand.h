/***********************************************************
    A hacky implementation of independent random number generation
    for separete threads.
***********************************************************/

#ifndef _QND_THREAD_RAND_H_
#define _QND_THREAD_RAND_H_

#include <cstdlib>
#include <array>
#include <algorithm>

// quick n dirty namespace
namespace qnd {
    const int maxThreadNum = 100; ///< max number of thread states to store

    void init_threadrand(); ///< call this before calling threadrand() to initialize states.

    /**
     * Generates random number between 0 and RAND_MAX
     * requires openmp, for thread number
     */
    int threadrand();

    inline double genRandomBetween01() {
        return threadrand() / double(RAND_MAX);
    }

    inline double genRandomBetween(double a, double b) {
        return a + (b-a) * genRandomBetween01();
    }

    /**
     * A generator that can be plugged into samplers
     */
    template <int Dim>
    struct uniform_generator {
        typedef std::array<double, Dim> result_type;

        static result_type generate() {
            result_type result;
            std::generate(result.begin(), result.end(), genRandomBetween01);
            return result;
        }
    };

    template <>
    struct uniform_generator<1> {
        typedef double result_type;

        static result_type generate() {
            return genRandomBetween01();
        }
    };
}

#endif // _QND_THREAD_RAND_H_
