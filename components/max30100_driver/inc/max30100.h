#ifndef INC_MAX30100_DRIVER_H_
#define INC_MAX30100_DRIVER_H_

#include <stdint.h>
#include "sys/cdefs.h"

#define DEVICE_PART_NUMBER              "MAX30100"
#define DEVICE_NAME                     "Pulse Oxymeter and HRV Sensor"


#define MAX30100_DEVICE_ADDR            0x57U

#define MAX30100_REG_BASE_ADDR          0x00U


#define MAX30100_POR_MASK               0x00

/*
* There are 5 interrupts and the functionality of each is exactly the same: 
  pulling the active-low interrupt pin into its low state until the interrupt is cleared
*/
#define MAX30100_INT_STATUS             0x00U  

/* Bit 7: FIFO Almost Full Flag (A_FULL) */ 
/* Bit 6: Temperature Ready Flag (TEMP_RDY) */
/* Bit 5: Heart Rate Data Ready (HR_RDY) */
/* Bit 4: SpO2 Data Ready (SPO2_RDY) */
/* Bit 0: Power Ready Flag (PWR_RDY) */

/*
When an interrupt enable bit is set to zero, the corresponding interrupt appears as 1 
in the interrupt status register, but the INT pin is not pulled low.
*/
#define MAX_30100_INT_ENABLE                  0x01U
uint8_t MAX30100_ENABLE_SPO2_RDY_INT        = 0x10;
uint8_t MAX30100_DISABLE_SPO2_RDY_INT       = 0x00;

uint8_t MAX30100_ENABLE_HR_RDY_INT          = 0x20;
uint8_t MAX30100_DISABLE_HR_RDY_INT         = 0x00;

uint8_t MAX30100_ENABLE_TEMP_RDY_INT        = 0x40;
uint8_t MAX30100_DISABLE_TEMP_RDY_INT       = 0x00;

uint8_t MAX30100_ENABLE_FIFO_AFULL_INT      = 0x80;
uint8_t MAX30100_DISABLE_FIFO_AFULL_INT     = 0x00;

uint8_t MAX30100_ENABLE_ALL_INTERRUPTS      = 0xF0;
uint8_t MAX30100_DISABLE_ALL_INTERRUPTS     = 0x00;

/*
The FIFO write pointer points to the location where the MAX30100 writes the next sample
*/
#define MAX_30100_FIFO_WRTIE_PTR                        0x02U

/*
* When the FIFO is full, samples are not pushed on to the FIFO, samples are lost. 
* OVF_COUNTER counts the number of samples lost
* It saturates at 0xF
*/
#define MAX_30100_FIFO_OVFLOW_COUNTER                   0x03U
#define MAX_30100_FIFO_OVFLOW_COUNTER_SATURATE          0xF

/*
The FIFO read pointer points to the location from where the processor gets the 
next sample from the FIFO via the I2C interface
*/
#define MAX_30100_FIFO_READ_PTR                         0x04U

/*
* The circular FIFO depth is 16 and can hold up to 16 samples of SpO2 channel data (Red and IR)
* FIFO_READ_PTR points to the next sample to be read from the FIFO_DATA register.
* Each sample consists of an IR and a red data word (2 registers), is 4 bytes of data, 
  so to read one sample requires 4 I2C byte reads in a row.
* The data FIFO consists of a 16-sample memory bank that stores both IR and RED ADC data
* there are 4 bytes of data for each sample, and therefore, 64 total bytes of data can be stored in the FIFO
* In heart-rate only mode, the 3rd and 4th bytes of each sample return zeros.
*/
#define MAX_30100_FIFO_DATA_REG                         0x05U
 
/*
*************  Pseudo-Code Example of Reading Data from FIFO  ********************

**** First transaction: Get the FIFO_WR_PTR ****
START;
Send device address + write mode Send address of FIFO_WR_PTR; REPEATED_START;
Send device address + read mode Read FIFO_WR_PTR;
STOP;
The central processor evaluates the number of samples to be read from the FIFO:
NUM_AVAILABLE_SAMPLES = FIFO_WR_PTR – FIFO_RD_PTR
NUM_SAMPLES_TO_READ <= NUM_AVAILABLE_SAMPLES 


**** Second transaction: Read NUM_SAMPLES_TO_READ samples from the FIFO ****
START;
Send device address + write mode
Send address of FIFO_DATA;
REPEATED_START;
Send device address + read mode
for (i = 0; i < NUM_SAMPLES_TO_READ; i++) { Read FIFO_DATA;
Save IR[15:8];
Read FIFO_DATA;
Save IR[7:0];
Read FIFO_DATA;
Save R[15:8];
Read FIFO_DATA;
Save R[7:0];
} STOP;


**** Third transaction: Write to FIFO_RD_PTR register **** 
** If the second transaction was successful, FIFO_RD_PTR points to the next sample in the FIFO, 
and this third transaction is not necessary. Otherwise, the processor updates the 
FIFO_RD_PTR appropriately, so that the samples are reread.
START;
Send device address + write mode Send address of FIFO_RD_PTR; Write FIFO_RD_PTR;
STOP;

*****************  End of Pseudo Code  *******************
*/

