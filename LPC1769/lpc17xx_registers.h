// lpc17xx_registers.h

#ifndef __LPC17XX_REGISTERS_H__
#define __LPC17XX_REGISTERS_H__

// UART registers
#define U0RBR LPC_UART0->RBR
#define U0THR LPC_UART0->THR
#define U0DLL LPC_UART0->DLL
#define U0DLM LPC_UART0->DLM
#define U0IER LPC_UART0->IER
#define U0IIR LPC_UART0->IIR
#define U0FCR LPC_UART0->FCR
#define U0LCR LPC_UART0->LCR
#define U0LSR LPC_UART0->LSR
#define U0SCR LPC_UART0->SCR
#define U0ACR LPC_UART0->ACR
#define U0ICR LPC_UART0->ICR
#define U0FDR LPC_UART0->FDR
#define U0TER LPC_UART0->TER
#define U0FIFOLVL LPC_UART0->FIFOLVL

#define U1RBR LPC_UART1->RBR
#define U1THR LPC_UART1->THR
#define U1DLL LPC_UART1->DLL
#define U1DLM LPC_UART1->DLM
#define U1IER LPC_UART1->IER
#define U1IIR LPC_UART1->IIR
#define U1FCR LPC_UART1->FCR
#define U1LCR LPC_UART1->LCR
#define U1MCR LPC_UART1->MCR
#define U1LSR LPC_UART1->LSR
#define U1MSR LPC_UART1->MSR
#define U1SCR LPC_UART1->SCR
#define U1ACR LPC_UART1->ACR
#define U1FDR LPC_UART1->FDR
#define U1TER LPC_UART1->TER
#define U1RS485CTRL LPC_UART1->RS485CTRL
#define U1ADRMATCH LPC_UART1->ADRMATCH
#define U1RS485DLY LPC_UART1->RS485DLY
#define U1FIFOLVL LPC_UART1->FIFOLVL

#define U2RBR LPC_UART2->RBR
#define U2THR LPC_UART2->THR
#define U2DLL LPC_UART2->DLL
#define U2DLM LPC_UART2->DLM
#define U2IER LPC_UART2->IER
#define U2IIR LPC_UART2->IIR
#define U2FCR LPC_UART2->FCR
#define U2LCR LPC_UART2->LCR
#define U2LSR LPC_UART2->LSR
#define U2SCR LPC_UART2->SCR
#define U2ACR LPC_UART2->ACR
#define U2ICR LPC_UART2->ICR
#define U2FDR LPC_UART2->FDR
#define U2TER LPC_UART2->TER
#define U2FIFOLVL LPC_UART2->FIFOLVL

#define U3RBR LPC_UART3->RBR
#define U3THR LPC_UART3->THR
#define U3DLL LPC_UART3->DLL
#define U3DLM LPC_UART3->DLM
#define U3IER LPC_UART3->IER
#define U3IIR LPC_UART3->IIR
#define U3FCR LPC_UART3->FCR
#define U3LCR LPC_UART3->LCR
#define U3LSR LPC_UART3->LSR
#define U3SCR LPC_UART3->SCR
#define U3ACR LPC_UART3->ACR
#define U3ICR LPC_UART3->ICR
#define U3FDR LPC_UART3->FDR
#define U3TER LPC_UART3->TER
#define U3FIFOLVL LPC_UART3->FIFOLVL


// power control bits in PCONP register
#define PCONP LPC_SC->PCONP

#define PCONP_TIMER0    (1 << 1)
#define PCONP_TIMER1    (1 << 2)
#define PCONP_UART0        (1 << 3)
#define PCONP_UART1        (1 << 4)
#define PCONP_PWM1        (1 << 6)
#define PCONP_I2C0        (1 << 7)
#define PCONP_SPI0        (1 << 8)
#define PCONP_RTC        (1 << 9)
#define PCONP_SSP1        (1 << 10)
#define PCONP_AD0        (1 << 12)
#define PCONP_CAN1        (1 << 13)
#define PCONP_CAN2        (1 << 14)
#define PCONP_GPIO        (1 << 15)
#define PCONP_RIT        (1 << 16)
#define PCONP_MCPWM        (1 << 17)
#define PCONP_QEI        (1 << 18)
#define PCONP_I2C1        (1 << 19)
#define PCONP_SSP0        (1 << 21)
#define PCONP_TIMER2        (1 << 22)
#define PCONP_TIMER3        (1 << 23)
#define PCONP_UART2        (1 << 24)
#define PCONP_UART3        (1 << 25)
#define PCONP_I2C2        (1 << 26)
#define PCONP_I2S        (1 << 27)
#define PCONP_GPDMA        (1 << 29)
#define PCONP_NET        (1 << 30)
#define PCONP_USB        (1 << 31)

