#include "threadrand.h"
#include <memory>
#include <omp.h>

namespace qnd {
    // TODO: instead of double indirection to fix false sharing,
    // put states in structs the size of cache-lines
    std::unique_ptr<unsigned int> randThreadStates[maxThreadNum];

    // reinitialize the array of thread state pointers
    void init_threadrand() {
        for (int i = 0; i < maxThreadNum; ++i) {
            randThreadStates[i].reset(new unsigned int(std::rand()));
        }
    }

    // requires openmp, for thread number
    int threadrand() {
        int i = omp_get_thread_num();
        return rand_r(randThreadStates[i].get());
    }

}
