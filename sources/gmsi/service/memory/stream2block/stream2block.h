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
#include "..\epool\epool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*----------------------------------------------------------------------------*
 * Output stream template                                                     *
 *----------------------------------------------------------------------------*/
 
#define OUTPUT_STREAM_BUFFER_CFG(__NAME, ...)                                   \
        do {                                                                    \
            stream_buffer_cfg_t tCFG = {                                        \
                .tDirection = OUTPUT_STREAM,                                    \
                __VA_ARGS__                                                     \
            };                                                                  \
            __NAME.Init(&tCFG);                                                 \
        } while(false)

#define __DEF_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                        \
    DECLARE_CLASS( __NAME##_stream_buffer_block_t )                             \
    DEF_CLASS(__NAME##_stream_buffer_block_t, INHERIT(stream_buffer_block_t))   \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    END_DEF_CLASS(__NAME##_stream_buffer_block_t,INHERIT(stream_buffer_block_t))\
    typedef struct {                                                            \
        uint32_t wSize;                                                         \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    } __NAME##_block_t;                                                         \
    NO_INIT static stream_buffer_t s_t##__NAME##StreamBuffer;                   \
                                                                                \
    static bool __NAME##_stream_buffer_init(stream_buffer_cfg_t *ptCFG)         \
    {                                                                           \
        return STREAM_BUFFER.Init(&s_t##__NAME##StreamBuffer, ptCFG);           \
    }                                                                           \
    static bool __NAME##_stream_add_buffer(void *pBuffer, uint_fast16_t hwSize) \
    {                                                                           \
        return STREAM_BUFFER.AddBuffer(                                         \
                &s_t##__NAME##StreamBuffer,                                     \
                pBuffer, hwSize, sizeof(__NAME##_stream_buffer_block_t));       \
    }                                                                           \
    static bool __NAME##_stream_write(uint8_t chData)                           \
    {                                                                           \
        return STREAM_BUFFER.Stream.Write(&s_t##__NAME##StreamBuffer, chData);  \
    }                                                                           \
    static void __NAME##_stream_flush(void)                                     \
    {                                                                           \
        STREAM_BUFFER.Stream.Flush(&s_t##__NAME##StreamBuffer);                 \
    }                                                                           \
    static __NAME##_block_t * __NAME##_stream_exchange_block(                   \
                                            __NAME##_block_t *ptOld)            \
    {                                                                           \
        return (__NAME##_block_t *)STREAM_BUFFER.Block.Exchange(                \
                &s_t##__NAME##StreamBuffer, (void *)ptOld);                     \
    }                                                                           \
    DEF_INTERFACE(i_##__NAME##_output_stream_buffer_t)                          \
                                                                                \
        bool (*Init)(stream_buffer_cfg_t *);                                    \
        bool (*AddBuffer)(void *, uint_fast16_t );                              \
                                                                                \
        struct {                                                                \
            bool (*Write)(uint8_t);                                             \
            void (*Flush)(void);                                                \
        } Stream;                                                               \
                                                                                \
        struct {                                                                \
            __NAME##_block_t *(*Exchange)( __NAME##_block_t * );                \
        } Block;                                                                \
                                                                                \
    END_DEF_INTERFACE(i_##__NAME##_output_stream_buffer_t)                      \
                                                                                \
    const i_##__NAME##_output_stream_buffer_t __NAME = {                        \
            .Init =         &__NAME##_stream_buffer_init,                       \
            .AddBuffer =    &__NAME##_stream_add_buffer,                        \
            .Stream = {                                                         \
                .Write =    &__NAME##_stream_write,                             \
                .Flush =    &__NAME##_stream_flush,                             \
            },                                                                  \
            .Block = {                                                          \
                .Exchange = &__NAME##_stream_exchange_block,                    \
            },                                                                  \
        };
    
#define DEF_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                          \
            __DEF_OUTPUT_STREAM_BUFFER(__NAME, (__BLOCK_SIZE))

        
#define END_DEF_OUTPUT_STREAM_BUFFER(__NAME)
        
#define __EXTERN_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                     \
    DECLARE_CLASS( __NAME##_stream_buffer_block_t )                             \
    EXTERN_CLASS(__NAME##_stream_buffer_block_t, INHERIT(stream_buffer_block_t))\
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    END_EXTERN_CLASS(__NAME##_stream_buffer_block_t,                            \
                        INHERIT(stream_buffer_block_t))                         \
    typedef struct {                                                            \
        uint32_t wSize;                                                         \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    } __NAME##_block_t;                                                         \
                                                                                \
    DEF_INTERFACE(i_##__NAME##_output_stream_buffer_t)                          \
                                                                                \
        bool (*Init)(stream_buffer_cfg_t *);                                    \
        bool (*AddBuffer)(void *, uint_fast16_t );                              \
                                                                                \
        struct {                                                                \
            bool (*Write)(uint8_t);                                             \
            void (*Flush)(void);                                                \
        } Stream;                                                               \
                                                                                \
        struct {                                                                \
            __NAME##_block_t *(*Exchange)( __NAME##_block_t * );                \
        } Block;                                                                \
                                                                                \
    END_DEF_INTERFACE(i_##__NAME##_output_stream_buffer_t)                      \
                                                                                \
    extern const i_##__NAME##_output_stream_buffer_t __NAME;

        
#define EXTERN_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                       \
            __EXTERN_OUTPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)

#define END_EXTERN_OUTPUT_STREAM_BUFFER(__NAME)         

        
/*----------------------------------------------------------------------------*
 * Input stream template                                                      *
 *----------------------------------------------------------------------------*/
#define INPUT_STREAM_BUFFER_CFG(__NAME, ...)                                    \
        do {                                                                    \
            stream_buffer_cfg_t tCFG = {                                        \
                .tDirection = INPUT_STREAM,                                     \
                __VA_ARGS__                                                     \
            };                                                                  \
            __NAME.Init(&tCFG);                                                 \
        } while(false)

#define __DEF_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                         \
    DECLARE_CLASS( __NAME##_stream_buffer_block_t )                             \
    DEF_CLASS(__NAME##_stream_buffer_block_t, INHERIT(stream_buffer_block_t))   \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    END_DEF_CLASS(__NAME##_stream_buffer_block_t,INHERIT(stream_buffer_block_t))\
    typedef struct {                                                            \
        uint32_t wSize;                                                         \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    } __NAME##_block_t;                                                         \
    NO_INIT static stream_buffer_t s_t##__NAME##StreamBuffer;                   \
                                                                                \
    static bool __NAME##_stream_buffer_init(stream_buffer_cfg_t *ptCFG)         \
    {                                                                           \
        return STREAM_BUFFER.Init(&s_t##__NAME##StreamBuffer, ptCFG);           \
    }                                                                           \
    static bool __NAME##_stream_add_buffer(void *pBuffer, uint_fast16_t hwSize) \
    {                                                                           \
        return STREAM_BUFFER.AddBuffer(                                         \
                &s_t##__NAME##StreamBuffer,                                     \
                pBuffer, hwSize, sizeof(__NAME##_stream_buffer_block_t));       \
    }                                                                           \
    static bool __NAME##_stream_read(uint8_t *pchData)                          \
    {                                                                           \
        return STREAM_BUFFER.Stream.Read(&s_t##__NAME##StreamBuffer, pchData);  \
    }                                                                           \
    static __NAME##_block_t * __NAME##_stream_exchange_block(                   \
                                            __NAME##_block_t *ptOld)            \
    {                                                                           \
        return (__NAME##_block_t *)STREAM_BUFFER.Block.Exchange(                \
                &s_t##__NAME##StreamBuffer, (void *)ptOld);                     \
    }                                                                           \
    DEF_INTERFACE(i_##__NAME##_input_stream_buffer_t)                           \
                                                                                \
        bool (*Init)(stream_buffer_cfg_t *);                                    \
        bool (*AddBuffer)(void *, uint_fast16_t );                              \
                                                                                \
        struct {                                                                \
            bool (*Read)(uint8_t *);                                            \
        } Stream;                                                               \
                                                                                \
        struct {                                                                \
            __NAME##_block_t *(*Exchange)( __NAME##_block_t * );                \
        } Block;                                                                \
                                                                                \
    END_DEF_INTERFACE(i_##__NAME##_input_stream_buffer_t)                       \
                                                                                \
    const i_##__NAME##_input_stream_buffer_t __NAME = {                         \
            .Init =         &__NAME##_stream_buffer_init,                       \
            .AddBuffer =    &__NAME##_stream_add_buffer,                        \
            .Stream = {                                                         \
                .Read =    &__NAME##_stream_read,                               \
            },                                                                  \
            .Block = {                                                          \
                .Exchange = &__NAME##_stream_exchange_block,                    \
            },                                                                  \
        };
    
#define DEF_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                           \
            __DEF_INPUT_STREAM_BUFFER(__NAME, (__BLOCK_SIZE))

        
#define END_DEF_INPUT_STREAM_BUFFER(__NAME)
        
#define __EXTERN_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                      \
    DECLARE_CLASS( __NAME##_stream_buffer_block_t )                             \
    EXTERN_CLASS(__NAME##_stream_buffer_block_t, INHERIT(stream_buffer_block_t))\
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    END_EXTERN_CLASS(__NAME##_stream_buffer_block_t,                            \
                        INHERIT(stream_buffer_block_t))                         \
    typedef struct {                                                            \
        uint32_t wSize;                                                         \
        uint8_t chBuffer[__BLOCK_SIZE];                                         \
    } __NAME##_block_t;                                                         \
                                                                                \
    DEF_INTERFACE(i_##__NAME##_input_stream_buffer_t)                           \
                                                                                \
        bool (*Init)(stream_buffer_cfg_t *);                                    \
        bool (*AddBuffer)(void *, uint_fast16_t );                              \
                                                                                \
        struct {                                                                \
            bool (*Read)(uint8_t *);                                            \
        } Stream;                                                               \
                                                                                \
        struct {                                                                \
            __NAME##_block_t *(*Exchange)( __NAME##_block_t * );                \
        } Block;                                                                \
                                                                                \
    END_DEF_INTERFACE(i_##__NAME##_input_stream_buffer_t)                       \
                                                                                \
    extern const i_##__NAME##_input_stream_buffer_t __NAME;

        
#define EXTERN_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)                        \
            __EXTERN_INPUT_STREAM_BUFFER(__NAME, __BLOCK_SIZE)

#define END_EXTERN_INPUT_STREAM_BUFFER(__NAME)   
        
        
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{
DECLARE_CLASS(stream_buffer_block_t)
EXTERN_CLASS(stream_buffer_block_t)
    INHERIT(__single_list_node_t)
    uint32_t wBlockSize;
    union {
        uint32_t wSize;                                 //!< memory block
        uint32_t wBuffer;
    };
END_EXTERN_CLASS(stream_buffer_block_t)
//! @}


EXTERN_EPOOL(StreamBufferBlock, stream_buffer_block_t)

END_EXTERN_EPOOL(StreamBufferBlock)

//! \note no thread safe queue is required
EXTERN_QUEUE_U8(StreamBufferQueue, uint_fast16_t, bool)
    
END_EXTERN_QUEUE_U8(StreamBufferQueue)

//! \brief stream buffer control block
//! @{
DECLARE_CLASS(stream_buffer_t)

typedef void stream_buffer_req_event_t(stream_buffer_t *ptThis);


EXTERN_CLASS(stream_buffer_t, 
    WHICH(   
        INHERIT(pool_t)                                                 //!< inherit from pool StreamBufferBlock
        INHERIT(QUEUE(StreamBufferQueue))                               //!< inherit from queue StreamBufferQueue
    )) 

    bool                                    bIsOutput;                  //!< direction
    bool                                    bIsQueueInitialised;        //!< Indicate whether the queue has been inialised or not
    stream_buffer_block_t                  *ptListHead;                 //!< Queue Head
    stream_buffer_block_t                  *ptListTail;                 //!< Queue Tail
    stream_buffer_block_t                  *ptUsedByQueue;              //!< buffer block used by queue
    stream_buffer_block_t                  *ptUsedByOutside;            //!< buffer block lent out  
    stream_buffer_req_event_t              *fnRequestSend;              //!< callback for triggering the first output transaction
    stream_buffer_req_event_t              *fnRequestReceive;    

END_EXTERN_CLASS(stream_buffer_t)
//! @}

typedef struct {
    enum {
        INPUT_STREAM = 0,
        OUTPUT_STREAM
    } tDirection;
    
    stream_buffer_req_event_t              *fnRequestHandler;
    
}stream_buffer_cfg_t;

DEF_INTERFACE(i_stream_buffer_t)

    bool (*Init)(stream_buffer_t *, stream_buffer_cfg_t *);
    bool (*AddBuffer)(stream_buffer_t *, void *, uint_fast16_t , uint_fast16_t );
        
    struct {
        bool (*Read)(stream_buffer_t *, uint8_t *);
        bool (*Write)(stream_buffer_t *, uint8_t);
        void (*Flush)(stream_buffer_t *ptObj);
    } Stream;
    
    struct {
        void *(*Exchange)(stream_buffer_t *, void *);
    } Block;

END_DEF_INTERFACE(i_stream_buffer_t)
    
/*============================ GLOBAL VARIABLES ==============================*/
    
extern const i_stream_buffer_t STREAM_BUFFER;    

/*============================ PROTOTYPES ====================================*/




#endif
#endif
/* EOF */