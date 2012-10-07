BOOST_PATH = /usr/include/
# EIGEN_PATH = /usr/include/eigen3/
# Temporarily using unstable release of Eigen to fix gcc4.7 bug
EIGEN_PATH = /home/kholdstare/tools/eigen3_devel/
DEBUG_FLAGS = -g
DEFINES = -DTIXML_USE_STL

# Define C++ compiler
CCC	          = g++-4.7

# Define C++ compiler options
CCCFLAGS      = $(DEBUG_FLAGS) -std=c++11 -c -O2 -Wall -Werror -fopenmp

# Define C/C++ pre-processor options
CPPFLAGS      = $(DEFINES) -I$(EIGEN_PATH) -I$(BOOST_PATH) -Itinyxml

# Define the location of the destination directory for the executable file
DEST	      = .

# Define flags that should be passed to the linker
LDFLAGS	      = $(DEBUG_FLAGS) -fopenmp

# Define libraries to be linked with
LIBS = -lm -ltinyxml

# Define linker
LINKER	      = g++-4.7

# Define all object files to be the same as CPPSRCS but with all the .cpp and .c suffixes replaced with .o
OBJ           = $(CPPSRCS:.cpp=.o) $(CSRCS:.c=.o)

# Define name of target executable
PROGRAM	          = raytracer

# Define all C++ source files here
CPPSRCS = main.cpp raytracer.cpp light_source.cpp \
		scene_object.cpp bmp_io.cpp camera.cpp \
		threadrand.cpp scene.cpp \
		xml_utils.cpp scene_object_factory.cpp light_source_factory.cpp \
		bounding_volume.cpp light_volume.cpp \
		sampling_strategy.cpp sampling_strategy_group.cpp \
		fresnel.cpp texture/texture_parser.cpp \
		texture/material.cpp data_xml_parser.cpp \
		texture/bmp_image.cpp texture/sensor.cpp mesh/obj_store.cpp \
		mesh/obj_parse.cpp mesh/mesh.cpp kdtree/kd_tree.cpp \
        mesh/face.cpp math/math_types.cpp ray.cpp colour.cpp

##############################################################################
# Define additional rules that make should know about in order to compile files.                                        
##############################################################################

# Define default rule if Make is run without arguments
all : $(PROGRAM)

# Define rule for compiling all C++ files
%.o : %.cpp
	$(CCC) $(CCCFLAGS) $(CPPFLAGS) -o $*.o $*.cpp
	
# Define rule for creating executable
$(PROGRAM) :	$(OBJ)
		@echo -n "Loading $(PROGRAM) ... "
		$(LINKER) $(LDFLAGS) $(OBJ) $(LIBS) -o $(PROGRAM)
		@echo "done"
		
# Define rule to clean up directory by removing all object, temp and core
# files along with the executable
clean :
	@rm -f $(OBJ) core $(PROGRAM)

