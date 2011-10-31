/**
* @ingroup znpInterface
* @{ 
*
* @file example_write_radio_GPIO_pins.c
*
* @brief Resets Radio, configures the GPIO pins as outputs, then toggles the GPIO pins once per second.
* To test, monitor GPIO pins with an oscilloscope.
*
* The CC2530ZNP supports four General Purpose Input/Output (GPIO) pins. Pins can be individually configured as:
* - Inputs
* - Outputs
* - Tri-State
*
* When configured as an output, the pin can be set, cleared, or toggled.
*
* @note The GPIO pins are not exposed on the CC2530ZNP target board
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 717 $
* $Author: dsmith $
* $Date: 2010-08-19 10:50:46 -0700 (Thu, 19 Aug 2010) $
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
    printf("Configuring Radio GPIO pins as outputs\r\n");   
    sysGpio(GPIO_SET_DIRECTION , ALL_GPIO_PINS);
    
    while (1) 
    {
        printf("Setting GPIO pins to high ...\r\n");
        sysGpio(GPIO_SET , ALL_GPIO_PINS);
        toggleLed(1);
        if (znpResult != ZNP_SUCCESS)
            printf("ERROR %i\r\n", znpResult);
        delayMs(1000);    
        
        printf("Setting GPIO pins to low ...\r\n");
        sysGpio(GPIO_CLEAR , ALL_GPIO_PINS);
        toggleLed(1);
        if (znpResult != ZNP_SUCCESS)
            printf("ERROR %i\r\n", znpResult);
        delayMs(1000); 
    }
}

/* @} */