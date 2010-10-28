#include <fcntl.h>
#include <sys/ioctl.h>
#include <utils/Log.h>

#include "device/BMA020.hpp"
#include "linux-2.6.29/bma020.h"
#include "util.hpp"

namespace akmd {

BMA020::BMA020()
: index(0), accelerometer(3600)
{
    abuf[0] = abuf[1] = Vector();
    unsigned char param=0;
    
    fd = open(BMA020_NAME, O_RDONLY);
    SUCCEED(fd != -1);
    
    param = BMA020_RANGE_2G;
    SUCCEED(ioctl(fd, BMA020_SET_RANGE,     &param) == 0);
    
    param = BMA020_BW_50HZ;
    SUCCEED(ioctl(fd, BMA020_SET_BANDWIDTH, &param) == 0);
}

BMA020::~BMA020()
{
    SUCCEED(close(fd) == 0);
}

int BMA020::get_delay()
{
    return -1;
}

void BMA020::calibrate()
{
    const int REFRESH = 10;
    /* Demand length to match with the long-term average before the vector
     * is trusted to represent gravity. */
    const float ERROR = 0.05f;
    /* Exponential average applied on acceleration to estimate gravity. */
    const float GRAVITY_SMOOTH = 0.8f;

    accelerometer_g = accelerometer_g.multiply(GRAVITY_SMOOTH).add(a.multiply(1.0f - GRAVITY_SMOOTH));

    /* a and g must have about the same length and point to about same
     * direction before I trust the value accumulated to g */
    float al = a.length();
    float gl = accelerometer_g.length();

    if (al == 0 || gl == 0) {
        return;
    }

    Vector an = a.divide(al);
    Vector gn = accelerometer_g.divide(gl);

    if (fabsf(al - gl) < ERROR
        && an.dot(gn) > 1.0f - ERROR) {

        /* Going to trust this point. */
        accelerometer.update(next_update.tv_sec, accelerometer_g);
        if (accelerometer.fit_time <= next_update.tv_sec - REFRESH) {
            accelerometer.try_fit(next_update.tv_sec);
        }
    }

    a = a.add(accelerometer.center.multiply(-1));
    a = a.multiply(accelerometer.scale);
}

void BMA020::measure()
{
    SUCCEED(gettimeofday(&next_update, NULL) == 0);

    bma020acc_t accels;
    
    SUCCEED(ioctl(fd, BMA020_READ_ACCEL_XYZ, &accels) == 0);

    abuf[index] = Vector(-accels.y, accels.x, accels.z);

    index = (index + 1) & 1;

    a = abuf[0].add(abuf[1]).multiply(0.5f * (720.0f / 256.0f));
    calibrate();
}

Vector BMA020::read()
{
    return a;
}

void BMA020::start()
{
    unsigned char bmode = BMA020_MODE_NORMAL;
    SUCCEED(ioctl(fd, BMA020_SET_MODE, &bmode) == 0);
}

void BMA020::stop()
{        
    unsigned char bmode = BMA020_MODE_SLEEP;
    SUCCEED(ioctl(fd, BMA020_SET_MODE, &bmode) == 0);
}

}
