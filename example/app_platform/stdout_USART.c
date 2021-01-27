/*-----------------------------------------------------------------------------
 * Name:    stdout_USART.c
 * Purpose: STDOUT USART Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2015 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
 
/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include <string.h>
#include <ARMCM4_FP.h>

enum  {
/* ----------------------  CMSDK_CM4 Specific Interrupt Numbers  ------------------ */
  UART0RX_IRQn                  =   0,      /* UART 0 receive interrupt */
  UART0TX_IRQn                  =   1,      /* UART 0 transmit interrupt */
  UART1RX_IRQn                  =   2,      /* UART 1 receive interrupt */
  UART1TX_IRQn                  =   3,      /* UART 1 transmit interrupt */
  UART2RX_IRQn                  =   4,      /* UART 2 receive interrupt */
  UART2TX_IRQn                  =   5,      /* UART 2 transmit interrupt */
  GPIO0ALL_IRQn                 =   6,      /* GPIO 0 combined interrupt */
  GPIO1ALL_IRQn                 =   7,      /* GPIO 1 combined interrupt */
  TIMER0_IRQn                   =   8,      /* Timer 0 interrupt */
  TIMER1_IRQn                   =   9,      /* Timer 1 interrupt */
  DUALTIMER_IRQn                =  10,      /* Dual Timer interrupt */
  SPI_0_1_IRQn                  =  11,      /* SPI #0, #1 interrupt */
  UART_0_1_2_OVF_IRQn           =  12,      /* UART overflow (0, 1 & 2) interrupt */
  ETHERNET_IRQn                 =  13,      /* Ethernet interrupt */
  I2S_IRQn                      =  14,      /* Audio I2S interrupt */
  TOUCHSCREEN_IRQn              =  15,      /* Touch Screen interrupt */
  GPIO2_IRQn                    =  16,      /* GPIO 2 combined interrupt */
  GPIO3_IRQn                    =  17,      /* GPIO 3 combined interrupt */
  UART3RX_IRQn                  =  18,      /* UART 3 receive interrupt */
  UART3TX_IRQn                  =  19,      /* UART 3 transmit interrupt */
  UART4RX_IRQn                  =  20,      /* UART 4 receive interrupt */
  UART4TX_IRQn                  =  21,      /* UART 4 transmit interrupt */
  SPI_2_IRQn                    =  22,      /* SPI #2 interrupt */
  SPI_3_4_IRQn                  =  23,      /* SPI #3, SPI #4 interrupt */
  GPIO0_0_IRQn                  =  24,      /* GPIO 0 individual interrupt ( 0) */
  GPIO0_1_IRQn                  =  25,      /* GPIO 0 individual interrupt ( 1) */
  GPIO0_2_IRQn                  =  26,      /* GPIO 0 individual interrupt ( 2) */
  GPIO0_3_IRQn                  =  27,      /* GPIO 0 individual interrupt ( 3) */
  GPIO0_4_IRQn                  =  28,      /* GPIO 0 individual interrupt ( 4) */
  GPIO0_5_IRQn                  =  29,      /* GPIO 0 individual interrupt ( 5) */
  GPIO0_6_IRQn                  =  30,      /* GPIO 0 individual interrupt ( 6) */
  GPIO0_7_IRQn                  =  31       /* GPIO 0 individual interrupt ( 7) */
};

/*============================ MACROS ========================================*/

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  __IOM  uint32_t  DATA;                     /* Offset: 0x000 (R/W) Data Register    */
  __IOM  uint32_t  STATE;                    /* Offset: 0x004 (R/W) Status Register  */
  __IOM  uint32_t  CTRL;                     /* Offset: 0x008 (R/W) Control Register */
  union {
    __IM   uint32_t  INTSTATUS;              /* Offset: 0x00C (R/ ) Interrupt Status Register */
    __OM   uint32_t  INTCLEAR;               /* Offset: 0x00C ( /W) Interrupt Clear Register  */
    };
  __IOM  uint32_t  BAUDDIV;                  /* Offset: 0x010 (R/W) Baudrate Divider Register */

} CMSDK_UART_TypeDef;

/* CMSDK_UART DATA Register Definitions */
#define CMSDK_UART_DATA_Pos               0                                                  /* CMSDK_UART_DATA_Pos: DATA Position */
#define CMSDK_UART_DATA_Msk              (0xFFUL /*<< CMSDK_UART_DATA_Pos*/)                 /* CMSDK_UART DATA: DATA Mask */

