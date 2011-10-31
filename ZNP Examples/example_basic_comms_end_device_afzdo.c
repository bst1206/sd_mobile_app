/**
* @ingroup znpCommunications
* @{
*
* @file example_basic_comms_end_device_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee End Device, joins a network, 
* then sends a message to the coordinator every few seconds. Sleeps inbetween transmissions.
*
* Calibrates the Very-Low-Power Oscillator (VLO) in the MSP430. Uses VLO to wakup the processor.
* Note: you could also use an external 32kHz xtal for more accuracy/stability
*
* Uses the AF/ZDO interface.
*
* This matches example_basic_communications_coordinator.c
* Get the coordinator running first, or else the end device won't have anything to join to.
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 720 $
* $Author: dsmith $
* $Date: 2010-08-19 10:52:25 -0700 (Thu, 19 Aug 2010) $
*
* YOU ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY 
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
#include "znp_example_utils.h"   //for handleReturnValue() and polling()

/** function pointer (in hal file) for the function that gets called when the timer generates an int*/
extern void (*timerIsr)(void);

signed int returnValue;

/** Handles timer interrupt */
void handleTimer();

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");    
    printf("Basic Communications Example - END DEVICE - using AFZDO\r\n");
    
    /** Calibrate the Very Low Power Oscillator (VLO) to use for sleep timing */
    printf("Calibrating VLO...\r\n");
    signed int vlo = -1;
    while (vlo < 0)
    {
        vlo = calibrateVlo();
        printf("VLO = %u Hz; ", vlo);
    }
    printf(" Done.\r\n");
    timerIsr = &handleTimer; 
    HAL_ENABLE_INTERRUPTS();
    
    /* Initialize the ZNP */
    znpInit(); 
    handleReturnValue();
    
    /* Set Startup Options (will restore the ZNP to default values on reset) */
    printf("Setting StartupOptions\r\n");    
    setStartupOptions(STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE);
    handleReturnValue();
    
    /* Reset the ZNP */
    printf("Reset the ZNP\r\n");  
    znpReset();
    handleReturnValue();
    
    /* Set Zigbee Device Type to be ROUTER */
    printf("Setting Zigbee Device Type\r\n"); 
    setZigbeeDeviceType(END_DEVICE);
    handleReturnValue();
    
    /* Configure the ZNP for our application */
    printf("Registering Application\r\n");   
    afRegisterGenericApplication();
    handleReturnValue();
    delayMs(500);
    
    /* Now, start the application. We will receive a START_REQUEST_SRSP, and then if it is successful, a START_CONFIRM. */
    printf("Starting the Application\r\n");     
    zdoStartApplication();
    handleReturnValue();

    /** Wait until we get on the network. 
    We will receive a ZDO_STATE_CHANGE_IND message whenever the state changes. */
    waitForDeviceState(DEV_END_DEVICE);
    
    printf("\r\nOn Network!\r\n");
    
    /* On network, display info about this network */
#ifdef DISPLAY_NETWORK_INFORMATION   
    getNetworkConfigurationParameters(); 
    getDeviceInformation();
#endif 
    
    /* Now the network is running - send a message to the coordinator every few seconds.*/
    unsigned char counter = 0;
    unsigned char testMessage[] = {0xA0,0xA1,0xA2,0xA3,0xA4};
#define TEST_CLUSTER 0x77    
    
    //P5SEL |= 0x70;  //Output MCLK on P5.4; SMCLK on P5.5; ACLK on P5.6 to verify sleeping
    
    signed int timerResult = initTimer(4, WAKEUP_AFTER_TIMER);
    if (timerResult != 0)
    {
        printf("timerResult Error %i, STOPPING\r\n", timerResult);
        while (1);   
    }
    
    clearLeds();
    while (1)
    {
        setLed(1);
        printf("Sending Message %u\r\n", counter++);
        afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, TEST_CLUSTER, testMessage, 5);
        handleReturnValue();
        clearLeds();        
        HAL_SLEEP();
    }
}

void handleTimer()  //Will automatically wakeup timer
{
    printf("#");
}

/* @} */