#define PCONP_MASK (PCONP_TIMER0 | PCONP_TIMER1 | PCONP_UART0 | PCONP_UART1 | PCONP_PWM1 | \
    PCONP_I2C0 | PCONP_SPI0 | PCONP_RTC | PCONP_SSP1 | PCONP_AD0 | PCONP_CAN1 | PCONP_CAN2 | \
    PCONP_RIT | PCONP_MCPWM | PCONP_QEI | PCONP_I2C1 | PCONP_SSP0 | PCONP_TIMER2 | PCONP_TIMER3 | \
    PCONP_UART2 | PCONP_UART3 | PCONP_I2C2 | PCONP_I2S | PCONP_GPDMA | PCONP_NET | PCONP_USB | PCONP_GPIO)


// SSP Registers
#define SSP0CR0          LPC_SSP0->CR0        /* Control Register 0 */
#define SSP0CR1          LPC_SSP0->CR1        /* Control Register 1 */
#define SSP0DR           LPC_SSP0->DR         /* Data Register */
#define SSP0SR           LPC_SSP0->SR         /* Status Register */
#define SSP0CPSR         LPC_SSP0->CPSR       /* Clock Prescale Register */
#define SSP0IMSC         LPC_SSP0->IMSC       /* Interrupt Mask Set and Clear Register */
#define SSP0RIS          LPC_SSP0->RIS        /* Raw Interrupt Status Register */
#define SSP0MIS          LPC_SSP0->MIS        /* Masked Interrupt Status Register */
#define SSP0ICR          LPC_SSP0->ICR        /* SSP0ICR Interrupt Clear Register */
#define SSP0DMACR        LPC_SSP0->DMACR      /* SSP0DMACR DMA Control Register */

#define SSP1CR0          LPC_SSP1->CR0        /* Control Register 0 */
#define SSP1CR1          LPC_SSP1->CR1        /* Control Register 1 */
#define SSP1DR           LPC_SSP1->DR         /* Data Register */
#define SSP1SR           LPC_SSP1->SR         /* Status Register */
#define SSP1CPSR         LPC_SSP1->CPSR       /* Clock Prescale Register */
#define SSP1IMSC         LPC_SSP1->IMSC       /* Interrupt Mask Set and Clear Register */
#define SSP1RIS          LPC_SSP1->RIS        /* Raw Interrupt Status Register */
#define SSP1MIS          LPC_SSP1->MIS        /* Masked Interrupt Status Register */
#define SSP1ICR          LPC_SSP1->ICR        /* SSPICR Interrupt Clear Register */
#define SSP1DMACR        LPC_SSP1->DMACR      /* SSP1DMACR DMA Control Register */

// SSPCR0 bits
#define SSP_DSS     0x0f   // Data Size Select.
#define SSP_FRF     0x30   // Frame Format.
#define SSP_FRF_SPI 0x00   // Frame Format SPI
#define SSP_FRF_SSI 0x10   // Frame Format SSI
#define SSP_FRF_MWI 0x20   // Frame Format Microwire
#define SSP_CPOL    0x40   // Clock Out Polarity.
#define SSP_CPHA    0x0080 // Clock Out Phase.
#define SSP_SCR     0xff00 // Serial Clock Rate.

// SSPCR1 bits
#define SSP_LBM     0x01   // Loop Back Mode.
#define SSP_SSE     0x02   // SSP Enable.
#define SSP_MS      0x04   // Master/Slave Mode.
#define SSP_SOD     0x08   // Slave Output Disable.