/* CMSDK_UART STATE Register Definitions */
#define CMSDK_UART_STATE_RXOR_Pos         3                                                  /* CMSDK_UART STATE: RXOR Position */
#define CMSDK_UART_STATE_RXOR_Msk        (0x1UL << CMSDK_UART_STATE_RXOR_Pos)                /* CMSDK_UART STATE: RXOR Mask */

#define CMSDK_UART_STATE_TXOR_Pos         2                                                  /* CMSDK_UART STATE: TXOR Position */
#define CMSDK_UART_STATE_TXOR_Msk        (0x1UL << CMSDK_UART_STATE_TXOR_Pos)                /* CMSDK_UART STATE: TXOR Mask */

#define CMSDK_UART_STATE_RXBF_Pos         1                                                  /* CMSDK_UART STATE: RXBF Position */
#define CMSDK_UART_STATE_RXBF_Msk        (0x1UL << CMSDK_UART_STATE_RXBF_Pos)                /* CMSDK_UART STATE: RXBF Mask */

#define CMSDK_UART_STATE_TXBF_Pos         0                                                  /* CMSDK_UART STATE: TXBF Position */
#define CMSDK_UART_STATE_TXBF_Msk        (0x1UL /*<< CMSDK_UART_STATE_TXBF_Pos*/)            /* CMSDK_UART STATE: TXBF Mask */

/* CMSDK_UART CTRL Register Definitions */
#define CMSDK_UART_CTRL_HSTM_Pos          6                                                  /* CMSDK_UART CTRL: HSTM Position */
#define CMSDK_UART_CTRL_HSTM_Msk         (0x01UL << CMSDK_UART_CTRL_HSTM_Pos)                /* CMSDK_UART CTRL: HSTM Mask */

#define CMSDK_UART_CTRL_RXORIRQEN_Pos     5                                                  /* CMSDK_UART CTRL: RXORIRQEN Position */
#define CMSDK_UART_CTRL_RXORIRQEN_Msk    (0x01UL << CMSDK_UART_CTRL_RXORIRQEN_Pos)           /* CMSDK_UART CTRL: RXORIRQEN Mask */

#define CMSDK_UART_CTRL_TXORIRQEN_Pos     4                                                  /* CMSDK_UART CTRL: TXORIRQEN Position */
#define CMSDK_UART_CTRL_TXORIRQEN_Msk    (0x01UL << CMSDK_UART_CTRL_TXORIRQEN_Pos)           /* CMSDK_UART CTRL: TXORIRQEN Mask */

#define CMSDK_UART_CTRL_RXIRQEN_Pos       3                                                  /* CMSDK_UART CTRL: RXIRQEN Position */
#define CMSDK_UART_CTRL_RXIRQEN_Msk      (0x01UL << CMSDK_UART_CTRL_RXIRQEN_Pos)             /* CMSDK_UART CTRL: RXIRQEN Mask */

#define CMSDK_UART_CTRL_TXIRQEN_Pos       2                                                  /* CMSDK_UART CTRL: TXIRQEN Position */
#define CMSDK_UART_CTRL_TXIRQEN_Msk      (0x01UL << CMSDK_UART_CTRL_TXIRQEN_Pos)             /* CMSDK_UART CTRL: TXIRQEN Mask */

#define CMSDK_UART_CTRL_RXEN_Pos          1                                                  /* CMSDK_UART CTRL: RXEN Position */
#define CMSDK_UART_CTRL_RXEN_Msk         (0x01UL << CMSDK_UART_CTRL_RXEN_Pos)                /* CMSDK_UART CTRL: RXEN Mask */

#define CMSDK_UART_CTRL_TXEN_Pos          0                                                  /* CMSDK_UART CTRL: TXEN Position */
#define CMSDK_UART_CTRL_TXEN_Msk         (0x01UL /*<< CMSDK_UART_CTRL_TXEN_Pos*/)            /* CMSDK_UART CTRL: TXEN Mask */

#define CMSDK_UART_INTSTATUS_RXORIRQ_Pos  3                                                  /* CMSDK_UART CTRL: RXORIRQ Position */
#define CMSDK_UART_CTRL_RXORIRQ_Msk      (0x01UL << CMSDK_UART_INTSTATUS_RXORIRQ_Pos)        /* CMSDK_UART CTRL: RXORIRQ Mask */

