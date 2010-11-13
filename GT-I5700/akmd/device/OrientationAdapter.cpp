#include <math.h>
#include <utils/Log.h>

#include "device/OrientationAdapter.hpp"
#include "util.hpp"

namespace akmd {

OrientationAdapter::OrientationAdapter(ChipReader* accelerometer, ChipReader* magnetometer)
{
    this->accelerometer = accelerometer;
    this->magnetometer = magnetometer;
}

OrientationAdapter::~OrientationAdapter()
{
}

Vector OrientationAdapter::read() {
    Vector a = accelerometer->read();
    earth = earth.multiply(0.8f).add(a.multiply(0.2f));
    earth.z = -earth.z;
    
    Vector ref(-1, 0, 0);
    Vector o1 = earth.cross(ref);
    Vector o2 = earth.cross(o1);
    /* Now project magnetic field on components o1 and o2. */
    Vector m = magnetometer->read();
    float o1l = m.dot(o1) * o2.length();
    float o2l = m.dot(o2) * o1.length();

    /* azimuth */
    float y = rad2deg(atan2f(o2l, o1l));
    if(y<0) {
        y = 360.0f + y;
    }
    earth.z = -earth.z;
    
    /* pitch x*/
    float p = rad2deg(asinf(earth.y / earth.length()));
    if (earth.z < 0) { // means that phone is on rotation at -pi/2 to pi/2
        p = -180.0f - p;
        if (p < -180.0f) {
            p += 360.0f;
        }
    }

    /* roll y*/
    float r = 90.0f - rad2deg(acosf(earth.x / earth.length()));
    
    return Vector(y, p, r);
}

}