// SSPSR bits
#define SSP_TFE     0x01   // Transmit FIFO Empty.
#define SSP_TNF     0x02   // Transmit FIFO Not Full.
#define SSP_RNE     0x04   // Receive FIFO Not Empty.
#define SSP_RFF     0x08   // Receive FIFO Full.
#define SSP_BSY     0x10   // Busy.

// SSPCPSR bits
#define SSP_CPSDVSR 0x00ff

// SSPIMSC bits
#define SSP_RORIM   0x01   // Receive Overrun
#define SSP_RTIM    0x02   // Receive Timeout
#define SSP_RXIM    0x04   // Rx FIFO is at least half full
#define SSP_TXIM    0x08   // Tx FIFO is at least half empty.

// SSPRIS bits
#define SSP_RORRIS  0x01   // 1 if another frame was completely received while the RxFIFO was full
#define SSP_RTRIS   0x02   // 1 if when there is a Receive Timeout condition.
#define SSP_RXRIS   0x04   // 1 if the Rx FIFO is at least half full.
#define SSP_TXRIS   0x08   // 1 if the Tx FIFO is at least half empty.

// SSPMIS bits
#define SSP_RORMIS  0x01   // 1 if another frame was completely received while the RxFIFO was full
#define SSP_RTMIS   0x02   // 1 when there is a Receive Timeout condition
#define SSP_RXMIS   0x04   // 1 if the Rx FIFO is at least half full,
#define SSP_TXMIS   0x08   // 1 if the Tx FIFO is at least half empty

// SSPICR bits
#define SSP_RORIC   0x01   // clears the “frame was received when RxFIFO was full” interrupt.
#define SSP_RTIC    0x02   // clears the Receive Timeout interrupt.

// SSPDMACR bits
#define SSP_RXDMAE 0x01     // enable DMA for the receive FIFO
#define SSP_TXDMAE 0x02     // enable DMA for the transmit FIFO


// USB registers
/* USB Device Interrupt Registers */
#define USBIntSt           LPC_SC->USBIntSt
#define USBDevIntSt        LPC_USB->USBDevIntSt
#define USBDevIntEn        LPC_USB->USBDevIntEn
#define USBDevIntClr       LPC_USB->USBDevIntClr
#define USBDevIntSet       LPC_USB->USBDevIntSet
#define USBDevIntPri       LPC_USB->USBDevIntPri

/* USB Device Endpoint Interrupt Registers */
#define USBEpIntSt         LPC_USB->USBEpIntSt
#define USBEpIntEn         LPC_USB->USBEpIntEn
#define USBEpIntClr        LPC_USB->USBEpIntClr
#define USBEpIntSet        LPC_USB->USBEpIntSet
#define USBEpIntPri        LPC_USB->USBEpIntPri

/* USB Device Endpoint Realization Registers */
#define USBReEp            LPC_USB->USBReEp
#define USBEpInd           LPC_USB->USBEpInd
#define USBMaxPSize        LPC_USB->USBMaxPSize

/* USB Device Data Transfer Registers */
#define USBRxData          LPC_USB->USBRxData
#define USBRxPLen          LPC_USB->USBRxPLen
#define USBTxData          LPC_USB->USBTxData
#define USBTxPLen          LPC_USB->USBTxPLen
#define USBCtrl            LPC_USB->USBCtrl

/* USB SIE Command Reagisters */
#define USBCmdCode         LPC_USB->USBCmdCode
#define USBCmdData         LPC_USB->USBCmdData

