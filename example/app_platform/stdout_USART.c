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
#include "Driver_USART.h"
#include "Device.h"

/*============================ MACROS ========================================*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
 
// <h>STDOUT USART Interface
 
//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM           0
// </h>


#ifndef USART_BAUDRATE
#   define USART_BAUDRATE          115200
#endif

 
#define _USART_Driver_(n)  Driver_USART##n
#define  USART_Driver_(n) _USART_Driver_(n)

#define ptrUSART       (&USART_Driver_(USART_DRV_NUM))


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

/*============================ TYPES =========================================*/ 

DEF_OUTPUT_STREAM_BUFFER(STREAM_OUT, OUTPUT_STREAM_BLOCK_SIZE)

END_DEF_OUTPUT_STREAM_BUFFER(STREAM_OUT)


DEF_INPUT_STREAM_BUFFER(STREAM_IN, INPUT_STREAM_BLOCK_SIZE)

END_DEF_INPUT_STREAM_BUFFER(STREAM_IN)


STREAM_IN_SERIAL_PORT_ADAPTER(STREAM_IN, INPUT_STREAM_BLOCK_COUNT)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern ARM_DRIVER_USART  USART_Driver_(USART_DRV_NUM);

/*============================ IMPLEMENTATION ================================*/

extern void serial_port_enable_tx_cpl_interrupt(void);
extern void serial_port_disbale_tx_cpl_interrupt(void);
extern void serial_port_fill_byte(uint8_t chByte);

NO_INIT static volatile struct {
    STREAM_OUT_block_t *ptBlock;
    uint8_t *pchBuffer;
    uint_fast16_t hwSize;
    uint_fast16_t hwIndex;
    uint_fast16_t hwTimeoutCounter;
} s_tStreamOutService;

static void request_send(void)
{
    s_tStreamOutService.ptBlock = STREAM_OUT.Block.Exchange(s_tStreamOutService.ptBlock);
    STREAM_OUT_block_t *ptBlock = s_tStreamOutService.ptBlock;
    if (NULL != ptBlock) {

        s_tStreamOutService.pchBuffer = ptBlock->chBuffer;
        s_tStreamOutService.hwSize = ptBlock->wSize;
        s_tStreamOutService.hwIndex = 0;
        
        //! enable TX interrupt
        serial_port_enable_tx_cpl_interrupt();
        
        serial_port_fill_byte(s_tStreamOutService.pchBuffer[s_tStreamOutService.hwIndex++]);
    }
}

static void output_stream_buffer_req_send_event_handler(stream_buffer_t *ptObj)
{
    if (NULL == ptObj) {
        return ;
    }
    
    request_send();
}

void insert_serial_port_tx_cpl_event_handler(void)
{
    if (NULL == s_tStreamOutService.pchBuffer || 0 == s_tStreamOutService.hwSize) {
        /* it appears output service is cancelled */
        //! disable TX Interrupt
        serial_port_disbale_tx_cpl_interrupt();
        return ;
    }
    
    if (s_tStreamOutService.hwIndex >= s_tStreamOutService.hwSize) {
        
        //! disable TX Interrupt
        serial_port_disbale_tx_cpl_interrupt();
        
        //! current buffer is full
        request_send();             //! request another read
    } else {
        serial_port_fill_byte(s_tStreamOutService.pchBuffer[s_tStreamOutService.hwIndex++]);
    }
}









/* this function is called instead of the original UART0RX_Handler() */
void USART0_RX_CPL_Handler(void)
{   
    //! clear interrupt flag
    CMSDK_UART0->INTCLEAR = CMSDK_UART0->INTSTATUS;
    STREAM_IN_insert_serial_port_rx_cpl_event_handler();
}


/* this function is called instead of the original UART0TX_Handler() */
void USART0_TX_CPL_Handler(void)
{   
    //! clear interrupt flag
    CMSDK_UART0->INTCLEAR = CMSDK_UART0->INTSTATUS;
    //! implement our own version of uart tx interrupt
    
    insert_serial_port_tx_cpl_event_handler();
}


void serial_port_enable_tx_cpl_interrupt(void)
{
    CMSDK_UART0->CTRL |= CMSDK_UART_CTRL_TXIRQEN_Msk;
}

void serial_port_disbale_tx_cpl_interrupt(void)
{
    CMSDK_UART0->CTRL &= ~CMSDK_UART_CTRL_TXIRQEN_Msk;
}

void serial_port_fill_byte(uint8_t chByte)
{
    CMSDK_UART0->DATA = chByte; 
}


void STREAM_IN_serial_port_enable_rx_cpl_interrupt(void)
{
    CMSDK_UART0->CTRL |= CMSDK_UART_CTRL_RXIRQEN_Msk;
}

void STREAM_IN_serial_port_disable_rx_cpl_interrupt(void)
{
    CMSDK_UART0->CTRL &= ~CMSDK_UART_CTRL_RXIRQEN_Msk;
}

uint8_t STREAM_IN_serial_port_get_byte(void)
{
    return CMSDK_UART0->DATA; 
}



/*! \note initialize usart for stdout
 *  \param none
 *  \retval true    initialization succeeded.
 *  \retval false   initialization failed
 */  
bool stdout_init (void) 
{
    
    do {
        int32_t status;

        status = ptrUSART->Initialize(NULL /*&UART0_Signal_Handler*/);
        if (status != ARM_DRIVER_OK) { 
            break; 
        }

        status = ptrUSART->PowerControl(ARM_POWER_FULL);
        if (status != ARM_DRIVER_OK) { 
            break; 
        }

        status = ptrUSART->Control( ARM_USART_MODE_ASYNCHRONOUS     |
                                    ARM_USART_DATA_BITS_8           |
                                    ARM_USART_PARITY_NONE           |
                                    ARM_USART_STOP_BITS_1           |
                                    ARM_USART_FLOW_CONTROL_NONE,
                                    USART_BAUDRATE                  );
        if (status != ARM_DRIVER_OK) { 
            break; 
        }

        status = ptrUSART->Control( ARM_USART_CONTROL_TX ,ENABLED   );
        if (status != ARM_DRIVER_OK) { 
            break; 
        }
        
        status = ptrUSART->Control( ARM_USART_CONTROL_RX ,ENABLED   );
        if (status != ARM_DRIVER_OK) { 
            break; 
        }
        
        do {
            static NO_INIT STREAM_OUT_stream_buffer_block_t s_tBlocks[OUTPUT_STREAM_BLOCK_COUNT];
            OUTPUT_STREAM_BUFFER_CFG(
                STREAM_OUT, 
                &output_stream_buffer_req_send_event_handler
            );
            
            STREAM_OUT.AddBuffer(s_tBlocks, sizeof(s_tBlocks));
            
            memset((void *)&s_tStreamOutService, 0, sizeof(s_tStreamOutService));
        } while(false);
        
        STREAM_IN_adapter_init();
        return true;
    } while(false);

    return false;
}


 

/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) 
{
    while(!STREAM_OUT.Stream.Write(ch));
    
    return ch;
}

