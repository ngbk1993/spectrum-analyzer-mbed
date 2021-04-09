/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <arm_math.h>


float32_t buffer_input[l_probek];
uint16_t i;
uint8_t mode;
float32_t dt;
float32_t freq;
bool DoFlag = false;
bool UBFlag = false;
uint32_t rozmiar = 4*l_probek;

union
{
    float32_t f[l_probek];
    uint8_t b[4*l_probek];
}data_out;


union
{
    float32_t f[l_probek];
    uint8_t b[4*l_probek];
}data_mag;

union
{
    float32_t f;
    uint8_t b[4];
}czest_rozdz;


/* Pointers ------------------------------------------------------------------*/
arm_rfft_fast_instance_f32 S;
arm_cfft_radix4_instance_f32 S_CFFT;
uint16_t output;
/* ---------------------------------------------------------------------------*/
int main(void)
{
    freq = 5000;
    dt = 0.000000390625;


    arm_rfft_fast_init_f32(&S, l_probek);

    mode = 2;


    //----------------- Infinite loop
  while (1)
    {
        if(true)//(UBFlag == true)

                    for(i=0; i<l_probek; ++i)
                    {
                        buffer_input[i] = (float32_t) 15*sin(2*PI*freq*i*dt);
                    }

            //Obliczanie FFT
            arm_rfft_fast_f32(&S, buffer_input, data_out.f, 0);
            //Obliczanie modulow
            arm_cmplx_mag_f32(data_out.f, data_mag.f, l_probek);


        }

    }

