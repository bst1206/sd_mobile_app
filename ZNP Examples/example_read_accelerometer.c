/**
* @ingroup hwExamples
* @{ 
* @file example_read_accelerometer.c
* 
* @brief Reads accelerometer values in the X-axis, Y-axis, and Z-axis and outputs to console regularly.
* 
* Uses the Hardware Abstraction Layer (hal) to access the accelerometer. Configures Accelerometer as:
* - Range = 2g
* - Disable I2C
* - Measure acceleration 100 times per sec
* - Disable interrupt
* 
* @see VTI Technologies document: CMA3000-D01 datasheet
* @see VTI Technologies document: Product Family Specification, CMA3000-D0X Series 3-axis accelerometer
* @see VTI Technologies document: Interfacing CMA3000-D01 to an MSP430 ultra low-power microcontroller
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
* 
* $Rev: 602 $
* $Author: dsmith $
* $Date: 2010-06-16 13:09:46 -0700 (Wed, 16 Jun 2010) $
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

#define DELAY_BETWEEN_ACCELEROMETER_READINGS_MS 100  //make this higher to output less often

int main( void )
{
    halInit(); 
    printf("Accelerometer Interface Example\r\n");
    halSpiInitAccelerometer();
    HAL_ENABLE_INTERRUPTS();

    // Configure Accelerometer: 2G range, disable I2C, 100Hz Measurement Mode, Disable Interrupt    
    writeAccelerometerRegister(ACCEL_CTRL, G_RANGE_2 | I2C_DIS | MODE_100 | INT_DIS);

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

    printf("Reading Accelerometer:\r\n");
    
    while (1) 
    {
        signed char x = readAccelerometerRegister(ACCEL_DOUTX);                         //X, Y, and Z values are SIGNED.
        delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); 
        signed char y = readAccelerometerRegister(ACCEL_DOUTY); 
        delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); 
        signed char z = readAccelerometerRegister(ACCEL_DOUTZ); 
        delayUs(ACCELEROMETER_DELAY_BETWEEN_OPERATIONS_US); 
        printf("X = %d   Y = %d   Z = %d\r\n", x,y,z);
        toggleLed(1);         
        delayMs(DELAY_BETWEEN_ACCELEROMETER_READINGS_MS);          
    }
}

/* @} */