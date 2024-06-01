#include "device_registers.h"
#include "SW_SPI.h"


// Macros for GPIO operations
#define SET_PIN_HIGH(port, pin) ((port)->PSOR |= (1U << (pin)))
#define SET_PIN_LOW(port, pin)  ((port)->PCOR |= (1U << (pin)))



uint8_t reverseBits_2(uint8_t value)
{
    uint8_t result = 0;
    int i;
    for (i = 0; i < 8; i++) {
        result <<= 1;
        result |= (value & 1);
        value >>= 1;
    }

    return result;
}


// Function to initialize GPIO for SPI
void SPI_GPIO_Init(void)
{
    // Configure MOSI (PTB4), MISO (PTD16), SCK (PTD15), CS (PTB0) as GPIO

    // Enable clock for PORTB
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
    // Enable clock for PORTB
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    PORTB->PCR[4] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;   //MOSI
    PORTD->PCR[15] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;   //SCK
    PTD->PDDR |= (1U << 15); // PTD15 (SCK), PTD16 (MISO) as GPIO
    PTB->PDDR |= (1U << 4); // PTD15 (SCK), PTD16 (MISO) as GPIO


    ////miso
    // Set Pin 16 on Port D as an input
    PTD->PDDR &= ~(1U << 16);
    // Configure Pin 16 on Port D as GPIO with input
    // PORTD->PCR[16] = PORT_PCR_MUX(1);
    // Optional: Enable internal pull-up or pull-down resistor
    // PORTD->PCR[16] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    PORTD->PCR[16] &= ~PORT_PCR_MUX_MASK;  // Clear MUX bits to configure as GPIO
    PORTD->PCR[16] |= PORT_PCR_MUX(1);     // Set MUX bits for GPIO

}


void delay(volatile uint32_t d)
{
	while(d--);
}


// Function to transmit a byte over software SPI
uint8_t SPI_Transmit(uint8_t data)
{
    uint8_t receivedData = 0;
    int8_t i;

    //data = reverseBits_2(data);
    for (i = 7; i >= 0; --i) {
        // Set up MOSI based on the current bit of data
        if (data & (1 << i)) {
            SET_PIN_HIGH(PTB, 4); // Set MOSI high
        } else {
            SET_PIN_LOW(PTB, 4);  // Set MOSI low
        }

        // Generate clock pulse (rising edge)
        SET_PIN_HIGH(PTD, 15); // Set SCK high
        // Read MISO
        if (PTD->PDIR & (1U << 16)) {
            receivedData |= (1 << i);
        }
        delay(1000);

        // Simulate some delay based on your requirements
        SET_PIN_LOW(PTD, 15);  // Set SCK low


        delay(1000);

    }

    //receivedData = reverseBits_2(receivedData);
    return receivedData;
}


// Function to perform SPI transaction
uint8_t SPI_Transaction(uint8_t data)
{
    uint8_t receivedData = SPI_Transmit(data);
    return receivedData;
}


