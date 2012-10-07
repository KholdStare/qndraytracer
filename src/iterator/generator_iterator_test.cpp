#include "generator_iterator.hpp"
#include "converted_iterator.hpp"
#include "stratified_iterator.hpp"
#include "../threadrand.h"
#include <string>

using namespace std;

struct generator {
    typedef int result_type;

    static result_type generate () {
        return 42;
    }
};

struct divider {
    typedef double result_type;
    typedef int argument_type;

    static result_type convert (argument_type num) {
        return result_type(num) / 5;
    }
};

struct stringifier {
    typedef string result_type;
    typedef double argument_type;

    static result_type convert (argument_type num) {
        return std::to_string(num);
    }
};

int main (int argc, char* argv[]) {
    
    // calls the above generator's function specified number of times.
    // Fed through converters using an iterator interface.
    typedef qnd::generator_iterator<generator> iter;
    typedef qnd::composed_converter<divider, stringifier> converter;
    typedef qnd::converted_iterator<iter, converter> conv_iter;
    typedef qnd::generator_iterator_builder<conv_iter > gen_stuff;
    

    for (auto output : gen_stuff(10) ) {
        std::cout << output << std::endl;
    }

    qnd::init_threadrand();

    const int subdivisions = 5;
    typedef qnd::stratified_iterator<qnd::uniform_generator<1> > strat_iter;
    typedef qnd::generator_iterator_builder<strat_iter> stratified_sampler;

    for (auto output : stratified_sampler(subdivisions*subdivisions) ) {
        std::cout << output[0] << " " << output[1] << std::endl;
    }

    return 0;
}


