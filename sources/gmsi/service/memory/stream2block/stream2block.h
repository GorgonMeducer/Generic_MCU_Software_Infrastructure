/****************************************************************************
*  Copyright 2017 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

#ifndef __STREAM_2_BLOCK_H__
#define __STREAM_2_BLOCK_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_SERVICE_STREAM_TO_BLOCK == ENABLED
#include "..\block_queue\block_queue.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define STREAM_BUFFER_CFG(__ADDR, ...)                  \
    do {                                                \
        stream_buffer_cfg_t tCFG = {                    \
            __VA_ARGS__                                 \
        };                                              \
                                                        \
        STREAM_BUFFER.Init((__ADDR), &tCFG);            \
    } while(false)


/*----------------------------------------------------------------------------*
 * common template                                                            *
 *----------------------------------------------------------------------------*/
#define __STREAM_BUFFER_COMMON(__NAME, __BLOCK_SIZE)                            \
    declare_class( __NAME##_stream_buffer_block_t )                             \
    def_class(__NAME##_stream_buffer_block_t, inherit(block_t))                 \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    end_def_class(__NAME##_stream_buffer_block_t,inherit(block_t))              \
    NO_INIT private stream_buffer_t s_t##__NAME##StreamBuffer;                  \
    NO_INIT private block_pool_t s_t##__NAME##_BlockPool;                       \
                                                                                \
    private bool __NAME##_stream_buffer_init(stream_buffer_cfg_t *ptCFG)        \
    {                                                                           \
        BLOCK.Heap.Init(&s_t##__NAME##_BlockPool);                              \
        return STREAM_BUFFER.Init(&s_t##__NAME##StreamBuffer, ptCFG);           \
    }                                                                           \
    private bool __NAME##_stream_add_buffer(void *pBuffer, uint_fast16_t hwSize)\
    {                                                                           \
        return BLOCK.Heap.Add(&s_t##__NAME##_BlockPool,                         \
                pBuffer, hwSize, sizeof(__NAME##_stream_buffer_block_t));       \
    }                                                                           \
    private __NAME##_stream_buffer_block_t * __NAME##_stream_exchange_block(    \
                              __NAME##_stream_buffer_block_t *ptOld)            \
    {                                                                           \
        return (__NAME##_stream_buffer_block_t *)STREAM_BUFFER.Block.Exchange(  \
                &s_t##__NAME##StreamBuffer, ref_obj_as((*ptOld), block_t));     \
    }                                                                           \
    private __NAME##_stream_buffer_block_t *__NAME##_stream_get_next_block(void)\
    {                                                                           \
        return (__NAME##_stream_buffer_block_t *)STREAM_BUFFER.Block.GetNext(   \
                &s_t##__NAME##StreamBuffer);                                    \
    }                                                                           \
    private void __NAME##_stream_return_block(                                  \
            __NAME##_stream_buffer_block_t *ptOld)                              \
    {                                                                           \
        STREAM_BUFFER.Block.Return(                                             \
            &s_t##__NAME##StreamBuffer, ref_obj_as((*ptOld), block_t));         \
    }                                                                           \
    
#define __EXTERN_STREAM_BUFFER_COMMON(__NAME, __BLOCK_SIZE)                     \
    DECLARE_CLASS( __NAME##_stream_buffer_block_t )                             \
    EXTERN_CLASS(__NAME##_stream_buffer_block_t, INHERIT(block_t))              \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    END_EXTERN_CLASS(__NAME##_stream_buffer_block_t,                            \
                        INHERIT(block_t))                                       


/*----------------------------------------------------------------------------*
 * Output stream template                                                     *
 *----------------------------------------------------------------------------*/

#define OUTPUT_STREAM_BUFFER_CFG(__NAME, ...)                                   \
        do {                                                                    \
            stream_buffer_cfg_t tCFG = {                                        \
                .ptPool = &s_t##__NAME##_BlockPool,                             \
                .tDirection = OUTPUT_STREAM,                                    \
                __VA_ARGS__                                                     \
            };                                                                  \
            __NAME.Init(&tCFG);                                                 \
        } while(false)
    
#define __OUTPUT_STREAM_BUFFER_COMMON(__NAME)                                   \
    const struct {                                                              \
                                                                                \
        bool (*Init)(stream_buffer_cfg_t *);                                    \
        bool (*AddBuffer)(void *, uint_fast16_t );                              \
                                                                                \
        struct {                                                                \
            bool    (*WriteByte)(uint8_t);                                      \
            int32_t (*Write)(uint8_t *, uint_fast16_t );                        \
            bool    (*Flush)(void);                                             \
        } Stream;                                                               \
                                                                                \
        struct {                                                                \
            __NAME##_stream_buffer_block_t *(*Exchange)                         \
                                ( __NAME##_stream_buffer_block_t * );           \
            __NAME##_stream_buffer_block_t *(*GetNext)    (void);               \
            void              (*Return)     (__NAME##_stream_buffer_block_t *); \
        } Block;                                                                \
                                                                                \
    }  __NAME        
        
#define __DEF_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                        \
    __STREAM_BUFFER_COMMON(__NAME, (__BLOCK_SIZE))                              \
    private bool __NAME##_stream_write_byte(uint8_t chData)                     \
    {                                                                           \
        return STREAM_BUFFER.Stream.WriteByte(                                  \
                                        &s_t##__NAME##StreamBuffer, chData);    \
    }                                                                           \
    private int32_t __NAME##_stream_write(uint8_t *pchSrc, uint_fast16_t hwSize)\
    {                                                                           \
        return STREAM_BUFFER.Stream.Write(  &s_t##__NAME##StreamBuffer,         \
                                            pchSrc,                             \
                                            hwSize);                            \
    }                                                                           \
    private bool __NAME##_stream_flush(void)                                    \
    {                                                                           \
        return STREAM_BUFFER.Stream.Flush(&s_t##__NAME##StreamBuffer);          \
    }                                                                           \
    __OUTPUT_STREAM_BUFFER_COMMON(__NAME) = {                                   \
            .Init =         &__NAME##_stream_buffer_init,                       \
            .AddBuffer =    &__NAME##_stream_add_buffer,                        \
            .Stream = {                                                         \
                .WriteByte =    &__NAME##_stream_write_byte,                    \
                .Write =        &__NAME##_stream_write,                         \
                .Flush =        &__NAME##_stream_flush,                         \
            },                                                                  \
            .Block = {                                                          \
                .Exchange = &__NAME##_stream_exchange_block,                    \
                .GetNext  = &__NAME##_stream_get_next_block,                    \
                .Return =   &__NAME##_stream_return_block,                      \
            },                                                                  \
        };
    
#define DEF_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                          \
            __DEF_OUTPUT_STREAM_BUFFER(__NAME, (__BLOCK_SIZE))

#define END_DEF_OUTPUT_STREAM_BUFFER(__NAME)
        

#define __EXTERN_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                     \
    __EXTERN_STREAM_BUFFER_COMMON(__NAME, (__BLOCK_SIZE))                       \
    extern __OUTPUT_STREAM_BUFFER_COMMON(__NAME);

        
#define EXTERN_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                       \
            __EXTERN_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)

#define END_EXTERN_OUTPUT_STREAM_BUFFER(__NAME)         

        
/*----------------------------------------------------------------------------*
 * Output stream seraial port adapter template                                *
 *----------------------------------------------------------------------------*/

#define __EXTERN_STREAM_OUT_SERIAL_PORT_ADAPTER(__NAME)                         \
            extern void __NAME##_insert_serial_port_tx_cpl_event_handler(void); \
            extern void __NAME##_output_stream_adapter_init(void); 
#define EXTERN_STREAM_OUT_SERIAL_PORT_ADAPTER(__NAME)                           \
            __EXTERN_STREAM_OUT_SERIAL_PORT_ADAPTER(__NAME) 
 
#define __STREAM_OUT_SERIAL_PORT_ADAPTER(__NAME, __BLOCK_COUNT)                 \
    extern void __NAME##_serial_port_enable_tx_cpl_interrupt(void);             \
    extern void __NAME##_serial_port_disbale_tx_cpl_interrupt(void);            \
    extern void __NAME##_serial_port_fill_byte(uint8_t chByte);                 \
                                                                                \
    NO_INIT private volatile struct {                                           \
        __NAME##_stream_buffer_block_t *ptBlock;                                \
        uint8_t *pchBuffer;                                                     \
        uint_fast16_t hwSize;                                                   \
        uint_fast16_t hwIndex;                                                  \
        uint_fast16_t hwTimeoutCounter;                                         \
    } s_t##__NAME##StreamOutService;                                            \
    private void __NAME##_request_send(void)                                    \
    {                                                                           \
        s_t##__NAME##StreamOutService.ptBlock = STREAM_OUT.Block.Exchange(      \
            s_t##__NAME##StreamOutService.ptBlock);                             \
        __NAME##_stream_buffer_block_t *ptBlock =                               \
            s_t##__NAME##StreamOutService.ptBlock;                              \
        if (NULL != ptBlock) {                                                  \
            s_t##__NAME##StreamOutService.pchBuffer =                           \
                (uint8_t *)BLOCK.Buffer.Get(ref_obj_as((*ptBlock), block_t));   \
            s_t##__NAME##StreamOutService.hwSize =                              \
                BLOCK.Size.Get(ref_obj_as((*ptBlock), block_t));                \
            s_t##__NAME##StreamOutService.hwIndex = 0;                          \
                                                                                \
            __NAME##_serial_port_enable_tx_cpl_interrupt();                     \
                                                                                \
            __NAME##_serial_port_fill_byte(                                     \
                s_t##__NAME##StreamOutService.pchBuffer                         \
                    [s_t##__NAME##StreamOutService.hwIndex++]);                 \
        }                                                                       \
    }                                                                           \
    void __NAME##_insert_serial_port_tx_cpl_event_handler(void)                 \
    {                                                                           \
        if (    (NULL == s_t##__NAME##StreamOutService.pchBuffer)               \
            ||  (0 == s_t##__NAME##StreamOutService.hwSize)) {                  \
            /* it appears output service is cancelled */                        \
            __NAME##_serial_port_disbale_tx_cpl_interrupt();                    \
            return ;                                                            \
        }                                                                       \
                                                                                \
        if (    s_t##__NAME##StreamOutService.hwIndex                           \
            >=  s_t##__NAME##StreamOutService.hwSize) {                         \
            __NAME##_serial_port_disbale_tx_cpl_interrupt();                    \
            __NAME##_request_send();                                            \
        } else {                                                                \
            __NAME##_serial_port_fill_byte(                                     \
                s_t##__NAME##StreamOutService.pchBuffer[                        \
                    s_t##__NAME##StreamOutService.hwIndex++]);                  \
        }                                                                       \
    }                                                                           \
    private void __NAME##_output_stream_req_send_event_handler(                 \
                stream_buffer_t *ptObj)                                         \
    {                                                                           \
        if (NULL == ptObj) {                                                    \
            return ;                                                            \
        }                                                                       \
                                                                                \
        __NAME##_request_send();                                                \
    }                                                                           \
    void __NAME##_output_stream_adapter_init(void)                              \
    {                                                                           \
        NO_INIT private __NAME##_stream_buffer_block_t                           \
                        s_tBlocks[__BLOCK_COUNT];                               \
        OUTPUT_STREAM_BUFFER_CFG(                                               \
            __NAME,                                                             \
            &__NAME##_output_stream_req_send_event_handler                      \
        );                                                                      \
                                                                                \
        __NAME.AddBuffer(s_tBlocks, sizeof(s_tBlocks));                         \
        memset((void *)&s_t##__NAME##StreamOutService, 0,                       \
            sizeof(s_t##__NAME##StreamOutService));                             \
    }
    
#define STREAM_OUT_SERIAL_PORT_ADAPTER(__NAME, __BLOCK_COUNT)                   \
            __STREAM_OUT_SERIAL_PORT_ADAPTER(__NAME, (__BLOCK_COUNT))
        
/*----------------------------------------------------------------------------*
 * Input stream template                                                      *
 *----------------------------------------------------------------------------*/
#define INPUT_STREAM_BUFFER_CFG(__NAME, ...)                                    \
        do {                                                                    \
            stream_buffer_cfg_t tCFG = {                                        \
                .ptPool = &s_t##__NAME##_BlockPool,                             \
                .tDirection = INPUT_STREAM,                                     \
                __VA_ARGS__                                                     \
            };                                                                  \
            __NAME.Init(&tCFG);                                                 \
        } while(false)

#define __INPUT_STREAM_BUFFER_COMMON(__NAME)                                    \
    const struct {                                                              \
                                                                                \
        bool (*Init)(stream_buffer_cfg_t *);                                    \
        bool (*AddBuffer)(void *, uint_fast16_t );                              \
                                                                                \
        struct {                                                                \
            bool    (*ReadByte)(uint8_t *);                                     \
            int32_t (*Read)(uint8_t *, uint_fast16_t );                         \
        } Stream;                                                               \
                                                                                \
        struct {                                                                \
            __NAME##_stream_buffer_block_t *(*Exchange)                         \
                            ( __NAME##_stream_buffer_block_t * );               \
            __NAME##_stream_buffer_block_t *(*GetNext) (void);                  \
            void              (*Return)  (__NAME##_stream_buffer_block_t *);    \
        } Block;                                                                \
                                                                                \
    }  __NAME
        
                                                                                        
#define __DEF_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                         \
    __STREAM_BUFFER_COMMON(__NAME, (__BLOCK_SIZE))                              \
    private bool __NAME##_stream_read_byte(uint8_t *pchData)                    \
    {                                                                           \
        return STREAM_BUFFER.Stream.ReadByte(                                   \
                                        &s_t##__NAME##StreamBuffer, pchData);   \
    }                                                                           \
    private int32_t __NAME##_stream_read(uint8_t *pchSrc, uint_fast16_t hwSize) \
    {                                                                           \
        return STREAM_BUFFER.Stream.Read(  &s_t##__NAME##StreamBuffer,          \
                                            pchSrc,                             \
                                            hwSize);                            \
    }                                                                           \
    __INPUT_STREAM_BUFFER_COMMON(__NAME) = {                                    \
            .Init =         &__NAME##_stream_buffer_init,                       \
            .AddBuffer =    &__NAME##_stream_add_buffer,                        \
            .Stream = {                                                         \
                .ReadByte = &__NAME##_stream_read_byte,                         \
                .Read =     &__NAME##_stream_read,                              \
            },                                                                  \
            .Block = {                                                          \
                .Exchange = &__NAME##_stream_exchange_block,                    \
                .GetNext  = &__NAME##_stream_get_next_block,                    \
                .Return =   &__NAME##_stream_return_block,                      \
            },                                                                  \
        };
    
#define DEF_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                           \
            __DEF_INPUT_STREAM_BUFFER(__NAME, (__BLOCK_SIZE))

        
#define END_DEF_INPUT_STREAM_BUFFER(__NAME)
        
#define __EXTERN_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                      \
    __EXTERN_STREAM_BUFFER_COMMON(__NAME, (__BLOCK_SIZE))                       \
    extern __INPUT_STREAM_BUFFER_COMMON(__NAME);

        
#define EXTERN_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                        \
            __EXTERN_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)

#define END_EXTERN_INPUT_STREAM_BUFFER(__NAME)   
        
/*----------------------------------------------------------------------------*
 * Input stream seraial port adapter template                                 *
 *----------------------------------------------------------------------------*/
#define __EXTERN_STREAM_IN_SERIAL_PORT_ADAPTER(__NAME)                          \
    extern void __NAME##_1ms_event_handler(void);                               \
    extern void __NAME##_insert_serial_port_rx_cpl_event_handler(void);         \
    extern void __NAME##_input_stream_adapter_init(void);                       
    
#define EXTERN_STREAM_IN_SERIAL_PORT_ADAPTER(__NAME)                            \
            __EXTERN_STREAM_IN_SERIAL_PORT_ADAPTER(__NAME)
    
#define __STREAM_IN_SERIAL_PORT_ADAPTER(__NAME, __BLOCK_COUNT)                  \
    extern void __NAME##_serial_port_enable_rx_cpl_interrupt(void);             \
    extern void __NAME##_serial_port_disable_rx_cpl_interrupt(void);            \
    extern uint8_t __NAME##_serial_port_get_byte(void);                         \
    NO_INIT private volatile struct {                                           \
        __NAME##_stream_buffer_block_t *ptBlock;                                \
        uint8_t *pchBuffer;                                                     \
        uint_fast16_t hwSize;                                                   \
        uint_fast16_t hwIndex;                                                  \
        uint_fast16_t hwTimeoutCounter;                                         \
    } s_t##__NAME##StreamInService;                                             \
    private void __NAME##_request_read(void)                                    \
    {                                                                           \
        s_t##__NAME##StreamInService.ptBlock =                                  \
            __NAME.Block.Exchange(                                              \
                s_t##__NAME##StreamInService.ptBlock);                          \
        __NAME##_stream_buffer_block_t *ptReadBuffer =                          \
                s_t##__NAME##StreamInService.ptBlock;                           \
        if (NULL != ptReadBuffer) {                                             \
            if (0 == BLOCK.Size.Get(ref_obj_as(*ptReadBuffer, block_t))) {      \
                return ;                                                        \
            }                                                                   \
            s_t##__NAME##StreamInService.pchBuffer =                            \
                BLOCK.Buffer.Get(ref_obj_as(*ptReadBuffer, block_t));           \
            s_t##__NAME##StreamInService.hwSize =                               \
                BLOCK.Size.Get(ref_obj_as(*ptReadBuffer, block_t));             \
            s_t##__NAME##StreamInService.hwIndex = 0;                           \
            __NAME##_serial_port_enable_rx_cpl_interrupt();                     \
        }                                                                       \
    }                                                                           \
    void __NAME##_1ms_event_handler(void)                                       \
    {                                                                           \
        if (s_t##__NAME##StreamInService.hwTimeoutCounter) {                    \
            s_t##__NAME##StreamInService.hwTimeoutCounter--;                    \
            if (0 == s_t##__NAME##StreamInService.hwTimeoutCounter) {           \
                /*! timeout! */                                                 \
                if (0 != s_t##__NAME##StreamInService.hwIndex) {                \
                    BLOCK.Size.Set(                                             \
                        ref_obj_as(*s_t##__NAME##StreamInService.ptBlock, block_t), \
                        s_t##__NAME##StreamInService.hwIndex);                  \
                    __NAME##_request_read();                                    \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }                                                                           \
    private void __NAME##_reset_stream_in_rx_timer(void)                        \
    {                                                                           \
        SAFE_ATOM_CODE (                                                        \
            s_t##__NAME##StreamInService.hwTimeoutCounter =                     \
                STREAM_IN_RCV_TIMEOUT;                                          \
        )                                                                       \
    }                                                                           \
    private void __NAME##_input_stream_req_read_event_handler(                  \
                    stream_buffer_t *ptObj)                                     \
    {                                                                           \
        if (NULL == ptObj) {                                                    \
            return ;                                                            \
        }                                                                       \
                                                                                \
        __NAME##_request_read();                                                \
    }                                                                           \
    void __NAME##_insert_serial_port_rx_cpl_event_handler(void)                 \
    {                                                                           \
        __NAME##_reset_stream_in_rx_timer();                                    \
                                                                                \
        if (    (NULL == s_t##__NAME##StreamInService.pchBuffer)                \
            ||  (0 == s_t##__NAME##StreamInService.hwSize)) {                   \
            /* it appears receive service is cancelled */                       \
            __NAME##_serial_port_disable_rx_cpl_interrupt();                    \
            return ;                                                            \
        }                                                                       \
                                                                                \
        s_t##__NAME##StreamInService.pchBuffer                                  \
            [s_t##__NAME##StreamInService.hwIndex++] =                          \
                __NAME##_serial_port_get_byte();                                \
        if (    s_t##__NAME##StreamInService.hwIndex                            \
            >=  s_t##__NAME##StreamInService.hwSize) {                          \
            __NAME##_serial_port_disable_rx_cpl_interrupt();                    \
            __NAME##_request_read();                                            \
        }                                                                       \
    }                                                                           \
    void __NAME##_input_stream_adapter_init(void)                               \
    {                                                                           \
        NO_INIT private __NAME##_stream_buffer_block_t                           \
                        s_tBlocks[__BLOCK_COUNT];                               \
        INPUT_STREAM_BUFFER_CFG(                                                \
            __NAME,                                                             \
            &__NAME##_input_stream_req_read_event_handler                       \
        );                                                                      \
                                                                                \
        __NAME.AddBuffer(s_tBlocks, sizeof(s_tBlocks));                         \
        memset((void *)&s_t##__NAME##StreamInService, 0,                        \
            sizeof(s_t##__NAME##StreamInService));                              \
    }
        
#define STREAM_IN_SERIAL_PORT_ADAPTER(__NAME, __BLOCK_COUNT)                    \
            __STREAM_IN_SERIAL_PORT_ADAPTER(__NAME, (__BLOCK_COUNT))    
    
/*============================ TYPES =========================================*/


//! \note no thread safe queue is required
EXTERN_QUEUE_U8(StreamBufferQueue, uint_fast16_t, bool)
    
END_EXTERN_QUEUE_U8(StreamBufferQueue)



//! \brief stream buffer control block
//! @{
declare_class(stream_buffer_t)

typedef void stream_buffer_req_event_t(stream_buffer_t *ptThis);


typedef union {
    struct {
        uint8_t        IsAvailable         :1;
        uint8_t        IsOutput            :1;
        uint8_t        IsDataAvailable     :1;
        uint8_t        IsBlockBufferDrain  :1;
        uint8_t                            :4;
    };
    uint8_t tValue;
} stream_buffer_status_t;

extern_class(stream_buffer_t, 
    which(   
        inherit(block_queue_t)                                                  //!< inherit from block_queue_t
        inherit(QUEUE(StreamBufferQueue))                                       //!< inherit from queue StreamBufferQueue
    )) 

    bool                                    bIsOutput;                          //!< direction
    bool                                    bIsQueueInitialised;                //!< Indicate whether the queue has been inialised or not
    block_pool_t                           *ptBlockPool;                        //!< a reference to outside block pool
    block_t                                *ptUsedByQueue;                      //!< buffer block used by queue
    block_t                                *ptUsedByOutside;                    //!< buffer block lent out  
    stream_buffer_req_event_t              *fnRequestSend;                      //!< callback for triggering the first output transaction
    stream_buffer_req_event_t              *fnRequestReceive; 
    uint8_t                                 chBlockReservedSize;
    stream_buffer_status_t                  tStatus;   
    uint8_t                                 chReservedBlock;
    uint8_t                                 chBlockLimit;
    uint8_t                                 chBlockCount;
end_extern_class(stream_buffer_t,
    which(   
        inherit(block_queue_t)                                                  //!< inherit from block_queue_t
        inherit(QUEUE(StreamBufferQueue))                                       //!< inherit from queue StreamBufferQueue
    ))
//! @}

typedef struct {
    block_pool_t                           *ptPool;
    enum {
        INPUT_STREAM = 0,
        OUTPUT_STREAM
    } tDirection;
    
    stream_buffer_req_event_t              *fnRequestHandler;
    uint_fast8_t                            chBlockReservedSize;   
    uint_fast8_t                            chReservedBlock;
    uint8_t                                 chBlockLimit;
}stream_buffer_cfg_t;



def_interface(i_stream_buffer_t)

    bool        (*Init)         (stream_buffer_t *, stream_buffer_cfg_t *);
    stream_buffer_status_t
                (*Status)       (stream_buffer_t *);
    bool        (*Dispose)      (stream_buffer_t *);
    struct {
        bool    (*ReadByte)     (stream_buffer_t *, uint8_t *);
        bool    (*WriteByte)    (stream_buffer_t *, uint8_t);
        int32_t (*Read)         (stream_buffer_t *, uint8_t *, uint_fast16_t );
        int32_t (*Write)        (stream_buffer_t *, uint8_t *, uint_fast16_t );
        bool    (*WriteBlock)   (stream_buffer_t *, block_t *ptBlock);
        bool    (*Flush)        (stream_buffer_t *ptObj);
    } Stream;
    
    struct {
        block_t*(*Exchange)     (stream_buffer_t *, block_t *);
        block_t*(*GetNext)      (stream_buffer_t *);
        void    (*Return)       (stream_buffer_t *, block_t *);
    } Block;
    
    

end_def_interface(i_stream_buffer_t)
    
/*============================ GLOBAL VARIABLES ==============================*/
    
extern const i_stream_buffer_t STREAM_BUFFER;    

/*============================ PROTOTYPES ====================================*/




#endif
#endif
/* EOF */
