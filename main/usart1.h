
#include <stm32f10x.h>

#define SEND_LENGTH 8 

void USART1_RCC_Configuration(void);
void USART1_NVIC_Configuration(void);
void USART1_GPIO_Configuration(void);
void USART1_Configuration(void);
void USART1_IRQHandler(void);

