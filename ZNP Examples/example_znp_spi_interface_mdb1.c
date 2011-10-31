/**
* @file example_znp_spi_interface_mdb1.c
*
* @brief Single file example of how to interface with ZNP; hardware platform MDB-1.
*
* $Rev: 587 $
* $Author: dsmith $
* $Date: 2010-06-11 10:17:52 -0700 (Fri, 11 Jun 2010) $
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
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

#include "msp430x24x.h"
#include <stdio.h>

#define RADIO_ON()                  (P1OUT |= BIT2)
#define RADIO_OFF()                 (P1OUT &= ~BIT2)
#define SPI_SS_SET()                (P5OUT &= ~BIT0)  //active low
#define SPI_SS_CLEAR()              (P5OUT |= BIT0)  
#define SRDY_IS_HIGH()              (P1IN & BIT3)
#define SRDY_IS_LOW()               ((~P1IN) & BIT3)

/** Initialize oscillator to 8MHz DCO, 2MHz SMCLK */
void oscInit()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)                                     
    {  
        while(1); // Stop if calibration constants erased
    }   
    BCSCTL1 = CALBC1_8MHZ; // Set DCO = 8MHz for MCLK
    DCOCTL = CALDCO_8MHZ;
    BCSCTL2 |= DIVS_1;     //SMCLK = DCO/2 (4MHz) 
}

/** Initialize GPIO ports */
void portInit()
{
    //Note: on this hardware, MRDY is hardwired to CS
    P1DIR = BIT2;    P1OUT &= ~BIT2;    //ZNP Reset Line, hold ZNP in reset. NOTE: SRDY is P1.3, leave as input
    P3SEL = BIT4+BIT5;                  //Debug Console Tx, Rx
    P4DIR = 0xFF;                       //LEDs
    P5SEL = BIT1+BIT2+BIT3;             //SPI SIMO, SOMI, and SCLK
    P5DIR = BIT0 + BIT4+BIT5+BIT6+BIT7;
    P5OUT &= ~(BIT4+BIT5+BIT6+BIT7);    //turn off all relay drive coils
}

/** Initialize serial console */
void debugConsoleInit()
{
    UCA0CTL1 |= UCSSEL_2;                 // Use SMCLK
    UCA0BR0 = 13; UCA0BR1 = 0;                // 4mHz smclk w/modulation for 19,200bps, table 15-5 
    UCA0MCTL = UCBRS_0 + +UCBRF_0 + UCOS16;   // Modulation UCBRSx=1, over sampling  
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    //IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

int putchar(int c)
{	
    while (!(IFG2 & UCA0TXIFG));   // Wait for ready
    UCA0TXBUF = (unsigned char) (c & 0xFF); 
    return c;
}

/** Initialize SPI interface to ZNP */
void halSpiInit()
{
    UCB1CTL1 |= UCSSEL_2 | UCSWRST;                 //serial clock source = SMCLK, hold SPI interface in reset
    UCB1CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;     //clock polarity = inactive is LOW (CPOL=0); Clock Phase = 0; MSB first; Master Mode; Synchronous Mode
    UCB1BR0 = 2;  UCB1BR1 = 0;                      //SPI running at 2MHz (SMCLK / 2)
    UCB1CTL1 &= ~UCSWRST;                           //start USCI_B1 state machine
    SPI_SS_CLEAR(); 
}

/** Write bytes to SPI port. */
void spiWrite(unsigned char *bytes, unsigned char numBytes)
{   //NOTE: you must also control MRDY if not tied together in hardware
    SPI_SS_SET(); 
    while (numBytes--)
    {  
        UCB1TXBUF = *bytes;
        while (!(UC1IFG & UCB1RXIFG)) ; //WAIT for a character to be received, if any
        *bytes++ = UCB1RXBUF;  //read bytes
    }
    SPI_SS_CLEAR();
}

#define TICKS_IN_ONE_MS_8MHZ        1980   
#define TICKS_IN_ONE_MS             TICKS_IN_ONE_MS_8MHZ 

/** Delay by the specified number of mSec */
void delayMs(unsigned int ms)    
{
    for (int i = ms; i; i--)
    {
        for (int j = TICKS_IN_ONE_MS; j; j--);   //delay 1mSec
    }
}

/** Poll for data from the ZNP */
signed int spiPoll(unsigned char *bytes)
{
    while (SRDY_IS_HIGH());
    SPI_SS_SET();      
    *bytes = 0; *(bytes+1) = 0; *(bytes+2) = 0;  //poll message is 0,0,0    
    spiWrite(bytes, (*bytes + 3));              //*bytes (first byte) is length after the first 3 bytes, all frames have at least the first 3 bytes
    while (SRDY_IS_LOW());
    spiWrite(bytes, 3);
    if (*bytes > 0)                             //*bytes (first byte) contains number of bytes to receive
        spiWrite(bytes+3, *bytes);              //write-to-read: read data into buffer     
    return 0;
}

int main( void )
{
    //osc
    oscInit();
    
    //ports
    portInit();
    
    //serial port
    debugConsoleInit();
    
    //spi to ZNP
    halSpiInit();
    
    printf("\r\nResetting ZNP\r\n");  
    
    //reset the ZNP
    RADIO_OFF();
    delayMs(1);
    RADIO_ON(); 
    unsigned char srsp[20];
    spiPoll(srsp);  //received data will be written to this buffer 
    //Typical ZNP reset data: TransportRev = 2; FW Rev = 2.3
    printf("ZNP Reset Reason %u, TransportRev=%u, ProductId=%u, FW Rev=%u.%u, HW Rev=%u\r\n", 
           srsp[3], srsp[4], srsp[5], srsp[6], srsp[7], srsp[8]);
    
}