/* Mode Configuration */
#define MAX_30100_MODE_CONFIG_REG                           0x06U
/* Bit 7: Shutdown Control (SHDN) */
uint8_t MAX_30100_MODE_SHUT_DOWN                          = 0x80;
/* Bit 6: Reset Control (RESET) */
uint8_t MAX_30100_MODE_RESET                              = 0x40;
/* Bit 3: Temperature Enable (TEMP_EN) */
uint8_t MAX_30100_EN_TEMP                                 = 0x08;
/* Bits 2:0: Mode Control */
uint8_t MAX30100_MODE_HR_ONLY                             = 0x02;
uint8_t MAX30100_MODE_SPO2_EN                             = 0x03;


/* SPO2 configuration */
#define MAX_30100_SPO2_CONFIG                             0x07U

/* Bit 6: SpO2 High Resolution Enable (SPO2_HI_RES_EN) */
uint8_t MAX_30100_SPO2_HI_RES_EN = 0x40;

/* Bit 4:2: SpO2 Sample Rate Control */
uint8_t MAX_30100_SPO2_SR_50_PER_SEC                    = 0x00;
uint8_t MAX_30100_SPO2_SR_100_PER_SEC                   = 0x04;
uint8_t MAX_30100_SPO2_SR_167_PER_SEC                   = 0x08;
uint8_t MAX_30100_SPO2_SR_200_PER_SEC                   = 0x0C;
uint8_t MAX_30100_SPO2_SR_400_PER_SEC                   = 0x10;
uint8_t MAX_30100_SPO2_SR_600_PER_SEC                   = 0x14;
uint8_t MAX_30100_SPO2_SR_800_PER_SEC                   = 0x18;
uint8_t MAX_30100_SPO2_SR_1000_PER_SEC                  = 0x1C;

/* Bits 1:0: LED Pulse Width Control */
uint8_t MAX_30100_LED_PW_200_us                         = 0x00;
uint8_t MAX_30100_LED_PW_400_us                         = 0x01;
uint8_t MAX_30100_LED_PW_800_us                         = 0x02;
uint8_t MAX_30100_LED_PW_1600_us                        = 0x03;


/* LED configuration */
#define MAX_30100_LED_CONFIG                             0x09U

/* Bits 7:4: Red LED Current Control */
uint8_t MAX_30100_RED_LED_CURRENT_0_MA                 = 0x00;
uint8_t MAX_30100_RED_LED_CURRENT_4_MA                 = 0x10;
uint8_t MAX_30100_RED_LED_CURRENT_8_MA                 = 0x20;
uint8_t MAX_30100_RED_LED_CURRENT_11_MA                = 0x30;
uint8_t MAX_30100_RED_LED_CURRENT_14_MA                = 0x40;
uint8_t MAX_30100_RED_LED_CURRENT_17_MA                = 0x50;
uint8_t MAX_30100_RED_LED_CURRENT_21_MA                = 0x60;
uint8_t MAX_30100_RED_LED_CURRENT_24_MA                = 0x70;
uint8_t MAX_30100_RED_LED_CURRENT_27_MA                = 0x80;
uint8_t MAX_30100_RED_LED_CURRENT_30_MA                = 0x90;
uint8_t MAX_30100_RED_LED_CURRENT_33_MA                = 0xA0;
uint8_t MAX_30100_RED_LED_CURRENT_37_MA                = 0xB0;
uint8_t MAX_30100_RED_LED_CURRENT_40_MA                = 0xC0;
uint8_t MAX_30100_RED_LED_CURRENT_43_MA                = 0xD0;
uint8_t MAX_30100_RED_LED_CURRENT_46_MA                = 0xE0;
uint8_t MAX_30100_RED_LED_CURRENT_50_MA                = 0xF0;


/* Bits 3:0: IR LED Current Control */
uint8_t MAX_30100_IR_LED_CURRENT_0_MA                 = 0x00;
uint8_t MAX_30100_IR_LED_CURRENT_4_MA                 = 0x01;
uint8_t MAX_30100_IR_LED_CURRENT_8_MA                 = 0x02;
uint8_t MAX_30100_IR_LED_CURRENT_11_MA                = 0x03;
uint8_t MAX_30100_IR_LED_CURRENT_14_MA                = 0x04;
uint8_t MAX_30100_IR_LED_CURRENT_17_MA                = 0x05;
uint8_t MAX_30100_IR_LED_CURRENT_21_MA                = 0x06;
uint8_t MAX_30100_IR_LED_CURRENT_24_MA                = 0x07;
uint8_t MAX_30100_IR_LED_CURRENT_27_MA                = 0x08;
uint8_t MAX_30100_IR_LED_CURRENT_30_MA                = 0x09;
uint8_t MAX_30100_IR_LED_CURRENT_33_MA                = 0x0A;
uint8_t MAX_30100_IR_LED_CURRENT_37_MA                = 0x0B;
uint8_t MAX_30100_IR_LED_CURRENT_40_MA                = 0x0C;
uint8_t MAX_30100_IR_LED_CURRENT_43_MA                = 0x0D;
uint8_t MAX_30100_IR_LED_CURRENT_46_MA                = 0x0E;
uint8_t MAX_30100_IR_LED_CURRENT_50_MA                = 0x0F;


/* Temperature Register */

/* T_MEASURED = T_INTEGER + T_FRACTION */
/* T_MEASURED in hex corresponds to temp in respective decimal*/
#define MAX_30100_TEMP_INT_VALUE                        0x16U
#define MAX_30100_TEMP_FRAC_VALUE                       0x17U



#endif /*   INC_MAX30100_DRIVER_H_  */