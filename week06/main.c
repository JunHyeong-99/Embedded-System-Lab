#include "stm32f10x.h"

void SysInit(void) {
    /* Set HSION bit */
    /* Internal Clock Enable */
    RCC->CR |= (uint32_t)0x00000001; //HSION

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR &= (uint32_t)0xF0FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    /* Reset PLL2ON and PLL3ON bits */
    RCC->CR &= (uint32_t)0xEBFFFFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x00FF0000;

    /* Reset CFGR2 register */
    RCC->CFGR2 = 0x00000000;
}

void SetSysClock(void) {
    volatile uint32_t StartUpCounter = 0, HSEStatus = 0;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    }
    else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        /* Flash 0 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;

//@TODO - 1 Set the clock, (//) 주석 표시를 없애고 틀린 값이 있다면 제대로 된 값으로 수정하시오 
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
        /* PCLK2 = HCLK / ?, use PPRE2 */
        /* 분주비를 2로 하여 PCLK2 (APB2 클럭)를 HCLK의 절반이 되도록 수정. */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV2;
        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

        /* Configure PLLs ------------------------------------------------------*/
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        /* 4MHz * PLL(7) = 28MHz */
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLMULL7);

        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL | RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        /* HSE(25MHz) / PREDIV2(5) * PLL2(8) / PREDIV1(10) = 4MHz */
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 | RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV10);
//@End of TODO - 1

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }
        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;
        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }
        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
        /* Select System Clock as output of MCO */
//@TODO - 2 Set the MCO port for system clock output
        /* MCO 설정 초기화 */
        RCC->CFGR &= ~(uint32_t)RCC_CFGR_MCO;
        /* MCO 포트가 SYSCLK(시스템 클럭) 출력하도록 설정 */
        RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
//@End of TODO - 2
    }
    else {
        /* If HSE fails to start-up, the application will have wrong clock
        configuration. User can add here some code to deal with this error */
    }
}

void RCC_Enable(void) {
//@TODO - 3 RCC Setting
    /*---------------------------- RCC Configuration -----------------------------*/
    /* GPIO RCC Enable  */
    /* UART Tx, Rx, MCO port */
    /* UART Tx, Rx, MCO 포트가 위치한 GPIOA 포트 클럭 활성화 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    /* USART RCC Enable */
    /* UART 통신을 위해 USART1 클럭 활성화 */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    /* User S1 버튼이 위치한 GPIOC 포트 클럭 활성화 */
    RCC->APB2ENR |= (uint32_t)RCC_APB2ENR_IOPCEN;
}

void PortConfiguration(void) {
//@TODO - 4 GPIO Configuration
    /* Reset(Clear) Port A CRH - MCO, USART1 TX,RX*/
    GPIOA->CRH &= ~(
       (GPIO_CRH_CNF8 | GPIO_CRH_MODE8) |
       (GPIO_CRH_CNF9 | GPIO_CRH_MODE9) |
       (GPIO_CRH_CNF10 | GPIO_CRH_MODE10)
   );
    /* MCO Pin Configuration */
    /* MCO(PA8) 핀을 Alternative Function push-pull 모드, 출력속도 최대속도로 설정 */
    GPIOA->CRH |= (GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8);
    /* USART Pin Configuration */
    /* TX(PA9) 핀을 Alternative Function push-pull 모드, 출력속도 최대속도로 설정 */
    /* RX(PA10) 핀을 Alternate Function floating input 모드, 입력 모드로 설정 */
    GPIOA->CRH |=  ((GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9) | (GPIO_CRH_CNF10_1));
    
    /* Reset(Clear) Port C CRL - User S1 Button */
    /* User S1 버튼(PC4) 핀 설정 초기화 */
    GPIOC->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
    /* User S1 Button Configuration */
    /* User S1 버튼(PC4) 핀을 input floating mode로 설정 */
    GPIOC->CRL |= GPIO_CRL_CNF4_1;
}

void UartInit(void) {
    /*---------------------------- USART CR1 Configuration -----------------------*/
    /* Clear M, PCE, PS, TE and RE bits */
    USART1->CR1 &= ~(uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE);
    /* Configure the USART Word Length, Parity and mode ----------------------- */
    /* Set the M bits according to USART_WordLength value */
//@TODO - 6: WordLength : 8bit
    
    /* Set PCE and PS bits according to USART_Parity value */
//@TODO - 7: Parity : None
    
    /* Set TE and RE bits according to USART_Mode value */
//@TODO - 8: Enable Tx and Rx
    /* TE, RE비트를 활성화 하여 Tx, Rx를 활성화 */
    USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    /*---------------------------- USART CR2 Configuration -----------------------*/
    /* Clear STOP[13:12] bits */
    USART1->CR2 &= ~(uint32_t)(USART_CR2_STOP);
    /* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit ------------*/
    USART1->CR2 &= ~(uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN);
    /* Set STOP[13:12] bits according to USART_StopBits value */
//@TODO - 9: Stop bit : 1bit
    

    /*---------------------------- USART CR3 Configuration -----------------------*/
    /* Clear CTSE and RTSE bits */
    USART1->CR3 &= ~(uint32_t)(USART_CR3_CTSE | USART_CR3_RTSE);
    /* Configure the USART HFC -------------------------------------------------*/
    /* Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
//@TODO - 10: CTS, RTS : disable


    /*---------------------------- USART BRR Configuration -----------------------*/
    /* Configure the USART Baud Rate -------------------------------------------*/
    /* Determine the integer part */
    /* Determine the fractional part */
//@TODO - 11: Calculate & configure BRR
    USART1->BRR |= 0x1E6;
    /* 14,000,000 / (28800 * 16) = 30.3819 */
    /* Mantisa : 30 = 0x1E */
    /* Fraction : 0.3819 * 16 = 6 = 0x6 */
    /* BRR = 0x1E6 */

    /*---------------------------- USART Enable ----------------------------------*/
    /* USART Enable Configuration */
//@TODO - 12: Enable UART (UE)
    /* UE 비트를 설정하여 UART를 활성화 */
    USART1->CR1 |= USART_CR1_UE;
}

void delay(void){
    int i = 0;
    for(i=0;i<1000000;i++);
}

void SendData(uint16_t data) {
    /* Transmit Data */
   USART1->DR = data;

   /* Wait till TC is set */
   while ((USART1->SR & USART_SR_TC) == 0);
}

int main() {
   int i;
   char msg[] = "Hello Team04\r\n";
   
    SysInit();
    SetSysClock();
    RCC_Enable();
    PortConfiguration();
    UartInit();
    
    // if you need, init pin values here
    
    
    while (1) {
      //@TODO - 13: Send the message when button is pressed
      if(!(GPIOC -> IDR & GPIO_IDR_IDR4)){
            for(i = 0; i < 14; i++) {
                SendData(msg[i]);
           }
        }
   }

}// end main
