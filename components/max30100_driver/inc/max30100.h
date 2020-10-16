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





#endif /*   INC_MAX30100_DRIVER_H_  */