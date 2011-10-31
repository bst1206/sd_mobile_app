/**
* @ingroup znpInterface
* @{ 
*
* @file example_read_radio_GPIO_pins.c
*
* @brief Resets Radio, configures the GPIO pins as inputs, then reads out the value 
* of these pins to console once per second, between 0x00 (all low) to 0x0F (all high).
* To test, start this application then connect one or more GPIO pins to GND. 
* You will see the value change accordingly.
*
* The CC2530ZNP supports four General Purpose Input/Output (GPIO) pins. Pins can be individually configured as:
* - Inputs
* - Outputs
* - Tri-State
* When configured as an input, the pin will have either a pull-up resistor or pull-down resistor.
*
* @note The GPIO pins are not exposed on the CC2530ZNP target board
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
#include "../HAL/hal.h"
#include "../ZNP/znp_interface.h"

extern signed int znpResult;

int main( void )
{
    halInit();                          
    znpInit();   
    printf("Configuring Radio GPIO pins as inputs with pull-ups\r\n");
    sysGpio(GPIO_SET_INPUT_MODE , GPIO_INPUT_MODE_ALL_PULL_UPS);
    sysGpio(GPIO_SET_DIRECTION , GPIO_DIRECTION_ALL_INPUTS);
    
    while (1) 
    {
        printf("Reading ZNP GPIO pins (0-3)...");
        unsigned char val = sysGpio(GPIO_READ , ALL_GPIO_PINS);
        if (znpResult == ZNP_SUCCESS)
            printf("GPIO Pins = 0x%02X\r\n\r\n", val);
        else
            printf("ERROR %i\r\n", znpResult); 
        delayMs(1000);          
    }
}


/* @} */