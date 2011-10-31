/**
* @ingroup znpInterface
* @{ 
*
* @file example_write_nv_items.c
*
* @brief Resets Radio, prompts the user for which NV item to write (1-6) and writes test data to that location.
*
* The ZNP supports six NV storage buffers (named "NV Items"). 1-4 are 2 bytes each; 5 & 6 are 16 bytes each.
* These NV items are stored in the ZNP's flash memory and can be used for non-volatile storage. 
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

#define NO_CHARACTER_RECEIVED 0xFF
unsigned char getWhichNvItemToWrite();
char command;

#define TEST_DATA {0x53, 0x6D, 0x69, 0x74, 0x68, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x6E, 0x69, 0x63, 0x73}

unsigned char dataToWrite[16] = TEST_DATA;

/** function pointer (in hal file) for the function that gets called when a byte is received on the debug console. */
extern void (*debugConsoleIsr)(char); 

void handleDebugConsoleInterrupt(char rxByte);

int main( void )
{
    halInit();                          //Initialize hardware
    printf("\r\nWriting NV Items\r\n");
    znpInit();    
    debugConsoleIsr = &handleDebugConsoleInterrupt;   //call method handleDebugConsoleInterrupt() when a byte is received    
    HAL_ENABLE_INTERRUPTS();              //Enable Interrupts
    
    while (1)
    {
    unsigned char whichNvItem = getWhichNvItemToWrite();  
    if (whichNvItem != NO_CHARACTER_RECEIVED)
    {
        unsigned char nvItemSize = getNvItemSize(whichNvItem);
        printf("\r\nWriting to NV item %u, L%u:", whichNvItem, nvItemSize);    
        printHexBytes(dataToWrite, nvItemSize);
        writeNvItem(whichNvItem, dataToWrite);
        if (znpResult == ZNP_SUCCESS)
            printf("Success\r\n");
        else
            printf("ERROR %i\r\n", znpResult);   
    }
}
}

/** @return which NV item the user selected (1-6), or NO_CHARACTER_RECEIVED if none. */
unsigned char getWhichNvItemToWrite()
{
    command = NO_CHARACTER_RECEIVED;     
    printf("Which NV Item to write to? (1-6)");
    while (command == NO_CHARACTER_RECEIVED) ;  //wait until user types a character
    //while ((command < '1') || (command > '6')) ; //wait until user types a valid character
    if ((command < '1') || (command > '6')) 
    {
        printf("Bad Input; skipping.\r\n"); 
        return NO_CHARACTER_RECEIVED; 
    }
    return(command - '0');
}

/** Method to handle bytes received on the debug console.
This gets called by the ISR in the hal file since we set the debugConsoleIsr function pointer (in hal file) to point to this function.
*/
void handleDebugConsoleInterrupt(char rxByte)
{
    command = rxByte;
}
