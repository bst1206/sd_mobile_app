/**
* @ingroup znpInterface
* @{ 
*
* @file example_get_mac_address.c
*
* @brief Resets Radio, gets MAC Address
* 
* Configures the MSP430 to communicate with the radio, resets the radio, and uses the ZB_GET_DEVICE_INFO to get the MAC address.
*
* ZB_DEVICE_INFO can be used to obtain valuable information about the operation of ZNP. Some fields 
* like DIP_MAC_ADDRESS are valid all the time but the Zigbee network information like DIP_SHORT_ADDRESS 
* is only valid once the ZNP has joined a network successfully. These fields can also be handy for debugging.
* When troubleshooting a connection problem, read DIP_STATE as it will give you an indication of what 
* the ZNP is doing and whether it is on a network.
*
* @note throughout the ZNP, data is always sent/received  with least significant byte first. 
* For example, if sending data 0x01020304 then you would send byte 0x04 first, then 0x03, then 0x02, finally 0x01.
* @note Although the ZB_GET_DEVICE_INFO command is under the "Simple API" group, this can be used at any time, even with using AF/ZDO interface.
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
    printf("\r\nResetting Radio, then getting MAC Address\r\n");
    znpInit();  
    while (1)
    {
        unsigned char* mac = getMacAddress();
        if (znpResult == ZNP_SUCCESS)
        {
            printf("MAC:");
            printHexBytes(mac, 8);
        }
        else
            printf("ERROR %i\r\n", znpResult); 
        delayMs(1000);
    }
}

/* @} */