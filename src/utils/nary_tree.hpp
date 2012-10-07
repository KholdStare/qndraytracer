#ifndef _QND_NARY_TREE_HPP_
#define _QND_NARY_TREE_HPP_

#include <vector>

namespace qnd {

    // A class representing an N-ary tree of T values
    // NOT IMPLEMENTED
    template <typename T, int N>
    class N_AryTree {

    public:
        N_AryTree() _tree(1) { }

        // insert nth child?


    private:
        struct Entry {
            Entry() : val(), isValid(false) { }

            T    val;
            bool isValid;
        }

        std::vector<Entry> _tree;

    }



}

#endif // _QND_NARY_TREE_HPP_
