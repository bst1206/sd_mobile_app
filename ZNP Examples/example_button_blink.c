/**
* @ingroup hwExamples
* @{
*
* @file example_button_blink.c
* @brief Toggles LED0 when button pressed.
* 
* Configures pin controlling LED0 and LED1 to be an output. Configures pin attached to button as an input with interrupt. 
* Once per second toggles LED0 as a simple indication that application is running.
* When interrupt generated, waits to allow button to debounce then if button pressed, toggles LED0.
*
* This example is a good way to verify that the development environment is configured correctly.
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 602 $
* $Author: dsmith $
* $Date: 2010-06-16 13:09:46 -0700 (Wed, 16 Jun 2010) $
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

/** function pointer (in hal file) for the function that gets called when a button is pressed*/
extern void (*buttonIsr)(void);

/** Our button interrupt handler */
void handleButtonPress(void);

int main( void )
{
    halInit();                          //Initialize hardware
    buttonIsr = &handleButtonPress;
    HAL_ENABLE_INTERRUPTS();              //Enable Interrupts
    while (1) 
    {
        toggleLed(1);         
        delayMs(1000);          
    }
}

/** Button interrupt service routine. Called when interrupt generated on the button.
@pre Button connects input to GND.
@pre pins are configured as interrupts appropriately and have pull-UP resistors.
*/
void handleButtonPress(void)
{
    toggleLed(0);
}


/* @} */