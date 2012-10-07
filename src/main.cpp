#include "raytracer.h"
#include "xml_utils.h"
#include "scene.h"

#include "texture/image_io.h"
#include <iostream>

int main(int argc, char* argv[])
{

    // init random number generator for distribution rendering
    std::srand(int(time(nullptr)));

    if (argc <= 1) {
        std::cerr << "No Scene XML specified. If more than one XML spceified, scenes and settings will be combined into one." << std::endl;
        std::cerr << "    Usage:" << std::endl;
        std::cerr << "    ./raytracer <path to xml> ..." << std::endl;
        return 0;
    }

    // Create the 3 main objects that handle all functionality
    Scene scene;
    Raytracer raytracer;
    raytracer.setScene(&scene);
    CameraContainer cameras;


    // parse all scene info
    SceneXmlParser xmlParser(raytracer, scene, cameras);
    for (int i = 1; i < argc; ++i) {
        std::string sceneFilename(argv[i]);

        if(!xmlParser.parseSceneDefinition(sceneFilename) ) {
            std::cerr << "Parsing failed... Exiting." << std::endl;
            return 1;
        }
    }

    // preprocess the scene before rendering
    scene.preprocess();

    std::string bmpSuffix(".bmp");
    std::string rawSuffix(".rsd");

    // Render for each camera.
    for (auto& cam : cameras) {
        std::cout << "Rendering camera \"" << cam->name << "\"" << std::endl;

        // if previous raw sensor data exists,
        // use it as starting point
        std::string rawFileName = cam->name + rawSuffix;
        std::ifstream rawFile(rawFileName.c_str());
        if (rawFile.good()) {
            Image<SensorPixel> accummulatedSensor = readImageFromFile<Image<SensorPixel> >(rawFile);

            if (accummulatedSensor) {
                std::cout << "Reusing previously rendered data for iterative raytacing." << std::endl;
                cam->mergeSensor(accummulatedSensor);
            }

            rawFile.close();
        }

        // render and dump to file
        raytracer.render(*cam.get());
        cam->dumpToBMP(cam->name + bmpSuffix);

        // if raytracer flag says to also dump raw, do so
        if (raytracer.dumpRaw) {
            cam->dumpRawData(rawFileName);
        }
    }

    return 0;
}

