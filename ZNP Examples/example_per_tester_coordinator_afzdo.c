/**
* @ingroup znpCommunications
* @{ 
*
* @file example_per_tester_coordinator_afzdo.c
*
* @brief Packet Error Rate Tester: configures this device to be a Zigbee Coordinator and displays how many messages were received.
* Uses the AF/ZDO interface.
*
* This matches example_per_tester_router_afzdo.c
* Get this running before the router, or else the router won't have anything to join to.
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
#include "znp_example_utils.h" 
#include "../ZNP/znp_interface_spi.h"

signed int returnValue;
extern unsigned char znpBuf[100];
unsigned int packetCounter = 0;
void waitForPackets();

/** function pointer (in hal file) for the function that gets called when a button is pressed*/
extern void (*buttonIsr)(void);

void handleButtonPress(void);

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");
    printf("Packet Error Rate Tester - COORDINATOR - using AFZDO\r\n");
    buttonIsr = &handleButtonPress;       
    HAL_ENABLE_INTERRUPTS();
    setLed(0);
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
    
    /* Set Zigbee Device Type to be COORDINATOR */
    printf("Setting Zigbee Device Type\r\n"); 
    setZigbeeDeviceType(COORDINATOR);
    handleReturnValue();
    
    /* Reset the ZNP */
    printf("Reset the ZNP\r\n");    
    znpReset();
    handleReturnValue();
    
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
    waitForDeviceState(DEV_ZB_COORD);

    printf("On Network!\r\n");
    setLed(1);
    
    /* On network, display info about this network */
#ifdef DISPLAY_NETWORK_INFORMATION     
    getNetworkConfigurationParameters();                
    getDeviceInformation();
#endif  
    
    printf("Silently listening for packets; press button to display count\r\n");
    printf("Count = %u; Start router sending packets\r\n", packetCounter);
    
    /* Now the network is running - continually poll for any received messages from the ZNP */
    waitForPackets();
}

void handleButtonPress(void)
{
    printf("Count = %u\r\n", packetCounter);
}

#define NUMBER_OF_PACKETS_TO_RECEIVE 1000

/** Waits for messages; increments a counter when a packet was received.*/
void waitForPackets()
{
znpBuf[0] = 0;    
    packetCounter = 0;  
    while (packetCounter < NUMBER_OF_PACKETS_TO_RECEIVE) 
    {
        while (SRDY_IS_HIGH());         
        spiPoll();      
        if ((znpBuf[0] > 0) && (znpBuf[1] == 0x44) && (znpBuf[2] == 0x81))  //only increment counter if it was a AF_INCOMING_MSG
        {
            toggleLed(0);
            packetCounter++;
            znpBuf[0] = 0;  
        }
    }
    printf("%u packets received!\r\n", NUMBER_OF_PACKETS_TO_RECEIVE);    
}

/* @} */