/**
* @ingroup znpInterface
* @{ 
*
* @file example_reset_radio.c
*
* @brief Resets Radio, parses message received and displays to console.
*
* Configures the MSP430 to communicate with the radio, asserts the radio RESET line, and then receives the SYS_RESET_IND message.
* 
* If this isn't working correctly:
* - Verify that the CC2530 is programmed correctly with the ZNP firmware image.
* - Verify that the CC2530 is running code by using an oscilloscope to probe the crystal. If the crystal is running then you should see a 32MHz sine wave, amplitude 400mV, DC offset of 400mV.
* - Verify that CFG pins are set correctly.
* - Verify that nothing else is connected to the RESET line, like an external programmer or debugger.
* - Run the Hello World application to verify that the console is working.
* - If all else fails, attach a logic anayzer and look at the traffic to/from the ZNP. Compare with example traces.
*
* @note The ZNP can be reset through hardware (the RESET pin) or software (SYS_RESET_REQ). 
* It is highly recommended to use the hardware reset instead of the software reset.
* @note This application is the foundation of all radio examples. If porting these examples to another processor, 
* get this example working before attempting any of the other radio examples.
* @note to see the version information, ZNP_INTERFACE_VERBOSE must be defined. The preferred way is 
* to define this on a per-project basis (in Project Options : C/C++ compiler : Preprocessor : Defined Symbols).
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 718 $
* $Author: dsmith $
* $Date: 2010-08-19 10:51:40 -0700 (Thu, 19 Aug 2010) $
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
    halInit();                          //Initialize hardware
    printf("\r\nResetting ZNP\r\n"); 
    while (1)
    {
        unsigned char* v = znpInit(); 
        if (znpResult == ZNP_SUCCESS)
            printf("ZNP %s Reset (%u), TransportRev=%u, ProductId=%u, FW Rev=%u.%u, HW Rev=%u\r\n",
                   getResetReason(v[0]), v[0], v[1], v[2], v[3], v[4], v[5]);
        else
            printf("ERROR %i\r\n", znpResult);   
        delayMs(1000);
    }
}


/* @} */