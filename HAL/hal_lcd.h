#ifndef HAL_LCD_H
#define HAL_LCD_H

#include "msp430x22x4.h"

// Pins on the MSP430 we're using
#define     TXD                   BIT2     // TXD on P2.2
#define     BUTTON                BIT3     // Button on P1.3

//   Conditions for 9600 Baud SW UART, SMCLK = 1MHz
#define     Bitime                13       // 1,000,000 / 8 / 9600 = ~13
#define     DISPLAY_DURATION      200000

// function prototypes
void sendCommand(char cmd);
void displayString(char *string);
void TXString(char *string);
void ConfigureTimerUart();
void Transmit();
void brag(void);
void itoa(unsigned int val, char *str, unsigned int limit);

// DO NOT USE
//void init_lcd( void )
//{
//  // Set clock to 1MHz, and SMCLK to 125kHz
//  BCSCTL1 = CALBC1_1MHZ;
//  DCOCTL = CALDCO_1MHZ;
//  BCSCTL2 &= ~(DIVS_3);         //SMCLK is DCO/8
//
//  // Set up the timer to simulate a UART
//  ConfigureTimerUart();
//
//  // Set up button (P1.3)
//  P1DIR &= ~BUTTON;
//  P1OUT |= BUTTON;
//  P1REN |= BUTTON;
//  P1IES |= BUTTON;
//  P1IFG &= ~BUTTON;
//  P1IE |= BUTTON;
//
//  // Turn on the LED
//  P1DIR |= (BIT0|BIT6);
//  P1OUT |= BIT0;
//  P1OUT &= ~BIT6;
//
//  // wait for LCD display to initialize
//  
//  __enable_interrupt();
//
//  // Set brightness full
//  
//
//  brag();
//
//  // Display the current button count, then go into LPM4. The button
//  // interrupt handler will turn the CPU back on which will bring us back
//  // to the beginning of the while() loop.
//  while(1)
//  {
//    brag();
//    __bis_SR_register(LPM4_bits + GIE);
//  }
//}

// base-10 itoa for positive numbers. Populates str with a null-terminated string.
// limit lets you overflow back to 1 when you hit the limit+1, 2*limit+1, ...
// make sure *str is an array that can hold the number of digits in your limit + 1.
//void itoa(unsigned int val, char *str, unsigned int limit)
//{
//  int temploc = 0;
//  int digit = 0;
//  int strloc = 0;
//  char tempstr[5]; //16-bit number can be at most 5 ASCII digits;
//
//  if(val>limit)
//    val %= limit;
//
//  do
//  {
//    digit = val % 10;
//    tempstr[temploc++] = digit + '0';
//    val /= 10;
//  } while (val > 0);
//
//  // reverse the digits back into the output string
//  while(temploc>0)
//    str[strloc++] = tempstr[--temploc];
//
//  str[strloc]=0;
//}

// Output the number of button presses to LCD
#endif