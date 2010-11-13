/*
 * Copyright Antti S. Lankila, 2010, Licensed under GPL.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <utils/Log.h>

#include "Calibrator.hpp"
#include "math/Matrix.hpp"
#include "util.hpp"

namespace akmd {    

Calibrator::Calibrator(int validity)
{
    this->validity = validity;
}

void Calibrator::reset(bool loadmag)
{
    calibrated = false;
    readingfromfile = false;
    if(loadmag) {
        readingfromfile = true;
        /* Check if calibration data file exsists. */
        FILE *fad = fopen("/data/misc/akmd.dat","rb");
        if(fad != NULL) {
            fad_exsists = true;
            LOGI("compass calibration file exsist");
        }
        else {
            fad_exsists = false;
            LOGI("compass calibration file doesnt exsist");
        }
        if(fad_exsists) {
            float akmd_data[139] = {0,};
            /* Read recorded calibration data from file. */
            fread(akmd_data, sizeof(float), 139, fad);
            center = Vector(akmd_data[0], akmd_data[1], akmd_data[2]);
            scale  = Vector(akmd_data[3], akmd_data[4], akmd_data[5]);
            old_nv = Vector(akmd_data[6], akmd_data[7], akmd_data[8]);
            minimum_points_needed = akmd_data[9];
            idx = akmd_data[10];
            memset(point_cloud, 0, sizeof(point_cloud));
            for(int j=0;j<32;j++) {
                    point_cloud[j].v.x = akmd_data[11+(j*4)];
                    point_cloud[j].v.y = akmd_data[12+(j*4)];
                    point_cloud[j].v.z = akmd_data[13+(j*4)];
                    point_cloud[j].time = (int)akmd_data[14+(j*4)];
            }
            fclose(fad);
            calibrated = true;
            LOGI("compass calibration data obtained from a file");
	    LOGI("compass calibrated to offset=(%f %f %f) scale=(%f %f %f)", center.x, center.y, center.z, scale.x, scale.y, scale.z);
        }
        else {
            /* There is no saved calibration data and there is a need to start over again. */
            center = Vector(0, 0, 0);
            scale  = Vector(1, 1, 1);
            old_nv = Vector(1, 0, 0);
            minimum_points_needed = PCR/4;
            idx = 0;
            memset(point_cloud, 0, sizeof(point_cloud));
            LOGI("reset of compass calibration data");
        }        
    }
    else {
        center = Vector(0, 0, 0);
        scale  = Vector(1, 1, 1);
        old_nv = Vector(1, 0, 0);
        minimum_points_needed = PCR/4;
        idx = 0;
        memset(point_cloud, 0, sizeof(point_cloud));
        LOGI("reset of calibration data");
    }
    fit_time = 0;
}

Calibrator::~Calibrator()
{
}

void Calibrator::update(int time, Vector v)
{
    const float SIMILARITY = 0.8f; /* 36 degrees' deviation, 10 vectors per circle */

    float vl = v.length();
    if (vl == 0.0f) {
        return;
    }

    Vector nv = v.divide(vl);

    /* Require sampled vectors to point to fairly different directions
     * before accepting another. */
    float similarity = nv.dot(old_nv);
    if (similarity > SIMILARITY) {
        return;
    }
    old_nv = nv;

    /* Check if we already have a vector nearly to same direction,
     * if so replace that one. This helps in not destroying our
     * history of vectors if user just jiggles device back and forth. */
    for (int i = 0; i < PCR; i ++) {
        if (point_cloud[i].time < time - validity) {
            idx = i;
            break;
        }

        Vector c = point_cloud[idx].v;
        Vector nc = c.divide(c.length());

        float similarity = nv.dot(nc);
        if (similarity > SIMILARITY) {
            idx = i;
            break;
        }

    }

    point_cloud[idx].time = time;
    point_cloud[idx].v = v;
    /* Round-robin vector reuse */
    idx = (idx + 1) & (PCR - 1);
}

bool Calibrator::try_fit(int time)
{
    int n = 0;
    for (int i = 0; i < PCR; i ++) {
        if (point_cloud[i].time >= time - validity) {
            n ++;
        }
    }

    /* Less than required bins filled with recent data? */
    if (n < minimum_points_needed) {
        return false;
    }

    /* Progressively require more and more points until saturated (at 50 %) */
    if (n < PCR/2) {
        minimum_points_needed = n;
    } else {
        minimum_points_needed = PCR/2;
    }

    fit_time = time;

    Matrix a = Matrix(n, 6);
    Matrix b = Matrix(n, 1);

    n = 0;
    for (int i = 0; i < PCR; i ++) {
        if (point_cloud[i].time < time - validity) {
            continue;
        }

        Vector v = point_cloud[i].v;

        b.set(n, 0, -v.x*v.x);
        a.set(n, 0, -2.0f*v.x);
        a.set(n, 1, v.y*v.y);
        a.set(n, 2, -2.0f*v.y);
        a.set(n, 3, v.z*v.z);
        a.set(n, 4, -2.0f*v.z);
        a.set(n, 5, 1.0f);

        n ++;
    }

    float *x = Matrix::leastSquares(&a, &b);
    if (x != NULL) {
        /* Do a little smell test on the values. */
        if (x[1] > 0.5f && x[1] < 2.0f && x[3] > 0.5f && x[3] < 2.0f) {
            center = Vector(x[0], x[2] / x[1], x[4] / x[3]);
            scale = Vector(1, sqrtf(x[1]), sqrtf(x[3]));
        }
        delete[] x;
    }

    /* Save calibration data to a file. */
    if(!calibrated && readingfromfile && center.x != 0 && center.y != 0 && center.z != 0) {
        if(!fad_exsists) {
            float calibration_data[139] =  {center.x, center.y, center.z,
                                            scale.x, scale.y, scale.z,
                                            old_nv.x, old_nv.y, old_nv.z,
                                            (float)minimum_points_needed, (float)idx, 0, };
            for(int i=0;i<32;i++) {
                calibration_data[11+(i*4)] = point_cloud[i].v.x;
                calibration_data[12+(i*4)] = point_cloud[i].v.y;
                calibration_data[13+(i*4)] = point_cloud[i].v.z;
                calibration_data[14+(i*4)] = (float)point_cloud[i].time;
            }
            FILE *fac = fopen("/data/misc/akmd.dat","wb");
            if(fac != NULL) {
                // sizeof(calibration_data) = 556 which is 139 elements of float
                fwrite(calibration_data, sizeof(float), 139, fac);
            }
            fclose(fac);
            calibrated = true;
            LOGI("compass calibrated to offset=(%f %f %f) scale=(%f %f %f)", center.x, center.y, center.z, scale.x, scale.y, scale.z);
        }
        calibrated = true;
    }    

    return true;
}

}
