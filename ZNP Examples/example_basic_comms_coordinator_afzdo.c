/**
* @ingroup znpCommunications
* @{ 
*
* @file example_basic_comms_coordinator_afzdo.c
*
* @brief Resets Radio, configures this device to be a Zigbee Coordinator, and displays any messages that are received.
* Uses the AF/ZDO interface.
*
* This matches example_basic_communications_ROUTER.c
* Get this running before the router, or else the router won't have anything to join to.
*
* Basic Coordinator Startup:
* - Reset Radio
* - Set Startup Options = CLEAR_STATE and CLEAR_CONFIG - This will restore the ZNP to "factory" configuration
* - Reset Radio
* - Set Zigbee DeviceType to COORDINATOR
* - Note: If you want to set a custom PANID or channel list, do that here
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

//#define FIND_MAC_ADDRESS_OF_SENDER  //define this to print out sender's MAC address

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");
    printf("Basic Communications Example - COORDINATOR - using AFZDO\r\n");
    HAL_ENABLE_INTERRUPTS();
    setLed(0);
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
    
    /* Set Zigbee Device Type to be COORDINATOR */
    printf("Setting Zigbee Device Type  "); 
    setZigbeeDeviceType(COORDINATOR);
    handleReturnValue();
    
    /* Enabling Callbacks (required to receive ZDO_IEEE_ADDR_RSP)  */
//#ifdef FIND_MAC_ADDRESS_OF_SENDER  //define this to print out sender's MAC address
    printf("Enabling Callbacks  "); 
    setCallbacks(CALLBACKS_ENABLED);
    handleReturnValue();    
//#endif
    
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
    waitForDeviceState(DEV_ZB_COORD);

    printf("On Network!\r\n");
    setLed(1);
            
    /* On network, display info about this network */
#ifdef DISPLAY_NETWORK_INFORMATION     
    getNetworkConfigurationParameters();                
    getDeviceInformation();
#endif  

    /* Now the network is running - continually poll for any received messages from the ZNP */
#ifdef FIND_MAC_ADDRESS_OF_SENDER  //define this to print out sender's MAC address
    displayReceivedMessagesAndFindDevice();
#else
    displayReceivedMessages();
#endif    
    
}

/* @} */