#define CMSDK_UART_CTRL_TXORIRQ_Pos       2                                                  /* CMSDK_UART CTRL: TXORIRQ Position */
#define CMSDK_UART_CTRL_TXORIRQ_Msk      (0x01UL << CMSDK_UART_CTRL_TXORIRQ_Pos)             /* CMSDK_UART CTRL: TXORIRQ Mask */

#define CMSDK_UART_CTRL_RXIRQ_Pos         1                                                  /* CMSDK_UART CTRL: RXIRQ Position */
#define CMSDK_UART_CTRL_RXIRQ_Msk        (0x01UL << CMSDK_UART_CTRL_RXIRQ_Pos)               /* CMSDK_UART CTRL: RXIRQ Mask */

#define CMSDK_UART_CTRL_TXIRQ_Pos         0                                                  /* CMSDK_UART CTRL: TXIRQ Position */
#define CMSDK_UART_CTRL_TXIRQ_Msk        (0x01UL /*<< CMSDK_UART_CTRL_TXIRQ_Pos*/)           /* CMSDK_UART CTRL: TXIRQ Mask */

/* CMSDK_UART BAUDDIV Register Definitions */
#define CMSDK_UART_BAUDDIV_Pos            0                                                  /* CMSDK_UART BAUDDIV: BAUDDIV Position */
#define CMSDK_UART_BAUDDIV_Msk           (0xFFFFFUL /*<< CMSDK_UART_BAUDDIV_Pos*/)           /* CMSDK_UART BAUDDIV: BAUDDIV Mask */


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

//#define CMSDK_UART0_BASE_ADDRESS	(0x41303000ul)
#define CMSDK_UART0_BASE_ADDRESS	(0x40004000ul)
#define CMSDK_UART0             ((CMSDK_UART_TypeDef *) CMSDK_UART0_BASE_ADDRESS)

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
 
// <h>STDOUT USART Interface
 
//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM           0
// </h>


#ifndef USART_BAUDRATE
#   define USART_BAUDRATE          115200
#endif


#ifndef STREAM_IN_RCV_TIMEOUT
#   warning No defined macro STREAM_IN_RCV_TIMEOUT, default value 10 is used.
#   define STREAM_IN_RCV_TIMEOUT            (10)
#endif

#ifndef OUTPUT_STREAM_BLOCK_SIZE
#   warning No defined macro OUTPUT_STREAM_BLOCK_SIZE, default value 32 is used.
#   define OUTPUT_STREAM_BLOCK_SIZE         (32)
#endif
#ifndef OUTPUT_STREAM_BLOCK_COUNT  
#   warning No defined macro OUTPUT_STREAM_BLOCK_COUNT, default value 4 is used.
#   define OUTPUT_STREAM_BLOCK_COUNT        (4)
#endif

#ifndef INPUT_STREAM_BLOCK_SIZE
#   warning No defined macro INPUT_STREAM_BLOCK_SIZE, default value 32 is used.
#   define INPUT_STREAM_BLOCK_SIZE          (32)
#endif
#ifndef INPUT_STREAM_BLOCK_COUNT  
#   warning No defined macro INPUT_STREAM_BLOCK_COUNT, default value 8 is used.
#   define INPUT_STREAM_BLOCK_COUNT         (8)
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USE_SERIAL_PORT_INPUT_ADAPTER(__NUM)                          \
                                                                        \
void STREAM_IN_serial_port_enable_rx_cpl_interrupt(void)                \
{                                                                       \
    CMSDK_UART##__NUM->CTRL |= CMSDK_UART_CTRL_RXIRQEN_Msk;             \
}                                                                       \
                                                                        \
void STREAM_IN_serial_port_disable_rx_cpl_interrupt(void)               \
{                                                                       \
    CMSDK_UART##__NUM->CTRL &= ~CMSDK_UART_CTRL_RXIRQEN_Msk;            \
}                                                                       \
                                                                        \
uint8_t STREAM_IN_serial_port_get_byte(void)                            \
{                                                                       \
    return CMSDK_UART##__NUM->DATA;                                     \
}                                                                       \
/* this function is called instead of the original UART0RX_Handler() */ \
void USART##__NUM##_RX_CPL_Handler(void)                                \
{                                                                       \
    /*! clear interrupt flag */                                         \
    CMSDK_UART##__NUM->INTCLEAR = CMSDK_UART##__NUM->INTSTATUS;         \
    STREAM_IN_insert_serial_port_rx_cpl_event_handler();                \
}                           

  
    
#define __USE_SERIAL_PORT_OUTPUT_ADAPTER(__NUM)                         \
                                                                        \
