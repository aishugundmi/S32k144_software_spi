#include <string.h>
#include <stdint.h>
#include "device_registers.h"           /* include peripheral declarations */
#include "clocks_and_modes.h"
#include "SW_SPI.h"



uint8_t tx_8bits = 0x12;
uint8_t LPSPI0_8bits_read;

 uint16_t  sine_t[] = {
		0x800,0x865,0x8ca,0x92e,0x992,0x9f5,0xa56,0xab6,
		0xb15,0xb71,0xbcc,0xc24,0xc79,0xccb,0xd1b,0xd67,
		0xdb0,0xdf6,0xe37,0xe75,0xeaf,0xee4,0xf15,0xf42,
		0xf6a,0xf8e,0xfad,0xfc7,0xfdc,0xfec,0xff7,0xffe,
		0xfff,0xffb,0xff2,0xfe5,0xfd2,0xfba,0xf9e,0xf7d,
		0xf57,0xf2c,0xefd,0xeca,0xe92,0xe57,0xe17,0xdd3,
		0xd8c,0xd42,0xcf4,0xca3,0xc4f,0xbf8,0xb9f,0xb43,
		0xae6,0xa87,0xa26,0x9c4,0x960,0x8fc,0x897,0x832,
		0x7cd,0x768,0x703,0x69f,0x63b,0x5d9,0x578,0x519,
		0x4bc,0x460,0x407,0x3b0,0x35c,0x30b,0x2bd,0x273,
		0x22c,0x1e8,0x1a8,0x16d,0x135,0x102,0xd3,0xa8,
		0x82,0x61,0x45,0x2d,0x1a,0xd,0x4,0x0,
		0x1,0x8,0x13,0x23,0x38,0x52,0x71,0x95,
		0xbd,0xea,0x11b,0x150,0x18a,0x1c8,0x209,0x24f,
		0x298,0x2e4,0x334,0x386,0x3db,0x433,0x48e,0x4ea,
		0x549,0x5a9,0x60a,0x66d,0x6d1,0x735,0x79a,0x800,
};


void WDOG_disable (void)
{
	WDOG->CNT=0xD928C520;     /* Unlock watchdog 		*/
    WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value */
    WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
}

//void PORT_init (void)
//{
//	/*!
//	 * Pins definitions
//	 * ===================================================
//	 *
//	 * Pin number        | Function
//	 * ----------------- |------------------
//	 * PTB14             | LPSPI1_SCK
//	 * PTB15             | LPSPI1_SIN
//	 * PTB16			 | LPSPI1_SOUT
//	 * PTB17			 | LPSPI1_PCS3
//	 */
//  PCC->PCCn[PCC_PORTB_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
//  PORTB->PCR[14]|=PORT_PCR_MUX(3); /* Port B14: MUX = ALT3, LPSPI1_SCK */
//  PORTB->PCR[15]|=PORT_PCR_MUX(3); /* Port B15: MUX = ALT3, LPSPI1_SIN */
//  PORTB->PCR[16]|=PORT_PCR_MUX(3); /* Port B16: MUX = ALT3, LPSPI1_SOUT */
//  PORTB->PCR[17]|=PORT_PCR_MUX(3); /* Port B17: MUX = ALT3, LPSPI1_PCS3 */
//}


// Function to initialize PTB0 as chip select and set it to logic high
void init_CS0(void) {
    // Enable clock for PORTB
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;

    // Set PTB0 as GPIO
    PORTB->PCR[0] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    // Set PTB0 as output
    PTB->PDDR |= (1U << 0);

    // Set PTB0 to logic high
    PTB->PSOR |= (1U << 0);
}


// Function to initialize PTB5 as chip select and set it to logic high
void init_CS1(void) {
    // Enable clock for PORTB
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;

    // Set PTB5 as GPIO
    PORTB->PCR[5] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    // Set PTB5 as output
    PTB->PDDR |= (1U << 5);

    // Set PTB5 to logic high
    PTB->PSOR |= (1U << 5);
}


//DAC0   X-axis
void CS0_control(int logic)
{
	if(logic){
	    // Set PTB5 to logic high
	    PTB->PSOR |= (1U << 0);
	}else{
		// Set PTB5 to logic low
		PTB->PCOR |= (1U << 0);
	}
}


//DAC1   Y-axis
void CS1_control(int logic)
{
	if(logic){
	    // Set PTB0 to logic high
	    PTB->PSOR |= (1U << 5);
	}else{
		// Set PTB0 to logic low
		PTB->PCOR |= (1U << 5);
	}
}


void PORT_init(void)
{
    /*!
     * Pins definitions
     * ===================================================
     *
     * Pin number        | Function
     * ----------------- |------------------
     * PTB0              | LPSPI0_PCS0 (CS)
     * PTB4              | LPSPI0_SOUT (MOSI)
     * PTD15             | LPSPI0_SCK (SCK)
     * PTD16             | LPSPI0_SIN (MISO)
     */

//    // Enable clock for PORTB and PORTD
//    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
//    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
//
//    // Configure PTB0 (LPSPI0_PCS0 / CS)
//    ///PORTB->PCR[0] |= PORT_PCR_MUX(2);  // MUX = ALT2
//
//    // Configure PTB4 (LPSPI0_SOUT / MOSI)
//    PORTB->PCR[4] |= PORT_PCR_MUX(2);  // MUX = ALT2
//
//    // Configure PTD15 (LPSPI0_SCK)
//    PORTD->PCR[15] |= PORT_PCR_MUX(2);  // MUX = ALT2
//
//    // Configure PTD16 (LPSPI0_SIN / MISO)
//    PORTD->PCR[16] |= PORT_PCR_MUX(2) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;  // MUX = ALT2, Enable pull-up resistor

    init_CS0();
	init_CS1();
	SPI_GPIO_Init();

}


int main(void)
{
  uint32_t counter = 0;

  WDOG_disable();		   /* Disable WDOG */
  SOSC_init_8MHz();        /* Initialize system oscillator for 8 MHz xtal */
  SPLL_init_160MHz();      /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
  NormalRUNmode_80MHz();   /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
  PORT_init();             /* Configure ports */

  int i;
  for(;;)
  {

	  for(i =0; i < sizeof(sine_t)/sizeof(sine_t[0]); i++){

			volatile uint8_t cmd[3] = {0};
			cmd[0] = 0x00;  //ch0 write
			//cmd[0] = 0x08;  //ch1 write

			//cmd[1] = (sine_t[i] >> 8 ) & 0x0F; 
			//cmd[2] = sine_t[i];  

			if(i%2 == 0){
				cmd[1] = 0x00;
				cmd[2] = 0x00;
			}else{
				cmd[1] = 0x0F;
				cmd[2] = 0xFF;
			}

			//CS0_control(0);
			CS1_control(0);

			ddd(1000);
			cmd[0] = SPI_Transmit(cmd[0]);
			cmd[1] = SPI_Transmit(cmd[1]);
			cmd[2] = SPI_Transmit(cmd[2]);
		    //CS0_control(1);
		    CS1_control(1);

		    delay(200000);

	  }
  }
}
