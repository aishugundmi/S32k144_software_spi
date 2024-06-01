#ifndef SW_SPI_H_
#define SW_SPI_H_


uint8_t reverseBits_2(uint8_t value);
uint8_t SPI_Transaction(uint8_t data);
uint8_t SPI_Transmit(uint8_t data);
void SPI_GPIO_Init(void);
void delay(volatile uint32_t d);



#endif /* SW_SPI_H_ */
