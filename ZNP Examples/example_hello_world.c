/**
* @ingroup hwExamples
* @{ 
* @file example_hello_world.c
*
* @brief Outputs "Hello World" to console and toggles LED1 approximately once per second.
*
* Configures the debug console UART pins and LED1 pin accordingly; loops and outputs text.
*
* If you aren't seeing the message:
* - Verify that hardware is installed correctly. Open the Device Manager: Go to Start : Run... type in devmgmt.msc and press Return. 
*   Then click on the + symbol next to ports. Note the COM port number next to the ez430 board.
* - Verify that your terminal program is configured correctly. If using HyperTerminal, open the Device Manager: Go to Start : Run... type in hypertrm.exe and press Return. 
*   Verify that the COM port number is correct and the baud rate (9600, 8N1) and other configuration is correct. 
* - Verify that the application is running - verify that LED1 is blinking.
* - Verify that the application is outputting bytes on the serial port - use an oscilloscope to probe the MSP430's Tx pin and verify that the UART is working.
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
    halInit();                          //Initialize hardware
    unsigned char counter = 0;
    while (1) 
    {
        printf("Hello World %u\r\n", counter++);  
        toggleLed(1);         
        delayMs(1000);          
    }
}


/* @} */