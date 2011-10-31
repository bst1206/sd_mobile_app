/**
* @ingroup apps
* @{ 
*
* @file example_simple_application_coordinator_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee Coordinator, and displays any messages that are received.
* Uses the AF/ZDO interface.
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 845 $
* $Author: dsmith $
* $Date: 2010-11-09 14:40:42 -0800 (Tue, 09 Nov 2010) $
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
#include "../ZNP/application_configuration.h"
#include "../ZNP/af_zdo.h"
#include "../ZNP/znp_interface_spi.h"
#include "Messages/infoMessage.h"
#include "znp_simple_app_utils.h" 
#include "znp_example_utils.h"

signed int returnValue;

void displayMessages();

void waitForDeviceState(unsigned char expectedState);

extern unsigned char znpBuf[100];

//uncomment below to see more information about the messages received.
//#define VERBOSE_MESSAGE_DISPLAY

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");
    printf("Simple Application Example - COORDINATOR - using AFZDO\r\n");
    HAL_ENABLE_INTERRUPTS();
    startZnp(COORDINATOR);

    printf("On Network!\r\n");
    
    /* On network, display info about this network */   
    getNetworkConfigurationParameters();                
    getDeviceInformation();
    
    /* Now the network is running - wait for any received messages from the ZNP */
#ifdef VERBOSE_MESSAGE_DISPLAY    
        printAfIncomingMsgHeaderNames();
#endif
        while (1)
    {
        while (SRDY_IS_HIGH());      //wait until SRDY goes low indicating a message has been received.   
        displayMessages();
    }
    
}

#define IS_INFO_MESSAGE_CLUSTER()  (CONVERT_TO_INT(znpBuf[AF_INCOMING_MESSAGE_CLUSTER_LSB_FIELD],znpBuf[AF_INCOMING_MESSAGE_CLUSTER_MSB_FIELD]) == INFO_MESSAGE_CLUSTER)

void displayMessages()
{
    spiPoll();
    if (znpBuf[SRSP_LENGTH_FIELD] > 0)
    {
        if (IS_AF_INCOMING_MESSAGE())
        {
            setLed(1);  //LED will blink to indicate a message was received
#ifdef VERBOSE_MESSAGE_DISPLAY               
            printAfIncomingMsgHeader(znpBuf);
            printf("\r\n");
#endif
            if (IS_INFO_MESSAGE_CLUSTER())
            {
                struct infoMessage im = deserializeInfoMessage(znpBuf+20);
                
#ifdef VERBOSE_MESSAGE_DISPLAY                
                printInfoMessage(&im);
#else
                printf("From:");
                for (int j = 0; j<8; j++)
                {
                    printf("%02X", im.header->mac[j]);
                }
                printf(",LQI=%02X", znpBuf[SRSP_HEADER_SIZE+9]);
#endif

                if (im.numParameters > 0)
                {
                    printf(";PanID:%04X", im.parameters[0]);
                    if (im.numParameters == 3)   //panId, vcc3v, light sensor (lux/10)
                    {
                        printf(",Vcc3=%umV,Light Sensor(lux)=%u,Cause=", im.parameters[1], im.parameters[2] * 10); 
                        switch (im.cause)
                        {
                        case CAUSE_SCHEDULED: printf("Timer"); break;
                        case CAUSE_MOTION: printf("Motion"); break;
                        case CAUSE_STARTUP: printf("Startup"); break;
                        }
                    }
                    printf("\r\n");
                }
            } else {
                printf("Rx: ");
                printHexBytes(znpBuf+SRSP_HEADER_SIZE+17, znpBuf[SRSP_HEADER_SIZE+16]);   //print out message payload
            }
            clearLeds();    
        } else { //unknown message, just print out the whole thing
            printf("??: ");
            printHexBytes(znpBuf, (znpBuf[SRSP_LENGTH_FIELD] + SRSP_HEADER_SIZE));
        }             
        znpBuf[SRSP_LENGTH_FIELD] = 0;
    } 
}

/** Note: can also use interrupts to detect when incoming message is received.
Configure high>low edge triggered interrupt for SRDY signal from ZNP.
When SRDY goes low, the interrupt will fire
in the interrupt service routine, first check MRDY. 
If MRDY is low (active) then ignore the interrupt, since transaction is happening
If MRDY is not low then disable interrupt, poll(), and set msg received flag, and enable interrupt
Also be sure to disable the interrupt in methods that access the ZNP, sendSreq(), spiPoll(), etc.
*/

/* @} */