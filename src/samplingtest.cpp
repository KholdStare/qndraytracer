#include "samplingutils.h"
#include "threadrand.h"
#include <iostream>

using namespace std;
using namespace boost;

int main (int argc, char* argv[]) {

    copy(
            make_function_input_iterator(qnd::genRandomBetween01, 0),
            make_function_input_iterator(qnd::genRandomBetween01, 10),
            ostream_iterator<int>(cout, " ")
        );


    return 0;
}
