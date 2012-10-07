#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "math/math_types.h"
#include "colour.h"
#include "ray.h"

#include "uv_sampler.h"
#include "math/math_traits.hpp"
#include "texture/sensor.h"

/**
 * Initalize an transformation matrix from view to world coordinates,
 * given the @a eye position, @a view direction, and @up orientation.
 */
AffineTrans3D initInvViewMatrix( Point3D const& eye, Vector3D view, Vector3D up);

/**
 * Camera class to handle viewpoint, antialiasing and depth of field calculations
 */
class Camera {
public:

    /**
     * A function that returns a colour given a ray
     */
    typedef std::function<Colour (Ray3D&)> sampling_func;

    Camera(unsigned int width, unsigned int height,
           Point3D const& eye, Vector3D const& view, Vector3D const& up,
           double fov);

    Camera(unsigned int width, unsigned int height,
           double fov);

    ~Camera();

    /**
     * Set the scene sampling function that would return a colour given a ray
     */
    void setSamplingFunc(sampling_func func) {
        sceneSamplingFunc_ = func;
    }

    // getters for dimensions
    int width()  const { return sensor_.width(); }
    int height() const { return sensor_.height(); }

    // camera configuration methods
    void setEye (Point3D const& eye);
    void setView(Vector3D const& view);
    void setUp  (Vector3D const& up);
    void setGamma  (double gamma) { gamma_ = gamma; }
    void setAntialiasSamples(int n) { subSampler_ = UVSampler(n); }
    std::string name;

    /************************
     *  lens configuration  *
     ************************/
    
    /**
     * Set the aperture radius of the lens, for DOF.
     *
     * If aperture radius is 0, then treat camera as pinhole camera
     */
    void setApertureRadius(double r);

    /**
     * Set the number of samples to take on the surface of the aperture.
     * Currently the aperture is circular.
     */
    void setApertureSamples(int n) { apertureSampler_ = UVSampler(n); }

    /**
     * Set the distance to the focal plane
     */
    void setFocalDistance(double d);
    /************************/


    /*********************************************************
     *  utility methods for computing/dumping colour values  *
     *********************************************************/
    
    /**
     * Given another sensor, merge its samples with the sensor
     * of this camera.
     */
    bool mergeSensor(Image<SensorPixel> const& other);

    /**
     * Dump sensor data to BMP image at path @a filename.
     *
     * Uses the gamma provided to do the correct conversion
     * from linear space.
     *
     * TODO: move all output functionality to an output module.
     */
    bool dumpToBMP(std::string filename) const;

    /**
     * Dump the raw sensor data to a file on disk at @a filename.
     *
     * Can later be retrieved for iterative rendering.
     */
    bool dumpRawData(std::string filename) const;

    /**
     * Compute the pixel (i,j) on the sensor, using the 
     * dampling function provided earlier.
     */
    void computePixel(int i, int j) const;

    /**
     * Convenience method to compute a rectangular area on 
     * the sensor.
     */
    void computeArea(int iStart, int iEnd,
                     int jStart, int jEnd) const {

        for (int i = iStart; i < iEnd; ++i) {
            for (int j = jStart; j < jEnd; ++j) {
                computePixel(i, j);
            }
        }

    }
    /*********************************************************/

private:
    /**
     * Initialize some default values that are not done in the 
     * initialization list of the constructor
     */
    void initDefault();

    /**
     * Given a location on the sensor array, sample through the lens
     */
    Colour sampleLens(double x, double y) const;

    double factor_; ///< scaling factor on x,y position to map from image coordinates to camera coordinates
    double apertureRadius_; ///< radius of lens aperture for DOF
    double focalDistance_; ///< distance to focal plane from aperture

    Image<SensorPixel> sensor_; ///< accumulate samples on "sensor"

    double gamma_; ///< sensor gamma

    std::function<Colour (Ray3D&)> sceneSamplingFunc_; ///< function pointer to shade function 

    UVSampler subSampler_; ///< Sampler used for antialiasing
    UVSampler apertureSampler_; ///< Sampler used for sampling the aperture

    Point3D eye_; ///< position of the "eye" or aperture in world space
    Vector3D view_; ///< direction where the camera is facing
    /** Specifies camera orientation (up direction), and is orthogonal to the view_ vector.  */
    Vector3D up_; 

    AffineTrans3D viewToWorld_; ///< transformation matrix from camera coordinates to world space
};

#endif // _CAMERA_H_
