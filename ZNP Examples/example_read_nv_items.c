/**
* @ingroup znpInterface
* @{ 
*
* @file example_read_nv_items.c
*
* @brief Resets Radio, reads out the contents of all non-volatile (NV) items and displays to console.
*
* The ZNP supports six NV storage buffers (named "NV Items"). 1 through 4 are 2 bytes each; 5 & 6 are 16 bytes each.
* These NV items are stored in the ZNP's flash memory and can be used for non-volatile storage. 
* To verify, run the Write NV Items example, power cycle the CC2530, and then run this example. You should see that the values are retained through loss of power.
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
    printf("NV Items:\r\n");
    znpInit();  
    while (1)
    {
                    printf("Reading NV Items from ZNP:\r\n");
        for (unsigned int itemNum = MIN_NV_ITEM; itemNum < (MAX_NV_ITEM+1); itemNum++)   //iterate through all NV items
        {
            unsigned char* buf = readNvItem(itemNum);
            if (znpResult == ZNP_SUCCESS)
            {
                printf("NV Item %u:", itemNum);
                for (int i=0; i<getNvItemSize(itemNum); i++)
                    printf("%02X(%c)", buf[i], buf[i]);
                printf("\r\n");
            }
            else
            {
                printf("ERROR %i\r\n", znpResult); 
            }
        }
        printf("\r\n");
        delayMs(1000);
    }
}

/* @} */