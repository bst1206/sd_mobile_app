/**
* @file znp_example_utils.c
*
* @brief Simple utilities for ZNP examples. 
* It is recommended that you modify or replace these in a "real" application.
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 594 $
* $Author: dsmith $
* $Date: 2010-06-14 13:04:31 -0700 (Mon, 14 Jun 2010) $
*
* YOU ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS?WITHOUT WARRANTY 
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
#include "../ZNP/af_zdo.h"              //for pollingAndFindDevice()

#include "../ZNP/znp_interface_spi.h"
#include "../HAL/hal_lcd.h"

extern unsigned char znpBuf[100];

extern signed int znpResult;                    //from znp_interface

void handleReturnValue()
{
    if (znpResult == ZNP_SUCCESS) 
    {
        printf("Success\r\n"); 
    }
    else 
    {
        printf("ERROR %i; stopping\r\n", znpResult);
        while(1);   
    }
}

/** Poll the ZNP for any messages and display them to the console.
@pre SRDY went LOW indicating data is ready
*/
void pollAndDisplay()
{
        spiPoll();
        if (znpBuf[SRSP_LENGTH_FIELD] > 0)
        {
            printf("\r\nRx: ");
            printHexBytes(znpBuf, (znpBuf[SRSP_LENGTH_FIELD] + SRSP_HEADER_SIZE));
            
            char* msg = (char*)&znpBuf[6];
            printf("DEBUG: %s" , msg);
            
            for(int i = 0 ; i < znpBuf[SRSP_LENGTH_FIELD]; ++i)
            {
              char cZnpBuf = (char)znpBuf[SRSP_HEADER_SIZE+i+6];
              printf("%c", cZnpBuf);
            }
            
            znpBuf[SRSP_LENGTH_FIELD] = 0;
            
            displayString(msg);
        } 
}

/** Blocking wait until a message is received. Exits if received message is a ZDO_STATE_CHANGE_IND
and the state matches what we want. Else loops. 
@param expectedState the deviceState we are expecting - DEV_ZB_COORD etc.*/
void waitForDeviceState(unsigned char expectedState)
{
    unsigned char state = 0xFF;
    while (state != expectedState)
    {
        while (SRDY_IS_HIGH());         
        pollAndDisplay();
        if (CONVERT_TO_INT(znpBuf[2], znpBuf[1]) == ZDO_STATE_CHANGE_IND)
            state = znpBuf[SRSP_PAYLOAD_START];
    }
}

/** Waits for SRDY to go low, indicating a message has arrived. Displays the msg to console. */
void displayReceivedMessages()
{
    while (1)
    {
        while (SRDY_IS_HIGH());         
        pollAndDisplay();
    }
}

//#define FIND_NWK_ADDRESS  //define to also find the long address too

/** Waits for SRDY to go low, indicating a message has arrived. Displays the msg to console, 
and find the device's MAC address using ZDO_IEEE_ADDR_REQ.
Then, if FIND_NWK_ADDRESS was defined, uses the received MAC address to find the device's short address.
The two should match.
@pre callbacks have been enabled with enableCallbacks() - otherwise you will never receive the ZDO_IEEE_ADDR_RSP message.
*/
void displayReceivedMessagesAndFindDevice()
{
    znpBuf[0] = 0;  
    unsigned int shortAddress;
    while (1) 
    {
        while (SRDY_IS_HIGH());         
        pollAndDisplay();
        if (CONVERT_TO_INT(znpBuf[2], znpBuf[1]) == AF_INCOMING_MSG)
        {
            shortAddress = CONVERT_TO_INT(znpBuf[7], znpBuf[8]);     
            zdoRequestIeeeAddress(shortAddress, SINGLE_DEVICE_RESPONSE, 0); 
            if (znpResult == 0)
            {
                printf("MAC Address (LSB first) of sender is :");
                printHexBytes(znpBuf+4, 8);
            } else {
                printf("IEEE Request Failed (%d)\r\n", znpResult);
                continue;
            }

#ifdef FIND_NWK_ADDRESS
            /** Very simple example of zdoNetworkAddressRequest() */
            unsigned char* response = zdoNetworkAddressRequest(znpBuf+4, SINGLE_DEVICE_RESPONSE, 0);  //was SINGLE_DEVICE_RESPONSE

            if (znpResult == 0)
            {
                unsigned int sa = CONVERT_TO_INT(znpBuf[12], znpBuf[13]);
                printf("Short Address = %04X\r\n", sa);
            } else {
                printf("NWK Request Failed (%d)\r\n", znpResult);
                continue;
            } 
            printZdoNetworkAddressResponse(response);
#endif            
        }
    }
}