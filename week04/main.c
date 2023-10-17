// Define register addresses
#define RCC_APB2ENR  (*(volatile unsigned int*)0x40021018)

// Port C : configuration register low, input data register
#define GPIOC_CRL    (*(volatile unsigned int*)0x40011000)
#define GPIOC_IDR    (*(volatile unsigned int*)0x40011008)

// Port B : configuration register low, input data register
#define GPIOB_CRH    (*(volatile unsigned int*)0x40010C04)
#define GPIOB_IDR    (*(volatile unsigned int*)0x40010C08)

// Port A : configuration register low, input data register
#define GPIOA_CRL    (*(volatile unsigned int*)0x40010800)
#define GPIOA_IDR    (*(volatile unsigned int*)0x40010808)

// Port D : configuration register low, input data register
#define GPIOD_CRL    (*(volatile unsigned int*)0x40011400)
#define GPIOD_CRH    (*(volatile unsigned int*)0x40011404)

// Port D : ouput -> BRR : reset, BSRR : reset or set
#define GPIOD_BRR    (*(volatile unsigned int *) 0x40011414)
#define GPIOD_BSRR   (*(volatile unsigned int *) 0x40011410)

void delay() {
  int i;
  for (i = 0; i < 10000000; i++) {}
}

void forward() {
  GPIOD_BRR |=  0x100; //PD 8 reset  
  GPIOD_BSRR |= 0x200; //PD 9 set
}

void reverse() {
  GPIOD_BSRR |= 0x100; //PD 8 set    
  GPIOD_BRR |= 0x200; //PD 9 reset 
}

void stop() {
  GPIOD_BRR |= 0x100; //PD 8 
  GPIOD_BRR |= 0x200; //PD 9
}

int main(void)
{
  // Enable clock for GPIOA, GPIOB, GPIOC, and GPIOD
  RCC_APB2ENR |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);

  // PORT A 
  GPIOA_CRL &= ~0xF;
  GPIOA_CRL |= 0x8;

  // PORT B
  GPIOB_CRH &= ~0x00000F00;
  GPIOB_CRH |= 0x00000800;
  
  // PORT C - Input mode pull up, pull down
  GPIOC_CRL &= ~0x00FFFF00;
  GPIOC_CRL |= 0x00888800;    
  
  // PORT D - General purpose output push-pull down
  GPIOD_CRL &= ~0xFFFFFFFF;
  GPIOD_CRL |= 0x33333333;
  GPIOD_CRH &= ~0xFFFFFFFF;
  GPIOD_CRH |=  0x33333333;

  // Initialize Port D output
  GPIOD_BSRR &= 0x00000000;
  GPIOD_BRR &= 0x00000000;
  
  while (1)
  {
    if (!(GPIOC_IDR & (1 << 4)))  // Check if KEY1 (PC4) is pressed
    {
      forward();
    }

    if (!(GPIOB_IDR & (1 << 10)))  // Check if KEY2 (PB10) is pressed
    {
      reverse();
    }

    if (!(GPIOC_IDR & (1 << 13)))  // Check if KEY3 (PC13) is pressed
    {
      stop();
    }

    if (!(GPIOA_IDR & (1 << 0)))  // Check if KEY4 (PA0) is pressed
    {
      forward();
      delay();
      stop();
      reverse();
      delay();
      stop();
    }
  }

  return 0;
}