/* USB Device DMA Registers */
#define USBDMARSt          LPC_USB->USBDMARSt
#define USBDMARClr         LPC_USB->USBDMARClr
#define USBDMARSet         LPC_USB->USBDMARSet
#define USBUDCAH           LPC_USB->USBUDCAH
#define USBEpDMASt         LPC_USB->USBEpDMASt
#define USBEpDMAEn         LPC_USB->USBEpDMAEn
#define USBEpDMADis        LPC_USB->USBEpDMADis
#define USBDMAIntSt        LPC_USB->USBDMAIntSt
#define USBDMAIntEn        LPC_USB->USBDMAIntEn
#define USBEoTIntSt        LPC_USB->USBEoTIntSt
#define USBEoTIntClr       LPC_USB->USBEoTIntClr
#define USBEoTIntSet       LPC_USB->USBEoTIntSet
#define USBNDDRIntSt       LPC_USB->USBNDDRIntSt
#define USBNDDRIntClr      LPC_USB->USBNDDRIntClr
#define USBNDDRIntSet      LPC_USB->USBNDDRIntSet
#define USBSysErrIntSt     LPC_USB->USBSysErrIntSt
#define USBSysErrIntClr    LPC_USB->USBSysErrIntClr
#define USBSysErrIntSet    LPC_USB->USBSysErrIntSet

/* USB Clock Control Registers */
#define USBClkCtrl         LPC_USB->USBClkCtrl
#define OTGClkCtrl         LPC_USB->OTGClkCtrl
#define USBClkSt           LPC_USB->USBClkSt
#define OTGClkSt           LPC_USB->OTGClkSt


// PINSEL registers
#define PINSEL0 LPC_PINCON->PINSEL0
#define PINSEL1 LPC_PINCON->PINSEL1
#define PINSEL2 LPC_PINCON->PINSEL2
#define PINSEL3 LPC_PINCON->PINSEL3
#define PINSEL4 LPC_PINCON->PINSEL4
#define PINSEL5 LPC_PINCON->PINSEL5
#define PINSEL6 LPC_PINCON->PINSEL6
#define PINSEL7 LPC_PINCON->PINSEL7
#define PINSEL8 LPC_PINCON->PINSEL8
#define PINSEL9 LPC_PINCON->PINSEL9
#define PINSEL10 LPC_PINCON->PINSEL10

// PINMODE registers
#define PINMODE0 LPC_PINCON->PINMODE0
#define PINMODE1 LPC_PINCON->PINMODE1
#define PINMODE2 LPC_PINCON->PINMODE2
#define PINMODE3 LPC_PINCON->PINMODE3
#define PINMODE4 LPC_PINCON->PINMODE4
#define PINMODE5 LPC_PINCON->PINMODE5
#define PINMODE6 LPC_PINCON->PINMODE6
#define PINMODE7 LPC_PINCON->PINMODE7
#define PINMODE8 LPC_PINCON->PINMODE8
#define PINMODE9 LPC_PINCON->PINMODE9

// PINMODE_OD registers
#define PINMODE_OD0 LPC_PINCON->PINMODE_OD0
#define PINMODE_OD1 LPC_PINCON->PINMODE_OD1
#define PINMODE_OD2 LPC_PINCON->PINMODE_OD2
#define PINMODE_OD3 LPC_PINCON->PINMODE_OD3
#define PINMODE_OD4 LPC_PINCON->PINMODE_OD4

// PLL1 registers
#define PLL1CON LPC_SC->PLL1CON
#define PLL1CFG LPC_SC->PLL1CFG
#define PLL1STAT LPC_SC->PLL1STAT
#define PLL1FEED LPC_SC->PLL1FEED


// GPIO registers
#define P0PIN LPC_GPIO0->FIOPIN
#define P1PIN LPC_GPIO1->FIOPIN
#define P2PIN LPC_GPIO2->FIOPIN
#define P3PIN LPC_GPIO3->FIOPIN
#define P4PIN LPC_GPIO4->FIOPIN
#define P0DIR LPC_GPIO0->FIODIR
#define P1DIR LPC_GPIO1->FIODIR
#define P2DIR LPC_GPIO2->FIODIR
#define P3DIR LPC_GPIO3->FIODIR
#define P4DIR LPC_GPIO4->FIODIR
#define P0CLR LPC_GPIO0->FIOCLR
#define P1CLR LPC_GPIO1->FIOCLR
#define P2CLR LPC_GPIO2->FIOCLR
#define P3CLR LPC_GPIO3->FIOCLR
#define P4CLR LPC_GPIO4->FIOCLR
#define P0SET LPC_GPIO0->FIOSET
#define P1SET LPC_GPIO1->FIOSET
#define P2SET LPC_GPIO2->FIOSET
#define P3SET LPC_GPIO3->FIOSET
#define P4SET LPC_GPIO4->FIOSET

