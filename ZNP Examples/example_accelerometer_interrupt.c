/**
* @ingroup hwExamples
* @{ 
* @file example_accelerometer_interrupt.c
* 
* @brief Configures accelerometer to generate an interrupt when motion is detected.
* 
* Configures accelerometer to be in motion detect mode with default threshold & time settings to interrupt the MSP430 upon detection of motion.
* Displays the cause of the motion interrupt (X-axis, Y-axis, or Z-axis).
* Uses the Hardware Abstraction Layer (hal) to access the accelerometer.
* Configures Accelerometer with the following settings:
* - Range = 2g
* - Disable I2C
* - Motion Detection Mode
* - Stay in motion detection mode after interrupt.
* 
* @note to clear accelerometer interrupt, must read the Interrupt Status register.
* @note to change the motion detection threshold, modify register ACCEL_MDTHR.
* @note to change the motion time threshold, modify register ACCEL_MDFFTMR
* 
* @see VTI Technologies document: CMA3000-D01 datasheet
* @see VTI Technologies document: Product Family Specification, CMA3000-D0X Series 3-axis accelerometer
* @see VTI Technologies document: Interfacing CMA3000-D01 to an MSP430 ultra low-power microcontroller
* 
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 719 $
* $Author: dsmith $
* $Date: 2010-08-19 10:52:09 -0700 (Thu, 19 Aug 2010) $
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

#include "../HAL/hal_cc2530znp_target_board.h"
#include "../HAL/hal_vti_cma3000_D01_accelerometer.h"

/** Function pointer points to the function that will be called upon Accelerator Interrupt */
extern void (*accelerometerIsr)(void);

/** Our Interrupt Service Routine (ISR) for Accelerometer Interrupt */
void handleAccelerometer();

int main( void )
{
    halInit(); 
    printf("\r\nAccelerometer Interrupt Example\r\n");
    halSpiInitAccelerometer();
    accelerometerIsr = &handleAccelerometer;
    halEnableAccelerometerInterrupt(NO_WAKEUP);
    HAL_ENABLE_INTERRUPTS();
    
    // Configure Accelerometer: 2G range, disable I2C, Motion Detect Mode, Stay in Motion Detect Mode upon motion
    writeAccelerometerRegister(ACCEL_CTRL, G_RANGE_2 | I2C_DIS | MODE_MD_10 | MDET_NO_EXIT);  
    
     // 11 bit-time delay required between operations when using SPI
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US);                                        
    
    // Product Identification
    unsigned char whoAmI = readAccelerometerRegister(ACCEL_WHO_AM_I);                           
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); 
    
    // Product Revision
    unsigned char revId = readAccelerometerRegister(ACCEL_REVID);                               
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); 
    
    printf("Accelerometer Registers: whoAmI = %02X, revId = %02X\r\n", whoAmI, revId);
    
    //Clear any pending interrupts
    readAccelerometerRegister(ACCEL_INT_STATUS);
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); 
    
    printf("Waiting for Accelerometer Interrupt...\r\n");
    while (1) 
    {
        toggleLed(1);                                                                            //Simple indication that program is running
        delayMs(1000);          
    }
}


/** Accelerometer interrupt service routine.
@note application must ensure that other operations are not accessing the accelerometer when interrupt fires. 
(Disable accelerometer interrupt during other communications with accelerometer, or even better, set a flag in the ISR that gets polled by your main application)
@pre accelerometerIsr function pointer points here; e.g. <code>accelerometerIsr = &handleAccelerometer;</code>
@pre global interrupts enabled
@pre accelerometer interrupt enabled; e.g. <code>halEnableAccelerometerInterrupt(NO_WAKEUP);</code>
@post interrupt is cleared
*/
void handleAccelerometer()
{
    toggleLed(0); 
    unsigned char interruptStatus = readAccelerometerRegister(ACCEL_INT_STATUS);  //will clear the interrupt
    printf("Interrupt: %s(%02X)\r\n", getAccelerometerInterruptReason(interruptStatus),interruptStatus);
    delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); //in case this interrupt is immediately followed by another one
}

/* @} */