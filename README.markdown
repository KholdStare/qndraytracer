### Description

This is a project of mine that grew out of a universty assignment.
Currently it implements a renderer that uses monte carlo path tracing, with multiple importance sampling.

### Features

* Uses Monte Carlo Path Tracing for the ray tracing algorithm.
    * Automatically gives rise to several phenomena (GI, soft shadows)
    * Customizable diffuse and specular bounce depth per scene
    * Uses Multiple Importance Sampling to sample in more important directions and reduce variance when combining samples
* Arbitrary light sources- any object can be made to emit light.
    * Largely possible due to Multiple Importance Sampling framework
* Several physical phenomena are rendered:
    * reflection
    * refraction (with fresnel reflection)
    * light absorption in refractive media
    * depth of field
    * diffuse interreflection (consequence of path tracing)
    * soft shadows (consequence of arbitrary light objects)
* Simple XML based scene/render settings format
    * Many cameras can be placed, and customized (FOV, DOF, focus plane)
* OBJ mesh import (very limited subset at the moment)
* KD trees used for storing meshes, and intersecting with rays
* Multithreaded- uses all your cores to the max!

### Dependencies

To build the project, you must have the following libraries available:

* Boost (>= 1.48)
* tinyxml
* Eigen (>= 3.1, use latest development release if g++ 4.7.0 craps out, known issue with g++)

### Acknowledgements

This whole project had its humble beginning as an assignment for a graphics course at my university.
Although it has evolved much since then, it still retains some components for which I will give credit here.
I will not mention the specific course as I want to avoid current students being able to search for this code on Google and submit it as their assignment.

The original starter code for the assignment was written by Jack Wang in 2005.

The code uses John Burkardt's small bmp_io library for reading and writing images for output and textures. Details can be found [here]( http://people.sc.fsu.edu/~jburkardt/cpp_src/bmp_io/bmp_io.html ), and it is distributed under this [LGPL license](http://people.sc.fsu.edu/~jburkardt/txt/gnu_lgpl.txt).

