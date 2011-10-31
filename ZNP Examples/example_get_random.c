/**
* @ingroup znpInterface
* @{ 
*
* @file example_get_znp_random.c
*
* @brief Resets Radio, gets a random number using SYS_RANDOM command and displays it. 
* 
* Configures the MSP430 to communicate with the radio, resets the radio, and sends the 
* SYS_RANDOM command and parses the received response and displays it to the console unless there was an error. 
* Demonstrates basic request-response functionality of the ZNP.
*
* @see section "Random-Number Generator" of CC253x User's Guide for more information.
*
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
    halInit();
    printf("\r\nResetting Radio, then getting Random Number\r\n");
    znpInit();   
    while (1)
    {
        unsigned int num = getRandom();
        if (znpResult == ZNP_SUCCESS)
            printf("Random Number = %u (%04X)\r\n", num, num);
        else
            printf("ERROR %i\r\n", znpResult);  
        delayMs(1000);
    }
}

/* @} */