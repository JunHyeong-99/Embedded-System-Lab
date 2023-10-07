#include <stdint.h>

// Define register addresses
#define RCC_APB2ENR  (*(volatile uint32_t*)0x40021018)

//IDR 버튼 입력값 받음 엘이디 BRR 끄기 BSRR 켜기
// Port C : configuration register low, input data register
#define GPIOC_CRL    (*(volatile uint32_t*)0x40011000)
#define GPIOC_IDR    (*(volatile uint32_t*)0x40011008)

// Port B : configuration register low, input data register
#define GPIOB_CRH    (*(volatile uint32_t*)0x40010C04)
#define GPIOB_IDR    (*(volatile uint32_t*)0x40010C08)

// Port A : configuration register low, input data register
#define GPIOA_CRL    (*(volatile uint32_t*)0x40010800)
#define GPIOA_IDR    (*(volatile uint32_t*)0x40010808)

// Port D : configuration register low, input data register
#define GPIOD_CRL    (*(volatile uint32_t*)0x40011400)
#define GPIOD_CRH    (*(volatile uint32_t*)0x40011404)

// Port D : ouput -> BRR : reset, BSRR : reset or set
#define GPIOD_BRR    (*(volatile uint32_t *) 0x40011414)
#define GPIOD_BSRR   (*(volatile uint32_t *) 0x40011410)

int main(void)
{
    // Enable clock for GPIOA, GPIOB, GPIOC, and GPIOD
    RCC_APB2ENR |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
    
//  사용하기 전 0으로 초기화
    GPIOA_CRL &= ~0xF;
    GPIOB_CRH &= ~0xF00;
    GPIOC_CRL &= ~0xF0000;
    GPIOC_CRH &= ~0xF00000;
    GPIOD_CRL &= ~0xF00FFF00;
	
// 3 == outmode설정 8 = inputmode로 설정 각핀에 해당하는 곳에 설정 CRL = 0~7 CRH = 8~15
    GPIOA_CRL &= ~0x8;	
    GPIOB_CRH &= ~0x800;
    GPIOC_CRL &= ~0x80000;
    GPIOC_CRH &= ~0x800000;
    GPIOD_CRL &= ~0x30033300;

    // Initialize Port D output
    GPIOD_BSRR &= 0x00000000;
    GPIOD_BRR &= 0x00000000;
    
    while (1)
    {
        if (!(GPIOC_IDR & (1 << 4)))  // Check if KEY1 (PC4) is pressed
        {
            GPIOD_BRR |= 0x04; // LED1 (PD2) ON
            GPIOD_BRR |= 0x08; // LED2 (PD3) ON
        }

        if (!(GPIOB_IDR & (1 << 10)))  // Check if KEY2 (PB10) is pressed
        {
            GPIOD_BSRR |= 0x04; // LED1 (PD2) OFF
            GPIOD_BSRR |= 0x08; // LED2 (PD3) OFF
        }

        if (!(GPIOC_IDR & (1 << 13)))  // Check if KEY3 (PC13) is pressed
        {
            GPIOD_BRR |= 0x10; // LED3 (PD4) ON
            GPIOD_BRR |= 0x80; // LED4 (PD7) ON
        }

        if (!(GPIOA_IDR & (1 << 0)))  // Check if KEY4 (PA0) is pressed
        {
            GPIOD_BSRR |= 0x10; // LED3 (PD4) OFF
            GPIOD_BSRR |= 0x80; // LED4 (PD7) OFF
        }
    }

    return 0;
}
