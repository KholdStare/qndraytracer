#include "obj_parse.h"
#include "obj_store.h"

#include <sstream>
#include <fstream>
#include <iostream>

namespace ObjParser {

    /**
     * Parse @a line and add vertex to @a obj
     */
    void parseVertex(std::stringstream& line, ObjStore& obj) {
        Point3D vertex;
        for (int i = 0; i < 3; ++i) {
            line >> vertex[i];
        }

        obj.addVertex(vertex);
    }

    /**
     * Parse @a line and add face to @a obj
     */
    void parseFace(std::stringstream& line, ObjStore& obj) {
        TriangleIndices indices;
        int index;
        for (int i = 0; i < 3; ++i) {
            line >> index;

            // indices in obj file start at 1
            indices[i] = index-1;
        }

        obj.addFace(indices);
    }
    

    bool parse(std::string path, ObjStore& obj) {
        std::ifstream file;

        file.open ( path.c_str() );

        if ( !file )
        {
            std::cerr << "Obj file read error" << std::endl;
            std::cerr << "Could not open the input file." << std::endl;
            return false;
        }

        parse(file, obj);

        file.close();

        return true;
    }

    void parse(std::ifstream& file, ObjStore& obj) {
        std::string line;
        std::stringstream lineStream;
        char firstChar;

        std::cout << "Parsing Obj!" << std::endl;

        while ( std::getline( file, line ) ) {

            // reject empty lines
            if ( line.empty() ) {
                continue;
            }

            // make stream from line
            lineStream.clear();
            lineStream.str(line);

            // check first char to determine type
            lineStream.get(firstChar);

            if (firstChar == 'v') {
                parseVertex(lineStream, obj);
            }
            else if (firstChar == 'f') {
                parseFace(lineStream, obj);
            }
        }
    }

}
