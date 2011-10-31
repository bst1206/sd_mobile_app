/**
* @ingroup znpCommunications
* @{
*
* @file example_secure_comms_router_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee Router, joins a network, then sends a message to the coordinator once per second.
* Uses the AF/ZDO interface.
*
* This matches example_secure_communications_COORDINATOR.c
* Get the coordinator running first, or else the router won't have anything to join to.
* Basic Router Startup with security:
* - Reset Radio
* - Set Startup Options = CLEAR_STATE and CLEAR_CONFIG - This will restore the ZNP to "factory" configuration
* - Reset Radio
* - Set Zigbee DeviceType to ROUTER
* - Set security on, set preconfigured keys on/off, and set key (if using pre-configured keys)
* - If you want to set a custom PANID or channel list, do that here and then reset the radio
* - Register Application (Configure the ZNP for our application)
* - Start Application
* - Wait for the Start Confirm
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 624 $
* $Author: dsmith $
* $Date: 2010-07-02 10:58:43 -0700 (Fri, 02 Jul 2010) $
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
#include "znp_example_utils.h"   //for handleReturnValue() and polling()

//uncomment only ONE of the two options below:
#define USE_SECURITY_MODE_PRECONFIGURED_KEYS
//#define USE_SECURITY_MODE_COORD_DIST_KEYS

//encryption key used in security
unsigned char key[16] = {0x44, 0x65, 0x72, 0x65, 0x6B, 0x53, 0x6D, 0x69, 0x74, 0x68, 0x44, 0x65, 0x73, 0x69, 0x67, 0x6E};

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");    
    printf("Secure Communications Example - ROUTER - using AFZDO\r\n");
    HAL_ENABLE_INTERRUPTS();
    
    //Simple idiot check to ensure that we didn't accidentally define both security options.
#if defined(USE_SECURITY_MODE_PRECONFIGURED_KEYS) && defined(USE_SECURITY_MODE_COORD_DIST_KEYS)
    printf("ERROR - only select one security option!\r\n");
    while (1);
#endif 
    
    /* Initialize the ZNP */
    printf("Initializing the ZNP\r\n");
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
    setZigbeeDeviceType(ROUTER);
    handleReturnValue();

    /* Configure security mode, if it is being used */
#ifdef USE_SECURITY_MODE_PRECONFIGURED_KEYS
    printf("PRECONFIGURED KEYS\r\n");
    /* Turn security ON with pre-configured keys */
    setSecurityMode(SECURITY_MODE_PRECONFIGURED_KEYS);
    handleReturnValue();    
    
    /* All devices on the network must be loaded with the same key */ 
    setSecurityKey(key);
    handleReturnValue();    
#endif
    
#ifdef USE_SECURITY_MODE_COORD_DIST_KEYS
    printf("COORDINATOR DISTRIBUTING KEYS\r\n");
    
    /* Turn security ON with the coordinator distributing keys. 
    Since this is the router we don't need to load a key; it will be sent to us by the coordinator! */
    setSecurityMode(SECURITY_MODE_COORD_DIST_KEYS);
    handleReturnValue();
#endif
    
#if !defined(USE_SECURITY_MODE_PRECONFIGURED_KEYS) && !defined(USE_SECURITY_MODE_COORD_DIST_KEYS)
    printf("WARNING - NO SECURITY OPTION SELECTED; SECURITY OFF\r\n");
#endif 
    
    /* Configure the ZNP for our application */
    printf("Registering Application\r\n");   
    afRegisterGenericApplication();
    handleReturnValue();
    
    /* Now, start the application. We will receive a START_REQUEST_SRSP, and then if it is successful, a START_CONFIRM. */
    printf("Starting the Application\r\n");     
    zdoStartApplication();
    handleReturnValue();
    
    /** Wait until we get on the network. 
    We will receive a ZDO_STATE_CHANGE_IND message whenever the state changes. */
    waitForDeviceState(DEV_ROUTER);

    printf("On Network!\r\n");
    setLed(0);
    
    /* On network, display info about this network */
#ifdef DISPLAY_NETWORK_INFORMATION     
    getNetworkConfigurationParameters();                
    getDeviceInformation();
#endif  
    
    /* Now the network is running - send a message to the coordinator every few seconds.*/
    unsigned char counter = 0;
    unsigned char testMessage[] = {0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9};
#define TEST_CLUSTER 0x77    
    
    while (1)
    {
        printf("Sending Message %u\r\n", counter++);
        afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, TEST_CLUSTER, testMessage, 10);
        handleReturnValue();
        toggleLed(1);         
        delayMs(5000);          
    }   
}


/* @} */