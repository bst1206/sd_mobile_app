#include "hal_lcd.h"
#include "hal_cc2530znp_target_board.h"
#include "znp_example_utils.h"

// variables for serial communication
unsigned char BitCnt;
unsigned int  TXByte;

// How many times have we pushed the button?
unsigned int  buttonPresses = 0;

void sendCommand(char cmd)
{
  TXByte = 0xFE; Transmit();
  TXByte = cmd; Transmit();
}

void displayString(char *string)
{
  // Clear LCD
  TXByte = 0xFE; Transmit();
  TXByte = 0x01; Transmit();  
  __delay_cycles(1);
  
  TXString(string);
  toggleLed(1);  
  
  long displayCounter = 0;
  while(displayCounter< DISPLAY_DURATION)
  {
    if(SRDY_IS_HIGH())
    {
      setLed(1);
      toggleLed(1);
      pollAndDisplay();
      break;
    }
  }

  setLed(1);
  toggleLed(1);
  TXByte = 0xFE; Transmit();
  TXByte = 0x01; Transmit();  
  __delay_cycles(1);
  //TODO: reset vibrator  
}

// Transmit a string via serial UART by looping through it and transmitting
// each character one at a time.
void TXString(char *string)
{
  while(*string != 0)
  {
    TXByte = *string; Transmit();
    string++;
  }
}

// Set up timer for simulated UART. Copied from MSP430 LaunchPad sample code.
void ConfigureTimerUart()
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);
  
  TACCTL0 = OUT;                             // TXD Idle as Mark
  TACTL = TASSEL_2 + MC_2 + ID_3;            // SMCLK/8, continuous mode
  
  P2SEL |= TXD;                              //
  P2DIR |= TXD; 

  
  
  
  __delay_cycles(1000000); //1000000
  
  TXByte = 0x7c; Transmit(); //
  TXByte = 157; Transmit(); //Brightness value (128-157)

//  TXByte = 0x7c; Transmit(); //
//  TXByte = 4; Transmit(); //
//
//  TXByte = 0x7c; Transmit(); //
//  TXByte = 6; Transmit(); //

}

// Function Transmits Character from TXByte
// Copied from MSP430 LaunchPad sample code
void Transmit()
{
  BitCnt = 0xA;                             // Load Bit counter, 8data + ST/SP
  while (TACCR0 != TAR)                       // Prevent async capture
    TACCR0 = TAR;                             // Current state of TA counter
  TACCR0 += Bitime;                     // Some time till first bit
  TXByte |= 0x100;                        // Add mark stop bit to TXByte
  TXByte = TXByte << 1;                 // Add space start bit
  TACCTL0 =  CCIS0 + OUTMOD0 + CCIE;          // TXD = mark = idle
  while ( TACCTL0 & CCIE );                   // Wait for TX completion
}

// Timer A0 interrupt service routine
// Copied from MSP430 LaunchPad sample code
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  TACCR0 += Bitime;                           // Add Offset to CCR0
  if (TACCTL0 & CCIS0)                        // TX on CCI0B?
  {
    if ( BitCnt == 0)
      TACCTL0 &= ~ CCIE;                     // All bits TXed, disable interrupt
    else
    {
      TACCTL0 |=  OUTMOD2;                    // TX Space
      if (TXByte & 0x01)
      {
        TACCTL0 &= ~ OUTMOD2;                   // TX Mark
      }
      TXByte = TXByte >> 1;
      --BitCnt;
    }
  }
}