// timer0,1,2,3
#define T0IR    LPC_TIM0->IR
#define T0TCR   LPC_TIM0->TCR
#define T0TC    LPC_TIM0->TC
#define T0PR    LPC_TIM0->PR
#define T0PC    LPC_TIM0->PC
#define T0MCR   LPC_TIM0->MCR
#define T0MR0   LPC_TIM0->MR0
#define T0MR1   LPC_TIM0->MR1
#define T0MR2   LPC_TIM0->MR2
#define T0MR3   LPC_TIM0->MR3
#define T0CCR   LPC_TIM0->CCR
#define T0CR0   LPC_TIM0->CR0
#define T0CR1   LPC_TIM0->CR1
#define T0EMR   LPC_TIM0->EMR
#define T0CTCR  LPC_TIM0->CTCR

#define T1IR    LPC_TIM1->IR
#define T1TCR   LPC_TIM1->TCR
#define T1TC    LPC_TIM1->TC
#define T1PR    LPC_TIM1->PR
#define T1PC    LPC_TIM1->PC
#define T1MCR   LPC_TIM1->MCR
#define T1MR0   LPC_TIM1->MR0
#define T1MR1   LPC_TIM1->MR1
#define T1MR2   LPC_TIM1->MR2
#define T1MR3   LPC_TIM1->MR3
#define T1CCR   LPC_TIM1->CCR
#define T1CR0   LPC_TIM1->CR0
#define T1CR1   LPC_TIM1->CR1
#define T1EMR   LPC_TIM1->EMR
#define T1CTCR  LPC_TIM1->CTCR

#define T2IR    LPC_TIM2->IR
#define T2TCR   LPC_TIM2->TCR
#define T2TC    LPC_TIM2->TC
#define T2PR    LPC_TIM2->PR
#define T2PC    LPC_TIM2->PC
#define T2MCR   LPC_TIM2->MCR
#define T2MR0   LPC_TIM2->MR0
#define T2MR1   LPC_TIM2->MR1
#define T2MR2   LPC_TIM2->MR2
#define T2MR3   LPC_TIM2->MR3
#define T2CCR   LPC_TIM2->CCR
#define T2CR0   LPC_TIM2->CR0
#define T2CR1   LPC_TIM2->CR1
#define T2EMR   LPC_TIM2->EMR
#define T2CTCR  LPC_TIM2->CTCR

#define T3IR    LPC_TIM3->IR
#define T3TCR   LPC_TIM3->TCR
#define T3TC    LPC_TIM3->TC
#define T3PR    LPC_TIM3->PR
#define T3PC    LPC_TIM3->PC
#define T3MCR   LPC_TIM3->MCR
#define T3MR0   LPC_TIM3->MR0
#define T3MR1   LPC_TIM3->MR1
#define T3MR2   LPC_TIM3->MR2
#define T3MR3   LPC_TIM3->MR3
#define T3CCR   LPC_TIM3->CCR
#define T3CR0   LPC_TIM3->CR0
#define T3CR1   LPC_TIM3->CR1
#define T3EMR   LPC_TIM3->EMR
#define T3CTCR  LPC_TIM3->CTCR

// PWM regs
#define PWMIR    LPC_PWM1->IR
#define PWMTCR   LPC_PWM1->TCR
#define PWMTC    LPC_PWM1->TC
#define PWMPR    LPC_PWM1->PR
#define PWMPC    LPC_PWM1->PC
#define PWMMCR   LPC_PWM1->MCR
#define PWMMR0   LPC_PWM1->MR0
#define PWMMR1   LPC_PWM1->MR1
#define PWMMR2   LPC_PWM1->MR2
#define PWMMR3   LPC_PWM1->MR3
#define PWMMR4   LPC_PWM1->MR4
#define PWMMR5   LPC_PWM1->MR5
#define PWMMR6   LPC_PWM1->MR6
#define PWMCCR   LPC_PWM1->CCR
#define PWMCR0   LPC_PWM1->CR0
#define PWMCR1   LPC_PWM1->CR1
#define PWMCR2   LPC_PWM1->CR2
#define PWMCR3   LPC_PWM1->CR3
#define PWMPCR   LPC_PWM1->PCR
#define PWMLER   LPC_PWM1->LER
#define PWMCTCR  LPC_PWM1->CTCR

