#pragma once

#include <sys/time.h>

#include "Calibrator.hpp"
#include "device/ChipReader.hpp"
#include "device/DataPublisher.hpp"
#include "device/TemperatureReader.hpp"
#include "util.hpp"
#include "math/Vector.hpp"

namespace akmd {

class AK8973B : public ChipReader, public TemperatureReader, public DataPublisher
{
    private:

    /* Temperature is -(value-zero). */
    char temperature_zero;
    /* The analog offset */
    signed char analog_offset[3];
    /* The user requested magnetometer gain */
    int magnetometer_gain;
    /* The actual gain used on hardware */
    int fixed_magnetometer_gain;
    /* Digital gain to compensate for analog setting. */
    float digital_gain;

    /* Misc. measurement data */
    struct timeval next_update;
    int fd;
    short temperature;

    Vector mbuf;

    Vector m;

    float rc_min[3];
    float rc_max[3];

    Calibrator magnetometer;

    char akm_analog_offset(int i);
    void calibrate_analog_apply();
    void calibrate_magnetometer_analog_helper(float val, int i);
    void calibrate_magnetometer_analog();
    void calibrate();

    public:
    AK8973B(int dgain,int again);
    ~AK8973B();

    int get_delay();
    void measure();
    Vector read();

    void start();
    void stop();
   
    /* AKM8973-specific addendums. */
    void publish(short *data);
    int get_temperature();

    void wait_start();
    void wait_stop();
};

}
