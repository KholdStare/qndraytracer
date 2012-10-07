#ifndef _POINTER_MAP_HPP_
#define _POINTER_MAP_HPP_

#include <unordered_map>
#include <string>

/**
 * A class that stores and owns pointers to objects, which
 * can be accessed by name, through a hashmap.
 * All pointers are deallocated on destruction.
 */
template <typename T>
class PointerMap {
    typedef std::unordered_map<std::string, T* > MapType;
    typedef typename MapType::const_iterator iter_type;

public:
    PointerMap() { }
    ~PointerMap() {
        // free the memory associated with the pointers
        for (iter_type it = _map.begin(), end =_map.end(); it != end; ++it) {
            delete it->second;
        }
        _map.clear();
    }

    // Make object not assignable or copyable
    PointerMap(PointerMap const&) = delete;
    PointerMap& operator=(PointerMap const&) = delete;

    /**
     * Hand over ownership of pointer to this map
     */
    void insert(std::string const& name, T* p) {
        _map.insert( make_pair(name, p) );
    }

    /**
     * Get the pointer associated with the @a name
     */
    T* get(std::string const& name) const {
        iter_type it = _map.find( name );

        if (it == _map.end()) {
            return nullptr;
        }

        return it->second;
    }

private:
    MapType _map;

};

#endif // _POINTER_MAP_HPP_