void STREAM_OUT_serial_port_enable_tx_cpl_interrupt(void)               \
{                                                                       \
    CMSDK_UART##__NUM->CTRL |= CMSDK_UART_CTRL_TXIRQEN_Msk;             \
}                                                                       \
                                                                        \
void STREAM_OUT_serial_port_disbale_tx_cpl_interrupt(void)              \
{                                                                       \
    CMSDK_UART##__NUM->CTRL &= ~CMSDK_UART_CTRL_TXIRQEN_Msk;            \
}                                                                       \
                                                                        \
void STREAM_OUT_serial_port_fill_byte(uint8_t chByte)                   \
{                                                                       \
    CMSDK_UART##__NUM->DATA = chByte;                                   \
}                                                                       \
                                                                        \
/* this function is called instead of the original UART0TX_Handler() */ \
void USART##__NUM##_TX_CPL_Handler(void)                                \
{                                                                       \
    /*! clear interrupt flag  */                                        \
    CMSDK_UART##__NUM->INTCLEAR = CMSDK_UART##__NUM->INTSTATUS;         \
    /*! implement our own version of uart tx interrupt */               \
                                                                        \
    STREAM_OUT_insert_serial_port_tx_cpl_event_handler();               \
}


                 

#define USE_SERIAL_PORT_INPUT_ADAPTER(__NUM)                            \
            __USE_SERIAL_PORT_INPUT_ADAPTER(__NUM)

#define USE_SERIAL_PORT_OUTPUT_ADAPTER(__NUM)                           \
            __USE_SERIAL_PORT_OUTPUT_ADAPTER(__NUM) 

/*============================ TYPES =========================================*/ 

//! \note define a buffer for output stream
//! @{
DEF_OUTPUT_STREAM_BUFFER(       STREAM_OUT, OUTPUT_STREAM_BLOCK_SIZE)

END_DEF_OUTPUT_STREAM_BUFFER(   STREAM_OUT)

//! \note add an adapter for serial port
STREAM_OUT_SERIAL_PORT_ADAPTER( STREAM_OUT, OUTPUT_STREAM_BLOCK_COUNT)
//! @}

//! \note define a buffer for input stream
//! @{
DEF_INPUT_STREAM_BUFFER(STREAM_IN, INPUT_STREAM_BLOCK_SIZE)

END_DEF_INPUT_STREAM_BUFFER(STREAM_IN)

//! \note add an adapter for serial port
STREAM_IN_SERIAL_PORT_ADAPTER(STREAM_IN, INPUT_STREAM_BLOCK_COUNT)
//! @}
    

/*------------------------------------------------------------------------------*
 * Implement Serial Port input interfaces required by STREAM_OUT adapter        *
 *------------------------------------------------------------------------------*/
USE_SERIAL_PORT_INPUT_ADAPTER(USART_DRV_NUM)


/*------------------------------------------------------------------------------*
 * Implement Serial Port output interfaces required by STREAM_OUT adapter       *
 *------------------------------------------------------------------------------*/
USE_SERIAL_PORT_OUTPUT_ADAPTER(USART_DRV_NUM)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/




/*! \note initialize usart for stdout
 *  \param none
 *  \retval true    initialization succeeded.
 *  \retval false   initialization failed
 */  
bool stdout_init (void) 
{    
    STREAM_OUT_output_stream_adapter_init();
    STREAM_IN_input_stream_adapter_init();
    
    CMSDK_UART0->CTRL = 0;         /* Disable UART when changing configuration */
    CMSDK_UART0->BAUDDIV = 651;    /* 25MHz / 38400 = 651 */
    CMSDK_UART0->CTRL = CMSDK_UART_CTRL_TXEN_Msk    |
                        CMSDK_UART_CTRL_RXEN_Msk    | 
                        CMSDK_UART_CTRL_RXIRQEN_Msk;  
                            
    NVIC_ClearPendingIRQ(UART0RX_IRQn);
    NVIC_EnableIRQ(UART0RX_IRQn);
    NVIC_ClearPendingIRQ(UART0TX_IRQn);
    NVIC_EnableIRQ(UART0TX_IRQn);
    
    return true;
}
    
/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) 
{
    while(!STREAM_OUT.Stream.WriteByte(ch)) ;
    
    return ch;
}

int stdin_getchar (void)
{
    uint8_t chByte;
    while(!STREAM_IN.Stream.ReadByte(&chByte));
    
    return chByte;
}
