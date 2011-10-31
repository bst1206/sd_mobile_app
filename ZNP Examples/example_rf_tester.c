/**
* @ingroup znpInterface
* @{
*
* @file example_rf_tester.c
*
* @brief Simple utility to configure the ZNP to output an RF test signal.
*
* Configures ZNP interface, resets radio, and then processes typed commands.
* The ZNP can output either an RF carrier signal, or random modulated data. 
* The user can change which test mode to use, channel (11-26), and turn off the transmitter.
* This utility can be used for radiated emissions compliance testing or tuning the RF circuit.
* 
* The Utility can be controlled through the simple command-line menu. See printUsage() for available commands.
* If, instead, you would like it to output one particular mode on startup, change <code>rfTestMode</code> and <code>rfTestChannel</code> accordingly.
*
* @note if ZNP_INTERFACE_VERBOSE is defined, this will display an error because the SYS_TEST_RF method does not return an SRSP. This error can be ignored.
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
#include "../HAL/hal.h"
#include "../ZNP/znp_interface.h"
#include "../ZNP/znpRfTestInterface.h"

extern signed int znpResult;

#define HARDWARE_UART
#define NO_CHARACTER_RECEIVED 0xFF

/** Which RF Test Mode we're in. Will be changed by the commands. 
Change this if you want the application to start in a different mode. */
unsigned char rfTestMode = RF_TEST_UNMODULATED;

/** The current channel that the RF test mode is set to. 
Stored so that we can increment/decrement this channel easily. 
Change this if you want the application to start on a different channel. */
unsigned char rfTestChannel = RF_TEST_CHANNEL_MIN;

/** the command that was entered by the user */
char command = NO_CHARACTER_RECEIVED;

/** function pointer (in hal file) for the function that gets called when a byte is received on the debug console. */
extern void (*debugConsoleIsr)(char);  

/** processes bytes received on the debug console */
void handleDebugConsoleInterrupt(char rxByte);

void printUsage()
{
    printf("Commands:\r\n");    
    printf("    ]   Increment RF Test Channel\r\n");
    printf("    [   Increment RF Test Channel\r\n");
    printf("    M   Change RF Test Mode to modulated output\r\n");
    printf("    U   Change RF Test Mode to UNmodulated output\r\n");    
    printf("    R   Change RF Test Mode to receive\r\n");
    printf("    !   Reset Radio\r\n");   
    printf("    ?   Print this menu\r\n");    
}

int main( void )
{
    halInit();                        
    printf("\r\nRF Tester\r\n");
    znpInit(); 
    HAL_ENABLE_INTERRUPTS();            
    printUsage();
    debugConsoleIsr = &handleDebugConsoleInterrupt;   //call method handleDebugConsoleInterrupt() when a byte is received

    while (1) 
    {
        if (command != NO_CHARACTER_RECEIVED)
        {
            switch (command)    //based on what command was entered, set the RF test mode and/or channel accordingly
            {
            case ']':
                printf("Increment RF Test Channel\r\n");
                if (rfTestChannel == RF_TEST_CHANNEL_MAX)
                {
                    printf("Error - already at the highest channel, %u\r\n", RF_TEST_CHANNEL_MAX);
                    command = NO_CHARACTER_RECEIVED;
                    continue;
                }
                rfTestChannel++;
                break;        
            case '[':
                printf("Decrement RF Test Channel\r\n");
                if (rfTestChannel == RF_TEST_CHANNEL_MIN)
                {
                    printf("Error - already at the lowest channel, %u\r\n", RF_TEST_CHANNEL_MIN);
                    command = NO_CHARACTER_RECEIVED;
                    continue;
                }
                rfTestChannel--;         
                break;
            case 'M':
                printf("Changing RF Test Mode to modulated output\r\n");
                rfTestMode = RF_TEST_MODULATED;
                break;
            case 'U':
                printf("Changing RF Test Mode to UNmodulated output\r\n");
                rfTestMode = RF_TEST_UNMODULATED;
                break;    
            case 'R':
                printf("Changing RF Test Mode to receive\r\n");
                rfTestMode = RF_TEST_RECEIVE;
                break;  
            case '!':
                printf("Resetting ZNP\r\n");                
                znpReset();
                command = NO_CHARACTER_RECEIVED;
                continue;   
            case '?':
                printUsage();
                break;
            default:
                printf("Unknown Command\r\n");
                command = NO_CHARACTER_RECEIVED;
                continue;  //return; don't change anything
            }
            // Reset the ZNP to exit test mode before executing next command
            znpReset();
            //now, set the RF test mode and channel based on what was entered
            signed int result = rfTest(rfTestMode, rfTestChannel);
            if (result > -10)  //ignore SRSP checking errors since this message has no SRSP
                printf("ERROR %i\r\n", result);
            else
                printf("Success\r\n"); 
            // Reset the ZNP to enter test mode
//            znpSoftReset();
            command = NO_CHARACTER_RECEIVED;
        }
    }
}

/** Method to handle bytes received on the debug console. Just stores it to the variable <code>command</code>.
This gets called by the ISR in the hal file since we set the debugConsoleIsr function pointer (in hal file) to point to this function.
*/
void handleDebugConsoleInterrupt(char rxByte)
{
    command = rxByte;
}



/* @} */