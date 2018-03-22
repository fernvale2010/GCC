/*
 *  File:     main.c
 *  Purpose:  RC Transmitter with STM32F103C8T6 and NRF24L01+
 *
 *  Date:     05 July 2013
 *  Info:     If __NO_SYSTEM_INIT is defined in the Build options,
 *            the startup code will not branch to SystemInit()
 *            and the function can be removed
 ************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "avr221-pid.h"

struct PID_DATA pidData;


uint32_t syscnt = 0;

// simulate adc reading..
#define ADC_SIZE     (sizeof(adc_samples)/sizeof(uint16_t))
uint16_t adc_idx = 0;
uint16_t adc_samples[] =
{
      0, 101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1220, 1240, 1250, 1250,
/*      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250,
      1250, 1250, 1350, 1300, 1280, 1270, 1250, 1250, 1240, 1230, 1250, 1250, 1250, 1250*/
};


/*
 * Delay must not be > 0x80000000L..
 */
int IsDelayOver(uint32_t start, uint32_t delay)
{
   uint32_t cur = syscnt;

   if ((cur - (start + delay)) < 0x80000000L)
      return 1; // delay is over
   else
      return -1; // not yet..
}


void DelayMSec(uint32_t msec)
{
   uint32_t curtick = syscnt;

   while(IsDelayOver(curtick, msec) < 0); // wait ms
}

uint16_t ADC_GetConvertedValue(void)
{
   uint16_t res;
   
   res = adc_samples[adc_idx++];
   return res;
}


/*********************************************************************
 *
 *  main()
 *
 *********************************************************************/
int main(void)
{
   /******************************************************************
    *
    *
    ******************************************************************/
   uint32_t starttime;
   int32_t adcValue;
   int32_t temp_pwm;
   uint16_t temp1;
   int16_t current_pwm;

   int16_t p_factor = 0.5 * SCALING_FACTOR;
   int16_t i_factor = 0 * SCALING_FACTOR;
   int16_t d_factor = 0 * SCALING_FACTOR;
   int16_t setPoint = 1250; // in mV. This is at voltage divider (30k+10k) corresponding to 5v.
   int16_t newValue;
   int16_t processValue;

   // Do some stuff so that ADC will have a value ready..
   memset(&pidData, 0, sizeof(pidData));
   pid_Init(p_factor, i_factor, d_factor, &pidData);
   
   adc_idx = 0;
   current_pwm = 0; // InitPWM sets timer to 0% pwm..
   adcValue = -1;
   printf("MAX_INT=%d\r\n", MAX_INT);
   
   for (; adc_idx < ADC_SIZE;)
   {
      starttime = syscnt;  // start of pid loop time
      adcValue = ADC_GetConvertedValue();  // in mV
      if (adcValue >= 0)
      {
         processValue = (int16_t)adcValue;
         newValue = pid_Controller(setPoint, processValue, &pidData);
                    // do PID, newValue can be +ve or -ve..
                    // newValue is scaled by SCALING_FACTOR
                    // so, if pid_Controller() returns MAX_INT, which is 32767
                    // then newValue will be 32767/SCALING_FACTOR = 255

         temp_pwm = newValue;
         if (temp_pwm < 0) // -ve value, limit to 0
         {
            temp_pwm = 0;
         }
         else if (temp_pwm > (MAX_INT/SCALING_FACTOR)) // exceeded 16 bits, limit to MAX_INT
         {
            // scale to (0% - 100%)
            temp_pwm = (MAX_INT/SCALING_FACTOR);
         }

         current_pwm = temp_pwm;

         // scale to percentage
         //current_pwm = (temp_pwm * 100)/255;
         temp1 = (uint16_t)current_pwm;
         
         printf("%d: adc=%d, newValue=%d, current_pwm=%d\r\n", adc_idx, adcValue, newValue, current_pwm);
         //printf("Integral error=%d\r\n", pidData.sumError);
      }
   }

   return 0;
}





