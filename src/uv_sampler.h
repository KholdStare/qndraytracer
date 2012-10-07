#ifndef _UV_SAMPLER_H_
#define _UV_SAMPLER_H_

#include "iterator/generator_iterator.hpp"
#include "iterator/stratified_iterator.hpp"
#include "threadrand.h"

typedef qnd::stratified_iterator<qnd::uniform_generator<1> > uv_iterator;
typedef qnd::generator_iterator_builder<uv_iterator> UVSampler;

#endif // _UV_SAMPLER_H_
