#ifndef _OBJ_PARSE_H_
#define _OBJ_PARSE_H_

#include <string>
#include <fstream>

class ObjStore;

/**
 * Namespace for OBJ parsing functions
 */
namespace ObjParser {

    /**
     * Given a @a path to an OBJ file, populate the @a obj structure.
     * 
     * @return true iff opening/parsing the file succeeds
     */
    bool parse(std::string path, ObjStore& obj);
    
    /**
     * Given a filestream of an OBJ @a file, parse and populate the @obj
     * structure.
     */
    void parse(std::ifstream& file, ObjStore& obj);

}


#endif // _OBJ_PARSE_H_
