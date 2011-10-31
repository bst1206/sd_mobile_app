/**
* @ingroup znpCommunications
* @{
*
* @file example_basic_comms_router_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee Router, joins a network, then sends a message to the coordinator once per second.
* Uses the AF/ZDO interface.
* Pressing the button will display device information.
*
* This matches example_basic_communications_COORDINATOR.c
* Get the coordinator running first, or else the router won't have anything to join to.
* Basic Router Startup:
* - Reset Radio
* - Set Startup Options = CLEAR_STATE and CLEAR_CONFIG - This will restore the ZNP to "factory" configuration
* - Reset Radio
* - Set Zigbee DeviceType to ROUTER
* - If you want to set a custom PANID or channel list, do that here and then reset the radio
* - Register Application (Configure the ZNP for our application)
* - Start Application
* - Wait for the Start Confirm
*
* If devices are not communicating, look at the device information fields and verify that both
* the coordinator and router have the same PAN ID and that the Extended PAN ID of the router
* matches the MAC address of the coordinator.
*
* @note code size for this example is ~0.5kB larger than SAPI example due to getDeviceInformation() in handleButtonPress().
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 721 $
* $Author: dsmith $
* $Date: 2010-08-19 10:52:40 -0700 (Thu, 19 Aug 2010) $
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
#include "../ZNP/application_configuration.h"
#include "../ZNP/af_zdo.h"
#include "znp_example_utils.h"

/** function pointer (in hal file) for the function that gets called when a button is pressed*/
extern void (*buttonIsr)(void);

void handleButtonPress(void);

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");    
    printf("Basic Communications Example - ROUTER - using AFZDO\r\n");
    buttonIsr = &handleButtonPress;    
    HAL_ENABLE_INTERRUPTS();
    
    /* Initialize the ZNP */
    printf("Initializing the ZNP  ");
    znpInit(); 
    handleReturnValue();
    
    /* Set Startup Options (will restore the ZNP to default values on reset) */
    printf("Setting StartupOptions  ");    
    setStartupOptions(STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE);
    handleReturnValue();
    
    /* Reset the ZNP */
    printf("Reset the ZNP  ");    
    znpReset();
    handleReturnValue();
    
    /* Set Zigbee Device Type to be ROUTER */
    printf("Setting Zigbee Device Type  "); 
    setZigbeeDeviceType(ROUTER);
    handleReturnValue();

    /* Configure the ZNP for our application */
    printf("Registering Application  ");   
    afRegisterGenericApplication();
    handleReturnValue();

    /* Now, start the application. We will receive a START_REQUEST_SRSP, and then if it is successful, a START_CONFIRM. */
    printf("Starting the Application  ");     
    zdoStartApplication();
    handleReturnValue();
    
    /** Wait until we get on the network. 
    We will receive a ZDO_STATE_CHANGE_IND message whenever the state changes. */
    waitForDeviceState(DEV_ROUTER);

    printf("On Network!\r\n");
    setLed(0);
    /* On network, display info about this network */ 
#ifdef GET_NETWORK_CONFIGURATION_PARAMETERS    //excluded to reduce code size
    getNetworkConfigurationParameters();   
    getDeviceInformation();
#endif 
    
    /* Now the network is running - send a message to the coordinator every few seconds.*/
    unsigned char counter = 0;
    unsigned char testMessage[] = "Read this message man\0";
#define TEST_CLUSTER 0x77    
    
    while (1)
    {         
        printf("Sending Message %u  %s\r\n", counter++, testMessage);
        afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, TEST_CLUSTER, testMessage, 22);
        handleReturnValue();
        toggleLed(1);         
        delayMs(5000);          
    }   
}

/** When a button is pressed, display device information */
void handleButtonPress(void)
{
    printf("Device Information:\r\n"); 
    getDeviceInformation();               // if button still pressed then display device information
}

/* @} */