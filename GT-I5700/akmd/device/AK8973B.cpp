#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <utils/Log.h>

#include "device/AK8973B.hpp"
#include "linux-2.6.29/ak8973b.h"

namespace akmd {

AK8973B::AK8973B(int dgain, int again)
: magnetometer(600)
{
    magnetometer.reset(true);
    mbuf = Vector();

    magnetometer_gain = dgain;
    fixed_magnetometer_gain = again;
    
    //starting offset
    analog_offset[0]=-3;
    analog_offset[1]=-3;
    analog_offset[2]=-5;
    
    fd = open("/dev/akm8973_daemon", O_RDONLY);
    SUCCEED(fd != -1);
    SUCCEED(ioctl(fd, ECS_IOCTL_RESET, NULL) == 0);
    calibrate_analog_apply();
}

AK8973B::~AK8973B() {
    SUCCEED(close(fd) == 0);
}

char AK8973B::akm_analog_offset(int i)
{
    signed char corr = analog_offset[i];
    if (corr < 0) {
        corr = 127 - corr;
    }
    return (char) corr;
}

void AK8973B::calibrate_analog_apply()
{
    char params[6] = {
        akm_analog_offset(0), akm_analog_offset(1), akm_analog_offset(2),
        fixed_magnetometer_gain, fixed_magnetometer_gain, fixed_magnetometer_gain,
    };

    for (int i = 0; i < 6; i ++) {
        char rwbuf[5] = { 2, AKECS_REG_HXDA+i, params[i] };
        SUCCEED(ioctl(fd, ECS_IOCTL_WRITE, &rwbuf) == 0);
    }
    
    digital_gain = powf(10.0f, (magnetometer_gain - fixed_magnetometer_gain) * 0.4f / 20.0f) * 16.0f;

    struct timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 300000;
    SUCCEED(nanosleep(&interval, NULL) == 0);
}

void AK8973B::calibrate_magnetometer_analog_helper(float val, int i)
{
    const float ANALOG_MAX = 126.0f;
    const float BOUND_MAX  = 240.0f;
    /* The rate of forgetting encountering the minimum or maximum bound.
    * Keeping this fairly large to make it less likely that analog gain
    * gets adjusted by mistake. */
    const float CALIBRATE_DECAY = 0.1f;

    /* Autoadjust analog parameters */
    if (val > ANALOG_MAX || val < -ANALOG_MAX) {
        analog_offset[i] += val > ANALOG_MAX ? -1 : 1;
        LOGI("Adjusting magnetometer axis %d to %d because of value %f", i, analog_offset[i], val);
        calibrate_analog_apply();

        /* The other axes are OK */
        rc_min[i] = 0;
        rc_max[i] = 0;
        return;
    }

    /* If recorded digital bounds get close to completely used,
    * we risk having to constantly adjust the analog gain. We
    * should be able to detect this happening as user rotates the
    * device. */
    if (rc_max[i] - rc_min[i] > BOUND_MAX && fixed_magnetometer_gain > 0) {
        fixed_magnetometer_gain -= 1;
        LOGI("Adjusting magnetometer gain to %d", fixed_magnetometer_gain);
        calibrate_analog_apply();

        /* Bounds will change on all axes. */
        for (int j = 0; j < 3; j ++) {
            rc_min[j] = 0;
            rc_max[j] = 0;
        }

        return;
    }

    rc_min[i] += CALIBRATE_DECAY;
    rc_max[i] -= CALIBRATE_DECAY;

    /* minimum value seen */
    if (rc_min[i] > val) {
        rc_min[i] = val;
    }

    /* maximum value seen */
    if (rc_max[i] < val) {
        rc_max[i] = val;
    }
}

void AK8973B::calibrate_magnetometer_analog()
{
    calibrate_magnetometer_analog_helper(m.x, 0);
    calibrate_magnetometer_analog_helper(m.y, 1);
    calibrate_magnetometer_analog_helper(m.z, 2);    

    /* Apply 16-bit digital gain factor to scale 8->12 bits. */
    m = m.multiply(digital_gain);
}

void AK8973B::calibrate()
{
    const int REFRESH = 1;
    magnetometer.update(next_update.tv_sec, m);
    if (magnetometer.fit_time <= next_update.tv_sec - REFRESH) {
        magnetometer.try_fit(next_update.tv_sec);
    }
    /* Correct for scale and offset. */
    m = m.add(magnetometer.center.multiply(-1));
    m = m.multiply(magnetometer.scale);
    //LOGD("mf x=%d y=%d z=%d",(int)m.x,(int)m.y,(int)m.z);
}

int AK8973B::get_delay() {
    unsigned short delay;
    SUCCEED(ioctl(fd, ECS_IOCTL_GET_DELAY, &delay) == 0);
    return delay;
}

void AK8973B::measure() {
    SUCCEED(gettimeofday(&next_update, NULL) == 0);

    /* Measuring puts readable state to 0. It is going to take
    * some time before the values are ready. Not using SET_MODE
    * because it contains mdelay(1) which makes measurements spin CPU! */
    char akm_data[5] = { 2, AKECS_REG_MS1, AKECS_MODE_MEASURE, 0, 0};

    SUCCEED(ioctl(fd, ECS_IOCTL_WRITE, &akm_data) == 0);

    /* Sleep for 300 us, which is the measurement interval. */ 
    struct timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 300000;
    SUCCEED(nanosleep(&interval, NULL) == 0);
    
    akm_data[0] = 4;
    akm_data[1] = AKECS_REG_TMPS;
    akm_data[2] = 0;
    akm_data[3] = 0;
    akm_data[4] = 0;  
    SUCCEED(ioctl(fd, ECS_IOCTL_READ, &akm_data) == 0);

    temperature = 110 - (unsigned char)akm_data[1]*0.625;
    
    //because every 2nd measure iz 0 0 0
    if(akm_data[2]!=0 && akm_data[3]!=0 && akm_data[4]!=0 )
    m = mbuf = mbuf.multiply(0.5f).add( Vector(
                                     127 - (unsigned char)akm_data[2],
                                     127 - (unsigned char)akm_data[3],
                                     127 - (unsigned char)akm_data[4]
                                     ).multiply(0.5f));
    else m = mbuf;
    
    //LOGD("mf x=%d y=%d z=%d",(int)mbuf.x,(int)mbuf.y,(int)mbuf.z);
    calibrate_magnetometer_analog();
    calibrate();
}

Vector AK8973B::read()
{
    return m;
}

void AK8973B::publish(short* data)
{
    SUCCEED(ioctl(fd, ECS_IOCTL_SET_YPR, data) == 0);
}

int AK8973B::get_temperature()
{
    return temperature;
}

void AK8973B::start()
{
}

void AK8973B::stop()
{
}

void AK8973B::wait_start()
{
    /* When open, we enable BMA and wait for close event. */
    int status;
    SUCCEED(ioctl(fd, ECS_IOCTL_GET_OPEN_STATUS, &status) == 0);
}

void AK8973B::wait_stop()
{
    /* When open, we enable BMA and wait for close event. */
    int status;
    SUCCEED(ioctl(fd, ECS_IOCTL_GET_CLOSE_STATUS, &status) == 0);
}

}
