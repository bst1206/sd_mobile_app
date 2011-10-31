/**
* @ingroup hwExamples
* @{ 
* @file example_read_light_sensor.c
*
* @brief Uses the MSP430's internal Analog to Digital Converter (ADC) to read the ambient light sensor value
* and supply voltage and outputs to console approximately once per second.
*
* Uses the Hardware Abstraction Layer (hal).
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 587 $
* $Author: dsmith $
* $Date: 2010-06-11 10:17:52 -0700 (Fri, 11 Jun 2010) $
*
* YOU ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY 
* OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, 
* TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL TEXAS INSTRUMENTS 
* OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, 
* BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY 
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/

#include "../HAL/hal_cc2530znp_target_board.h"

int main( void )
{
    halInit();
    printf("Reading Vcc and Light Sensor\r\n");
    while (1) 
    {
        unsigned int vcc3 = getVcc3();
        unsigned int lightSensor = getLightSensor() * 10;
        printf("Vcc3 = %u mV, Light Sensor = %u lux\r\n", vcc3, lightSensor);
        toggleLed(1);         
        delayMs(1000);          
    }
}

/* @} */
