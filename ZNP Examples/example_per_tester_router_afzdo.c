/**
* @ingroup znpCommunications
* @{
*
* @file example_per_tester_router_afzdo.c
*
* @brief Packet Error Rate Tester: Resets Radio, configures this device to be a Zigbee Router, joins a network, 
* then sends 1000 messages to coordinator.
*
* Uses the AF/ZDO interface.
*
* @note code size for this example is ~0.5kB larger than PER coordinator due to getDeviceInformation() in handleButtonPress().
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
#include <string.h>                 //for memcpy()

#include "../ZNP/znp_interface_spi.h"

extern unsigned char znpBuf[100];

extern signed int znpResult;                    //from znp_interface

/** function pointer (in hal file) for the function that gets called when a button is pressed*/
extern void (*buttonIsr)(void);
void handleButtonPress(void);

int main( void )
{
    halInit();
    printf("\r\n****************************************************\r\n");    
    printf("Packet Error Rate Tester - ROUTER - using AFZDO\r\n");
    buttonIsr = &handleButtonPress;     
    HAL_ENABLE_INTERRUPTS();
    
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
    waitForDeviceState(DEV_ROUTER);

    printf("On Network!\r\n");
    
    /* On network, display info about this network */
#ifdef DISPLAY_NETWORK_INFORMATION     
    getNetworkConfigurationParameters();                
    getDeviceInformation();
#endif  
    
    /* Now the network is running - send messages to the Coordinator.*/
    unsigned char testMessage[] = {0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9};
#define TEST_CLUSTER 0x77    
    
#define NUMBER_OF_PACKETS_TO_SEND 1000
    
    printf("Sending %u packets to Coordinator\r\n", NUMBER_OF_PACKETS_TO_SEND);
    
    //preload znpBuf Contents:  
    unsigned char testBuf[20];
    testBuf[0] = 20;
    testBuf[1] = MSB(AF_DATA_REQUEST);
    testBuf[2] = LSB(AF_DATA_REQUEST);      
    
    testBuf[3] = 0; 
    testBuf[4] = 0;
    testBuf[5] = DEFAULT_ENDPOINT;
    testBuf[6] = DEFAULT_ENDPOINT;
    testBuf[7] = LSB(TEST_CLUSTER); 
    testBuf[8] = MSB(TEST_CLUSTER); 
    testBuf[9] = 0xFF;  //ignore transaction sequence number
    testBuf[10] = MAC_ACK; //Could also use AF_APS_ACK;
    testBuf[11] = DEFAULT_RADIUS;
    testBuf[12] = 10; 
    memcpy(testBuf+13, testMessage, 10);
    //testBuf is now loaded with our test message.
    
    for (unsigned int ctr = 0; ctr<NUMBER_OF_PACKETS_TO_SEND; ctr++)
    {
        //this is the equivalent of afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, TEST_CLUSTER, testMessage, 10);

        memcpy(znpBuf, testBuf, 20);
        znpResult = sendMessage();
        if (znpResult != 0)
        {
            printf("sendMessage Error %d; stopping\r\n", znpResult);
            for (int i=0; i<znpBuf[0]+3; i++)
                printf("%02x,", znpBuf[i]);
            while (1);
        }
        
        //now, get send confirm:
        while (SRDY_IS_HIGH());
            spiPoll();
        if (CONVERT_TO_INT(znpBuf[2], znpBuf[1]) != AF_DATA_CONFIRM)
        {
            printf("Error; stopped after packet %u", ctr);
        }        
        toggleLed(1); 
        delayMs(1); //wait a couple of mSec else we overload the coord.
    }
    printf("Done! Sent %u packets!\r\n", NUMBER_OF_PACKETS_TO_SEND);
    while(1);
}

void handleButtonPress(void)
{
    getDeviceInformation();               // if button still pressed then display device information
}

/* @} */