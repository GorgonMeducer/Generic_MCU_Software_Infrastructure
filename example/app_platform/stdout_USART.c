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

END_DEF_OUTPUT_STREAM_BUFFER(STREAM_IN)




/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static STREAM_OUT_block_t *s_ptWriteBuffer = NULL;
static STREAM_IN_block_t *s_ptReadBuffer = NULL;

NO_INIT static volatile struct {
    uint8_t *pchBuffer;
    uint_fast16_t hwSize;
    uint_fast16_t hwIndex;
    uint_fast16_t hwTimeoutCounter;
} s_tStreamInRXService;

/*============================ PROTOTYPES ====================================*/

 
extern ARM_DRIVER_USART  USART_Driver_(USART_DRV_NUM);



/*============================ IMPLEMENTATION ================================*/

static void request_read(void)
{
    s_ptReadBuffer = STREAM_IN.Block.Exchange(s_ptReadBuffer);
    if (NULL != s_ptReadBuffer) {
        if (NULL == s_ptReadBuffer->chBuffer || 0 == s_ptReadBuffer->wSize) {
            return ;            //!< this should not happen
        }
        s_tStreamInRXService.pchBuffer = s_ptReadBuffer->chBuffer;
        s_tStreamInRXService.hwSize = s_ptReadBuffer->wSize;
        s_tStreamInRXService.hwIndex = 0;
        
        //! enable RX interrupt
        CMSDK_UART0->CTRL |= CMSDK_UART_CTRL_RXIRQEN_Msk;
    }
}


static void reset_stream_in_rx_timer(void)
{
    SAFE_ATOM_CODE (
        s_tStreamInRXService.hwTimeoutCounter = STREAM_IN_RCV_TIMEOUT;
    )
}

/* \note please put it into a 1ms timer handler
 */
void stream_in_1ms_event_handler(void)
{
    if (s_tStreamInRXService.hwTimeoutCounter) {
        s_tStreamInRXService.hwTimeoutCounter--;
        if (0 == s_tStreamInRXService.hwTimeoutCounter) {
            /*! timeout! */
            if (0 != s_tStreamInRXService.hwIndex) {
                s_ptReadBuffer->wSize = s_tStreamInRXService.hwIndex;
                request_read();
            } /* else {
                //nothing is received, so we keep waiting...
            } */
        }
    }
}




/* this function is called instead of the original UART0RX_Handler() */
void USART0_RX_CPL_Handler(void)
{   
    //! clear interrupt flag
    CMSDK_UART0->INTCLEAR = CMSDK_UART0->INTSTATUS;
    //! implement our own version of uart rx interrupt
    reset_stream_in_rx_timer();
    
    if (NULL == s_tStreamInRXService.pchBuffer || 0 == s_tStreamInRXService.hwSize) {
        /* it appears receive service is cancelled */
        //! disable RX Interrupt
        CMSDK_UART0->CTRL &= ~CMSDK_UART_CTRL_RXIRQEN_Msk;
        return ;
    }
    
    s_tStreamInRXService.pchBuffer[s_tStreamInRXService.hwIndex++] = CMSDK_UART0->DATA;
    if (s_tStreamInRXService.hwIndex >= s_tStreamInRXService.hwSize) {
        
        //! disable RX Interrupt
        CMSDK_UART0->CTRL &= ~CMSDK_UART_CTRL_RXIRQEN_Msk;
        //! current buffer is full
        request_read();             //! request another read
    }
}



static void request_send(void)
{
    s_ptWriteBuffer = STREAM_OUT.Block.Exchange(s_ptWriteBuffer);
    if (NULL != s_ptWriteBuffer) {
        while(ARM_DRIVER_OK != Driver_USART0.Send(s_ptWriteBuffer->chBuffer, s_ptWriteBuffer->wSize));
    }
}




static void UART0_Signal_Handler (uint32_t wEvent)
{
    if (wEvent == ARM_USART_EVENT_SEND_COMPLETE) {
        request_send();
    } else if (wEvent == ARM_USART_EVENT_RECEIVE_COMPLETE) {
        request_read();
    }
}
static void output_stream_req_transaction_event_handler(stream_buffer_t *ptObj)
{
    if (NULL == ptObj) {
        return ;
    }
    
    request_send();
}

static void output_stream_req_read_event_handler(stream_buffer_t *ptObj)
{
    if (NULL == ptObj) {
        return ;
    }
    
    request_read();
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

        status = ptrUSART->Initialize(&UART0_Signal_Handler);
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
                &output_stream_req_transaction_event_handler
            );
            
            STREAM_OUT.AddBuffer(s_tBlocks, sizeof(s_tBlocks));
        } while(false);
        
        do {
            static NO_INIT STREAM_IN_stream_buffer_block_t s_tBlocks[INPUT_STREAM_BLOCK_COUNT];
            INPUT_STREAM_BUFFER_CFG(
                STREAM_IN,
                &output_stream_req_read_event_handler
            );
            
            STREAM_IN.AddBuffer(s_tBlocks, sizeof(s_tBlocks));
            
            memset((void *)&s_tStreamInRXService, 0, sizeof(s_tStreamInRXService));
        } while(false);
        return true;
    } while(false);

    return false;
}

/*! \note transfer spcified stream via uart
 *! \param chStr  the start address of the target memory
 *! \param hwSize the size of the target memory
 *! \return none
 */
void transfer_string(const char *pchStr, uint_fast16_t hwSize)
{
    while(ARM_DRIVER_OK != Driver_USART0.Send(pchStr, hwSize));
}
 

/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) 
{
    /*
    if (ptrUSART->Send((uint8_t *)&ch, 1) != ARM_DRIVER_OK) {
        return (-1);
    }
    while (ptrUSART->GetTxCount() != 1);
    return (ch);
    */
    while(!STREAM_OUT.Stream.Write(ch));
    
    return ch;
}


/*! \note send one char directly to usart
 *  \param chByte target byte
 *  \retval true  access succeed 
 *  \retval false access failed
 */ 
bool serial_out(uint8_t chByte)
{
    return (ptrUSART->Send(&chByte, 1) == ARM_DRIVER_OK); 
}


/*! \note read one char directly from usart
 *  \param pchByte the address of a byte buffer
 *  \retval true  access succeed 
 *  \retval false access failed
 */
bool serial_in(uint8_t *pchByte)
{
    if (NULL == pchByte) {
        return false;
    }
    
    return (ARM_DRIVER_OK == ptrUSART->Receive(pchByte, 1));
}
