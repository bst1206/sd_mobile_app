/**
* @ingroup apps
* @{
*
* @file example_basic_comms_router_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee Router, joins a network, then sends a message to the coordinator once per second.
* Uses the AF/ZDO interface. Reads the ambient light sensor and supply voltage and sends these in the message.
* Also sends a message upon a motion interrupt from the accelerometer. 
*
* @note This matches example_simple_application_coordinator.c
* @note This example will not compile with the IAR Kickstart edition because CODE + CONST exceeds 4kB
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
#include "../HAL/hal_vti_cma3000_D01_accelerometer.h"
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

/** This is the function pointer (in hal file) for the ISR called when the accelerometer generates an interrupt*/
extern void (*accelerometerIsr)(void);

/** Handles Accelerometer interrupt */
void handleAccelerometer();

/** Variable to track the cause of the Info Message; whether it be CAUSE_SCHEDULED or CAUSE_MOTION etc.*/
unsigned char infoMessageCause = CAUSE_STARTUP;

struct infoMessage im;
struct header hdr;
                
/**Comment these out to only send a message on motion or timer respectively */
#define SEND_MESSAGE_ON_MOTION
#define SEND_MESSAGE_ON_TIMER

int main( void )
{
    halInit();
    HAL_DISABLE_INTERRUPTS();
    printf("\r\n****************************************************\r\n");    
    printf("Simple Application Example - END DEVICE - using AFZDO\r\n");
#ifdef SEND_MESSAGE_ON_TIMER    
    unsigned int vlo = calibrateVlo();
    printf("VLO = %u Hz\r\n", vlo);   
    timerIsr = &handleTimer;  
    printf("Send message on timer enabled.\r\n");    
#endif
    
#ifdef SEND_MESSAGE_ON_MOTION
    printf("Send message on motion enabled.\r\n");    
    halSpiInitAccelerometer();  //note: this puts the SPI port in a non-ZNP configuration; must init it for ZNP afterwards
    writeAccelerometerRegister(ACCEL_CTRL, G_RANGE_2 | I2C_DIS | MODE_MD_10 | MDET_NO_EXIT);    // Configure Accelerometer
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US);                                         // 11 bit-time delay required when using SPI
    readAccelerometerRegister(ACCEL_INT_STATUS);  // clear the interrupt
    accelerometerIsr = &handleAccelerometer;
    halEnableAccelerometerInterrupt(WAKEUP_AFTER_ACCELEROMETER);
#endif    
    HAL_ENABLE_INTERRUPTS();
    
    //create the infoMessage. Most of these fields are the same, so we can create most of the message ahead of time. 
    hdr.sequence = 0;  //this will be incremented each message
    hdr.version = INFO_MESSAGE_VERSION;
    hdr.flags = INFO_MESSAGE_FLAGS_NONE;
    im.header = &hdr;  //Note, if you have multiple similar message types then you can use the same header for all
    im.deviceType = DEVICETYPE_SMITH_ELECTRONCS_ROUTER_DEMO;
    im.deviceSubType = DEVICESUBTYPE_SMITH_ELECTRONCS_ROUTER_DEMO;
    im.numParameters = 3;
                
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
                /* Start the network; if fails then wait a second and try again. */
                signed int startResult = startZnp(END_DEVICE);
                while (startResult != ZNP_SUCCESS)
                {
                    printf("FAILED. Error Code %i, ZNP Result %i. Retrying...\r\n", startResult, znpResult);
                    delayMs(ZNP_START_DELAY_IF_FAIL_MS);
                    startResult = startZnp(END_DEVICE);
                }
                printf("Success\r\n"); 
                
                //ZNP Initialized so store MAC Address
                memcpy(hdr.mac, getMacAddress(), 8); 
#ifdef SEND_MESSAGE_ON_TIMER
                signed int timerResult = initTimer(4, WAKEUP_AFTER_TIMER);
                if (timerResult != 0)
                {
                    printf("timerResult Error %i, STOPPING\r\n", timerResult);
                    while (1);   
                }
#endif
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
                im.header->sequence = sequenceNumber++;                
                im.cause = infoMessageCause;
                unsigned char* panid = getDeviceInformationProperty(DIP_PANID);
                im.parameters[0] = CONVERT_TO_INT(*panid, *(panid+1));      //PAN ID
                im.parameters[1] = getVcc3();                               //VCC
                im.parameters[2] = getLightSensor();                        //Light Sensor
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
                    clearLeds();        
                    HAL_SLEEP();                    
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

#ifdef SEND_MESSAGE_ON_TIMER
/** Handles timer interrupt */
void handleTimer()
{
    printf("$");   
    infoMessageCause = CAUSE_SCHEDULED;
    stateFlags |= STATE_FLAG_SEND_INFO_MESSAGE;
}
#endif

#ifdef SEND_MESSAGE_ON_MOTION
/** Handles Accelerometer interrupt */
void handleAccelerometer()
{
    printf("@");
    infoMessageCause = CAUSE_MOTION;
    stateFlags |= STATE_FLAG_SEND_INFO_MESSAGE;
    halSpiInitAccelerometer();
    readAccelerometerRegister(ACCEL_INT_STATUS);  //will clear the interrupt
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); //in case this interrupt is immediately followed by another one
    halSpiInitZnp();  //return SPI configuration to work with the ZNP
}
#endif

/* @} */