// Timer Interrupt Register Bit Definitions
#define TIR_MR0I    (1 << 0)            // Interrupt flag for match channel 0
#define TIR_MR1I    (1 << 1)            // Interrupt flag for match channel 1
#define TIR_MR2I    (1 << 2)            // Interrupt flag for match channel 2
#define TIR_MR3I    (1 << 3)            // Interrupt flag for match channel 3
#define TIR_CR0I    (1 << 4)            // Interrupt flag for capture channel 0 event
#define TIR_CR1I    (1 << 5)            // Interrupt flag for capture channel 1 event
#define TIR_CR2I    (1 << 6)            // Interrupt flag for capture channel 2 event
#define TIR_CR3I    (1 << 7)            // Interrupt flag for capture channel 3 event

// PWM Interrupt Register Bit Definitions
#define PWMIR_MR0I  (1 << 0)            // Interrupt flag for match channel 0
#define PWMIR_MR1I  (1 << 1)            // Interrupt flag for match channel 1
#define PWMIR_MR2I  (1 << 2)            // Interrupt flag for match channel 2
#define PWMIR_MR3I  (1 << 3)            // Interrupt flag for match channel 3
#define PWMIR_MR4I  (1 << 8)            // Interrupt flag for match channel 4
#define PWMIR_MR5I  (1 << 9)            // Interrupt flag for match channel 5
#define PWMIR_MR6I  (1 << 10)           // Interrupt flag for match channel 6
#define PWMIR_MASK  (0x070F)

// Timer Control Register Bit Definitions
#define TCR_ENABLE  (1 << 0)
#define TCR_RESET   (1 << 1)

// PWM Control Register Bit Definitions
#define PWMCR_ENABLE (1 << 0)
#define PWMCR_RESET (1 << 1)

// Timer Match Control Register Bit Definitions
#define TMCR_MR0_I  (1 << 0)            // Enable Interrupt when MR0 matches TC
#define TMCR_MR0_R  (1 << 1)            // Enable Reset of TC upon MR0 match
#define TMCR_MR0_S  (1 << 2)            // Enable Stop of TC upon MR0 match
#define TMCR_MR1_I  (1 << 3)            // Enable Interrupt when MR1 matches TC
#define TMCR_MR1_R  (1 << 4)            // Enable Reset of TC upon MR1 match
#define TMCR_MR1_S  (1 << 5)            // Enable Stop of TC upon MR1 match
#define TMCR_MR2_I  (1 << 6)            // Enable Interrupt when MR2 matches TC
#define TMCR_MR2_R  (1 << 7)            // Enable Reset of TC upon MR2 match
#define TMCR_MR2_S  (1 << 8)            // Enable Stop of TC upon MR2 match
#define TMCR_MR3_I  (1 << 9)            // Enable Interrupt when MR3 matches TC
#define TMCR_MR3_R  (1 << 10)           // Enable Reset of TC upon MR3 match
#define TMCR_MR3_S  (1 << 11)           // Enable Stop of TC upon MR3 match

// Timer Capture Control Register Bit Definitions
#define TCCR_CR0_R (1 << 0)            // Enable Rising edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_F (1 << 1)            // Enable Falling edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_I (1 << 2)            // Enable Interrupt on load of CR0
#define TCCR_CR1_R (1 << 3)            // Enable Rising edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_F (1 << 4)            // Enable Falling edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_I (1 << 5)            // Enable Interrupt on load of CR1
#define TCCR_CR2_R (1 << 6)            // Enable Rising edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_F (1 << 7)            // Enable Falling edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_I (1 << 8)            // Enable Interrupt on load of CR2
#define TCCR_CR3_R (1 << 9)            // Enable Rising edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_F (1 << 10)           // Enable Falling edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_I (1 << 11)           // Enable Interrupt on load of CR3


#endif // __LPC17XX_REGISTERS_H__
