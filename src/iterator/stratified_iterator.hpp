#ifndef _QND_STRATIFIED_ITERATOR_HPP
#define _QND_STRATIFIED_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/array.hpp>
#include <math.h>

namespace qnd {
    
    inline int closestSquareRoot(int num) {
        return int(sqrt(double(num)));
    }

    /**
     * Given a static generator that generates uniformly distributed
     * numbers from 0-1, makes the samples stratified in 2-d
     */
    template < class UnitGen>
    class stratified_iterator :
            public boost::iterator_facade< stratified_iterator<UnitGen>,
                                   boost::array<typename UnitGen::result_type, 2> const,
                                   boost::forward_traversal_tag,
                                   boost::array<typename UnitGen::result_type, 2> const> {

    public:
        explicit stratified_iterator(int n, bool end) :
                    _n(closestSquareRoot(n)),
                    _i((end ? (_n) : 0)),
                    _j(0) { }

        static int round(int iterations) {
            int n = closestSquareRoot(iterations);
            return n*n;
        }

    private:
        friend class boost::iterator_core_access;
        typedef boost::array<typename UnitGen::result_type, 2> result_type;

        void increment() { 
            if (_j == (_n - 1)) {
                ++_i;
                _j = 0;
            }
            else {
                ++_j;
            }
        }

        bool equal(stratified_iterator<UnitGen> const& other) const {
            return _i == other._i && _j == other._j;
        }
        
        result_type dereference() const {
            result_type result = { { (double(_i) + UnitGen::generate()) / _n, (double(_j) + UnitGen::generate()) / _n } };
            return result;
        }

    private:
        int const _n;
        // indeces for which subsection we are sampling from
        int _i;
        int _j;

    };

}

#endif // _QND_STRATIFIED_ITERATOR_HPP
