/**
* @ingroup apps
* @{
*
* @file example_basic_comms_router_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee Router, joins a network, then sends a message to the coordinator periodically.
* 
* Demonstrates using a state machine with the ZNP. Uses the AF/ZDO interface.
*
* @note This matches example_simple_application_coordinator.c
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
#include "../ZNP/application_configuration.h"
#include "../ZNP/af_zdo.h"
#include "znp_simple_app_utils.h" 
#include "Messages/infoMessage.h"
#include <string.h>  

unsigned int sequenceNumber = 0;  //an application-level sequence number to track acknowledgements from server

extern signed int znpResult;      //from znp_interface

/** function pointer (in hal file) for the function that gets called when the timer generates an int*/
extern void (*timerIsr)(void);

/** This is the current state of the application. 
* Gets changed by other states, or based on messages that arrive. */
unsigned int state = STATE_ZNP_STARTUP;

/** The main application state machine */
void stateMachine();

/** Various flags between states */
unsigned int stateFlags = 0;
#define STATE_FLAG_SEND_INFO_MESSAGE 0x01

/** Handles timer interrupt */
void handleTimer();

/** Variable to track the cause of the Info Message; whether it be CAUSE_SCHEDULED or CAUSE_MOTION etc.*/
unsigned char infoMessageCause = CAUSE_STARTUP;

struct infoMessage im;
struct header hdr;

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");    
    printf("Simple Application Example - ROUTER - using AFZDO\r\n");
    unsigned int vlo = calibrateVlo();
    printf("VLO = %u Hz\r\n", vlo);   
    timerIsr = &handleTimer;    
    HAL_ENABLE_INTERRUPTS();
    
    //create the infoMessage. Most of these fields are the same, so we can create most of the message ahead of time. 
    hdr.sequence = 0;  //this will be incremented each message
    hdr.version = INFO_MESSAGE_VERSION;
    hdr.flags = INFO_MESSAGE_FLAGS_NONE;
    im.header = &hdr;
    im.deviceType = DEVICETYPE_SMITH_ELECTRONCS_ROUTER_DEMO;
    im.deviceSubType = DEVICESUBTYPE_SMITH_ELECTRONCS_ROUTER_DEMO;
    im.numParameters = 1;
                
    //run the state machine
    stateMachine();
}

void stateMachine()
{
    while (1)
    {
        switch (state)
        {
        case STATE_IDLE:
            {
                if (stateFlags & STATE_FLAG_SEND_INFO_MESSAGE)  //if there is a pending info message to be sent
                {
                    state = STATE_SEND_INFO_MESSAGE;            //then send the message and clear the flag
                    stateFlags &= ~STATE_FLAG_SEND_INFO_MESSAGE;
                }
                //note: other flags (for different messages or events) can be added here
                break;
            }
            
        case STATE_ZNP_STARTUP:
            {
#define ZNP_START_DELAY_IF_FAIL_MS 5000
                signed int startResult;
                while ((startResult = startZnp(ROUTER)) != ZNP_SUCCESS)
                {
                    printf("FAILED. Error Code %i, ZNP Result %i. Retrying...\r\n", startResult, znpResult);
                    delayMs(ZNP_START_DELAY_IF_FAIL_MS);
                }
                
                printf("Success\r\n"); 
                
                //ZNP Initialized so store MAC Address
                memcpy(hdr.mac, getMacAddress(), 8); 
                
                //Now safe to enable message timer:
                signed int timerResult = initTimer(4, NO_WAKEUP);
                if (timerResult != 0)
                {
                    printf("timerResult Error %i, STOPPING\r\n", timerResult);
                    while (1);   
                }
                state = STATE_DISPLAY_NETWORK_INFORMATION;
                break;
            }
        case STATE_DISPLAY_NETWORK_INFORMATION:
            {
                printf("~ni~");
                /* On network, display info about this network */ 
                getNetworkConfigurationParameters();                
                getDeviceInformation();
                state = STATE_SEND_INFO_MESSAGE;
                break;   
            }
        case STATE_SEND_INFO_MESSAGE:
            {
                printf("~im~");
                setLed(1);
                hdr.sequence = sequenceNumber++;                
                im.cause = infoMessageCause;
                unsigned char* sa = getDeviceInformationProperty(DIP_PANID);
                im.parameters[0] = CONVERT_TO_INT(*sa, *(sa+1));
                printInfoMessage(&im);
#define ZNP_RESTART_DELAY_IF_MESSAGE_FAIL_MS 5000
                unsigned char msgBuf[100];
                serializeInfoMessage(&im, msgBuf);
                afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, INFO_MESSAGE_CLUSTER, msgBuf, getSizeOfInfoMessage(&im));
                if (znpResult != ZNP_SUCCESS)
                {
                    printf("afSendData error %i; restarting...\r\n", znpResult);
                    delayMs(ZNP_RESTART_DELAY_IF_MESSAGE_FAIL_MS);  //allow enough time for coordinator to fully restart, if that caused our problem
                    state = STATE_ZNP_STARTUP;
                } else {      
                    state = STATE_IDLE;
                }
                break;   
            }
        default:     //should never happen
            {
                printf("UNKNOWN STATE\r\n");
                state = STATE_ZNP_STARTUP;
            }
            break;
        }
    } 
}

/** Handles timer interrupt */
void handleTimer()
{
    printf("$");   
    infoMessageCause = CAUSE_SCHEDULED;    
    stateFlags |= STATE_FLAG_SEND_INFO_MESSAGE;
}


/* @} */