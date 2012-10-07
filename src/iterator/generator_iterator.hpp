/* allows iterating over static nullary function's output.
 * Author: Alexander Kondratskiy
 */

#ifndef _QND_GENERATOR_ITERATOR_HPP
#define _QND_GENERATOR_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace qnd {

    /**
     * Iterates over a generator's output an integer number of times
     */
    template <class Gen>
    class generator_iterator : 
            public boost::iterator_facade< generator_iterator<Gen>,
                                   typename Gen::result_type const,
                                   boost::forward_traversal_tag,
                                   typename Gen::result_type const > {

    public:
        explicit generator_iterator(int n, bool end) : _i((end ? n : 0)) { }

        /**
         * Adjust the final number of iterations if needed
         * (e.g. in stratified sampling has to be a perfect square)
         */
        static inline int round(int iterations) { return iterations; }

    private:
        friend class boost::iterator_core_access;

        void increment() { ++_i; }

        bool equal(generator_iterator<Gen> const& other) const {
            return _i == other._i;
        }
        
        typename Gen::result_type dereference() const {
            return Gen::generate();
        }

    private:
        int _i;

    };

    /**
     * Can be iterated over like a container and used in algorithms
     *
     * Think of this as a generator in python that yields values.
     */
    template <class GenIter>
    class generator_iterator_builder {

    public:
        typedef GenIter const_iterator;
        typedef const_iterator iterator;

        generator_iterator_builder(int iterations) :
                _n(GenIter::round(iterations)) { }

        int n() const { 
            return _n;
        }
        
        GenIter begin() const {
            return GenIter(_n, false);
        }

        GenIter end() const {
            return GenIter(_n, true);
        }

    private:
        int _n;

    };

}

#endif // _QND_GENERATOR_ITERATOR_HPP
