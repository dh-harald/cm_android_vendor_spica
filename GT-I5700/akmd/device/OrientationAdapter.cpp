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
    
//   LOGD("EARTH: x=%d y=%d z=%d",(int)a.x,(int)a.y,(int)a.z);    
    Vector ref(-1, 0, 0);
    Vector o1 = earth.cross(ref);
    Vector o2 = earth.cross(o1);
//    LOGD("o1: x=%d y=%d z=%d",(int)o1.x,(int)o1.y,(int)o1.z);    
//    LOGD("o2: x=%d y=%d z=%d",(int)o2.x,(int)o2.y,(int)o2.z);    
    /* Now project magnetic field on components o1 and o2. */
    Vector m = magnetometer->read();
//    LOGD("m : x=%d y=%d z=%d",(int)m.x,(int)m.y,(int)m.z);    
    float o1l = m.dot(o1) * o2.length();
    float o2l = m.dot(o2) * o1.length();

    /* Establish the angle in E */
    float y = 180.0f - rad2deg(atan2f(o2l, o1l));
    
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
    
//    LOGD("OR: y=%d p=%d r=%d",(int)y,(int)p,(int)r);
    return Vector(y, p, r);
}

}
