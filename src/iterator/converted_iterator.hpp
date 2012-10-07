#ifndef _QND_CONVERTED_ITERATOR_HPP
#define _QND_CONVERTED_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace qnd {

    // Takes an iterator and a conversion function object to 
    // create a new iterator that ierates over the converted values
    template <class Iter, class Conv>
    class converted_iterator :
            public boost::iterator_facade< converted_iterator<Iter, Conv>,
                                   typename Conv::result_type const,
                                   boost::forward_traversal_tag,
                                   typename Conv::result_type const> {

    public:
        explicit converted_iterator(int n, bool end) : _i(n, end) { }

        static int round(int iterations) { return Iter::round(iterations); }

    private:
        friend class boost::iterator_core_access;

        void increment() { ++_i; }

        bool equal(converted_iterator<Iter, Conv> const& other) const {
            return _i == other._i;
        }
        
        typename Conv::result_type dereference() const {
            return Conv::convert(*_i);
        }

    private:
        Iter _i;

    };


    // compose two converter objects.
    // Conv1 is applied first, followed by Conv2.
    // wouldn't variadic templates be nice here?
    // TODO: C++11 compose several converter classes
    // with variadics
    template <class Conv1, class Conv2>
    struct composed_converter {
        typedef typename Conv2::result_type result_type;
        typedef typename Conv1::argument_type argument_type;

        static result_type convert(argument_type arg) {
            return Conv2::convert(Conv1::convert(arg));
        }
    };



}

#endif // _QND_CONVERTED_ITERATOR_HPP
