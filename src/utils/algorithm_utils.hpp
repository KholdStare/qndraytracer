#ifndef _QND_TEMPLATE_UTILS_HPP_
#define _QND_TEMPLATE_UTILS_HPP_

namespace qnd {

    /**
     * Given an accumulator container, and another container,
     * add the value of each element from the second container to
     * the values from the first. This happens in-place to the 
     * first accumulator container.
     * Assumes other contains at least as many elements as the
     * accumulator */
    template <typename T, typename U>
    void elementWiseAdd(T& accumulator, U const& other) {

        auto otherIter = other.begin();
        for (auto& acc : accumulator) {
            acc += *(otherIter++);
        }

    }

}

#endif // _QND_TEMPLATE_UTILS_HPP_
