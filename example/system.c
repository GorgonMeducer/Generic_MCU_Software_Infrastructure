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

/*============================ INCLUDES ======================================*/
#include ".\app_platform\app_platform.h"
#include <string.h>

/*============================ MACROS ========================================*/

#ifndef FRAME_BUFFER_SIZE
#   warning No defined FRAME_BUFFER_SIZE, default value 512 is used
#   define FRAME_BUFFER_SIZE                (512)
#endif

#ifndef DELAY_OBJ_POOL_SIZE
#   warning No defined DELAY_OBJ_POOL_SIZE, default value 4 is used
#   define DELAY_OBJ_POOL_SIZE              (4)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static volatile uint32_t    s_wMSTicks = 0;   
NO_INIT es_simple_frame_t   s_tFrame;
NO_INIT multiple_delay_t    s_tDelayService;
NO_INIT telegraph_engine_t  s_tTelegraphEngine;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------
  SysTick / Timer0 IRQ Handler
 *----------------------------------------------------------------------------*/

void SysTick_Handler (void) 
{
    /*! 1ms timer event handler */
    s_wMSTicks++;
    
    if (!(s_wMSTicks % 1000)) {
        static volatile uint16_t wValue = 0;

        //printf("%s [%08x]\r\n", "Hello world!", wValue++);
        
        //STREAM_OUT.Stream.Flush();
    }
    
    MULTIPLE_DELAY.Dependent.TimerTickService(&s_tDelayService);
    
    /*! call application platform 1ms event handler */
    app_platform_1ms_event_handler();
}



static void system_init(void)
{
    if (!app_platform_init()) {
        NVIC_SystemReset();
    }
        
    
    //! initialise multiple delay service
    do {
        NO_INIT static multiple_delay_item_t s_tDelayObjPool[DELAY_OBJ_POOL_SIZE];

        MULTIPLE_DELAY_CFG (    &s_tDelayService,
                                (uint8_t *)s_tDelayObjPool,
                                sizeof(s_tDelayObjPool)
                                
                            );
    } while(false);
    
    SysTick_Config(SystemCoreClock  / 1000);  //!< Generate interrupt every 1 ms 
}


#if DEMO_MULTIPLE_DELAY == ENABLED

static void app_2000ms_delay_timeout_event_handler(multiple_delay_report_status_t tStatus, void *pObj)
{
    static volatile uint16_t wValue = 0;

    printf("%s [%08x]\r\n", "Hello world!", wValue++);
        
    STREAM_OUT.Stream.Flush();
    
    //! request again
    MULTIPLE_DELAY.RequestDelay(&s_tDelayService, 
                                2000,                                           //!< request delay 2000ms
                                MULTIPLE_DELAY_LOW_PRIORITY,                    //!< priority is low
                                NULL,                                           //!< no tag
                                &app_2000ms_delay_timeout_event_handler);       //!< timout event handler
}


static void app_3000ms_delay_timeout_event_handler(multiple_delay_report_status_t tStatus, void *pObj)
{
    static volatile uint16_t wValue = 0;

    printf("%s [%08x]\r\n", "Apple!", wValue++);
        
    STREAM_OUT.Stream.Flush();
    
    //! request again
    MULTIPLE_DELAY.RequestDelay(&s_tDelayService, 
                                3000,                                           //!< request delay 3000ms
                                MULTIPLE_DELAY_LOW_PRIORITY,                    //!< priority is low
                                NULL,                                           //!< no tag
                                &app_3000ms_delay_timeout_event_handler);       //!< timout event handler
}

static void app_1500ms_delay_timeout_event_handler(multiple_delay_report_status_t tStatus, void *pObj)
{
    static volatile uint16_t wValue = 0;

    printf("%s [%08x]\r\n", "Orange!", wValue++);
        
    STREAM_OUT.Stream.Flush();
    
    //! request again
    MULTIPLE_DELAY.RequestDelay(&s_tDelayService, 
                                1500,                                           //!< request delay 1500ms
                                MULTIPLE_DELAY_NORMAL_PRIORITY,                 //!< priority is normal
                                NULL,                                           //!< no tag
                                &app_1500ms_delay_timeout_event_handler);       //!< timout event handler
}

#endif

#if DEMO_FRAME_USE_BLOCK_MODE == ENABLED
    
static block_t * frame_parser(block_t *ptBlock, void *ptObj)
{
    #define DEMO_STRING         "Hello world!\r\n"
    do {
        if (NULL == ptBlock) {
            break;
        }
        uint8_t *pchBuffer = BLOCK.Buffer.Get(ptBlock);
        memcpy(&pchBuffer[1], DEMO_STRING, sizeof(DEMO_STRING));
        BLOCK.Size.Set(ptBlock, sizeof(DEMO_STRING)+1);
        
    } while(false);
    
    
    return ptBlock;
}
#else
static uint_fast16_t frame_parser(mem_block_t tMemory, uint_fast16_t hwSize)
{
    return hwSize;
}
#endif



static void app_init(void)
{
    //! initialise simple frame service
    do {
        NO_INIT static uint8_t s_chFrameBuffer[FRAME_BUFFER_SIZE];
        NO_INIT static i_byte_pipe_t s_tPipe;
        s_tPipe.ReadByte = (STREAM_IN.Stream.ReadByte);
        s_tPipe.WriteByte = (STREAM_OUT.Stream.WriteByte);
        
    #if DEMO_FRAME_USE_BLOCK_MODE == ENABLED
    
        NO_INIT static union {
            block_t tBlock;
            uint8_t chBuffer[FRAME_BUFFER_SIZE + sizeof(block_t)];
        } s_tBuffer;
        BLOCK.Init(&s_tBuffer.tBlock, sizeof(s_tBuffer) - sizeof(block_t));
    #endif
        //! initialise simple frame service
        ES_SIMPLE_FRAME_CFG(    &s_tFrame, 
                                &s_tPipe,
                                
                            #if DEMO_FRAME_USE_BLOCK_MODE == ENABLED
                                &frame_parser,
                                .bStaticBufferMode = false,
                                .ptBlock = &s_tBuffer.tBlock,
                                .pTag = &s_tTelegraphEngine
                            #else
                                &frame_parser,
                                s_chFrameBuffer,
                                sizeof(s_chFrameBuffer)
                            #endif
                            );
    } while(false);
    
    
#if DEMO_MULTIPLE_DELAY == ENABLED
    MULTIPLE_DELAY.RequestDelay(&s_tDelayService, 
                                2000,                                           //!< request delay 2000ms
                                MULTIPLE_DELAY_LOW_PRIORITY,                    //!< priority is low
                                NULL,                                           //!< no tag
                                &app_2000ms_delay_timeout_event_handler);           //!< timout event handler
                                
    //! request again
    MULTIPLE_DELAY.RequestDelay(&s_tDelayService, 
                                3000,                                           //!< request delay 3000ns
                                MULTIPLE_DELAY_LOW_PRIORITY,                    //!< priority is low
                                NULL,                                           //!< no tag
                                &app_3000ms_delay_timeout_event_handler);       //!< timout event handler
                                
    //! request again
    MULTIPLE_DELAY.RequestDelay(&s_tDelayService, 
                                1500,                                           //!< request delay 1500ms
                                MULTIPLE_DELAY_NORMAL_PRIORITY,                 //!< priority is normal
                                NULL,                                           //!< no tag
                                &app_1500ms_delay_timeout_event_handler);       //!< timout event handler
#endif

}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
 
 #if __IS_COMPILER_ARM_COMPILER_6__
__asm(".global __use_no_semihosting\n\t");
__asm(".global __ARM_use_no_argv\n\t");

void _sys_exit(int ch)
{
    while(1);
}

void _ttywrch(int ch)
{

}

#include <rt_sys.h>

FILEHANDLE $Sub$$_sys_open(const char *name, int openmode)
{
    return 0;
}

#endif
 


typedef struct __menu_item  menu_item_t;
typedef struct __menu      menu_t;

typedef fsm_rt_t menu_item_handler_t(menu_item_t *);

struct __menu_item {
    menu_item_handler_t *fnHandle;                      //!< handler
    menu_t              *ptChild;                       //!< Child Menu
};

typedef struct __menu_engine_cb menu_engine_cb_t;
typedef fsm_rt_t menu_engine_t(menu_engine_cb_t *);

struct __menu {
    menu_item_t        *ptItems;                        //!< menu item list
    uint_fast8_t        chCount;                        //!< menu item count
    menu_t             *ptParent;                       //!< parent menu;
    menu_engine_t      *fnEngine;                       //!< engine for process current menu
}; 

struct __menu_engine_cb {
    uint_fast8_t    tState;
    const menu_t    *ptCurrentMenu;
    uint_fast8_t    chCurrentItemIndex;
};


#define __declare_menu_item_template(__NAME)                                    \
    typedef struct __##__NAME __NAME;
#define declare_menu_item_template(__NAME)                                      \
        __declare_menu_item_template(__NAME)
    
#define __def_menu_item_template(__NAME)                                        \
    struct __##__NAME {                                                         \
        menu_item_t;                                                            
#define def_menu_item_template(__NAME)                                          \
            __def_menu_item_template(__NAME)                                    

#define end_def_menu_item_template(__NAME)                                      \
    };

#define __def_menu(__NAME, __PARENT, __ENGINE, __TEMPLATE)                      \
extern const menu_t c_tMenu##__NAME;                                            \
__TEMPLATE c_tMenu##__NAME##Items[] = {
#define def_menu(__NAME, __PARENT, __ENGINE)                                    \
            __def_menu(__NAME, (__PARENT), (__ENGINE), default_menu_item_t)
            
#define def_menu_ex(__NAME, __PARENT, __ENGINE, __TEMPLATE)                     \
            __def_menu(__NAME, (__PARENT), (__ENGINE), __TEMPLATE)

#define __end_def_menu(__NAME, __PARENT, __ENGINE, __TEMPLATE)                  \
    };                                                                          \
    const menu_t c_tMenu##__NAME = {                                            \
        (menu_item_t *)c_tMenu##__NAME##Items,                                  \
        (sizeof(c_tMenu##__NAME##Items)/sizeof(__TEMPLATE)),                    \
        (menu_t *)(__PARENT),                                                   \
        (__ENGINE),                                                             \
    };
#define end_def_menu(__NAME, __PARENT, __ENGINE)                                \
            __end_def_menu(__NAME, (__PARENT), (__ENGINE), default_menu_item_t)
#define end_def_menu_ex(__NAME, __PARENT, __ENGINE, __TEMPLATE)                 \
            __end_def_menu(__NAME, (__PARENT), (__ENGINE), __TEMPLATE)
            
#define __extern_menu(__NAME)   extern const menu_t c_tMenu##__NAME;
#define extern_menu(__NAME)     __extern_menu(__NAME)

#define __menu(__NAME)          c_tMenu##__NAME
#define menu(__NAME)            __menu(__NAME)            
          
#define __menu_item(__HANDLER, __CHILD_MENU, ...)                               \
    {                                                                           \
        (__HANDLER),                                                            \
        (menu_t *)(__CHILD_MENU),                                               \
        __VA_ARGS__,                                                            \
    },
#define menu_item(__HANDLER, __CHILD_MENU, ...)                                 \
            __menu_item((__HANDLER), (__CHILD_MENU), __VA_ARGS__)


declare_menu_item_template(default_menu_item_t)

def_menu_item_template(default_menu_item_t)
    //! depends on your application, you can add/remove/change following members
    char                *pchTitle;                      //!< Menu Title
    char                *pchDescription;                //!< Description for this menu item
    char                chShortCutKey;                  //!< Shortcut Key value in current menu
end_def_menu_item_template(default_menu_item_t)


extern fsm_rt_t default_menu_engine(menu_engine_cb_t *ptMenu);

extern fsm_rt_t top_menu_item_a_handler(menu_item_t *ptItem);
extern fsm_rt_t top_menu_item_b_handler(menu_item_t *ptItem);
extern fsm_rt_t top_menu_item_c_handler(menu_item_t *ptItem);

extern_menu(lv2_menu_A)

def_menu(TopMenu, NULL, default_menu_engine)
    menu_item(
        top_menu_item_a_handler, 
        &menu(lv2_menu_A), 
        "Top Menu A",
        "This is Top Menu A"
    )
    menu_item(
        top_menu_item_a_handler, 
        NULL, 
        "Top Menu B",
        "This is Top Menu B"
    )
    menu_item(
        top_menu_item_a_handler, 
        NULL, 
        "Top Menu C",
        "This is Top Menu C"
    )
end_def_menu(TopMenu, NULL, default_menu_engine)


fsm_rt_t top_menu_item_a_handler(menu_item_t *ptItem)
{
    return fsm_rt_cpl;
}

fsm_rt_t top_menu_item_b_handler(menu_item_t *ptItem)
{
    return fsm_rt_cpl;
}

fsm_rt_t top_menu_item_c_handler(menu_item_t *ptItem)
{
    return fsm_rt_cpl;
}

typedef enum {
    KEY_NULL = 0,
    KEY_DOWN,
    KEY_UP,
    KEY_ENTER,
    KEY_ESC,
} key_t;

extern key_t get_key(void);

fsm_rt_t default_menu_engine(menu_engine_cb_t *ptThis)
{
#define DEFAULT_MENU_ENGINE_RESET_FSM() \
    do { this.tState = START; } while(0)

    enum {
        START = 0,
        READ_KEY_EVENT, 
        KEY_PROCESS, 
        RUN_ITEM_HANDLER
    };
    key_t tKey;
    menu_item_t *ptItem;
    
    switch(this.tState) {
        case START:
            this.tState++;
        case READ_KEY_EVENT: 
            tKey = get_key();
            if (KEY_NULL == tKey) {
                break;
            }

            
        case KEY_PROCESS:

            switch (tKey) {
                case KEY_DOWN:
                    this.chCurrentItemIndex++;
                    if (this.chCurrentItemIndex >= this.ptCurrentMenu->chCount) {
                        this.chCurrentItemIndex = 0;
                    }
                    break;
                case KEY_UP:
                    if (0 == this.chCurrentItemIndex) {
                        this.chCurrentItemIndex = this.ptCurrentMenu->chCount - 1;
                    }
                    break;
                case KEY_ENTER: {
                        ptItem = &(this.ptCurrentMenu->ptItems[this.chCurrentItemIndex]);
                        if (NULL != ptItem->fnHandle) {
                            this.tState = RUN_ITEM_HANDLER;
                        } else if (NULL != ptItem->ptChild) {
                            this.ptCurrentMenu = ptItem->ptChild;
                            this.chCurrentItemIndex = 0;
                            
                            DEFAULT_MENU_ENGINE_RESET_FSM();
                            return fsm_rt_cpl;
                        }
                    }
                    break;
                case KEY_ESC:
                
                    //! return to upper menu
                    if (NULL != this.ptCurrentMenu->ptParent) {
                        this.ptCurrentMenu = this.ptCurrentMenu->ptParent;
                        this.chCurrentItemIndex = 0;
                            
                        DEFAULT_MENU_ENGINE_RESET_FSM();
                        return fsm_rt_cpl;
                    }
                    break;
                default:
                    break;
            }
            break;
            
        case RUN_ITEM_HANDLER:
            ptItem = &(this.ptCurrentMenu->ptItems[this.chCurrentItemIndex]);
            fsm_rt_t tFSM = ptItem->fnHandle(ptItem);
            if (IS_FSM_ERR(tFSM)) {
                //! report error
                DEFAULT_MENU_ENGINE_RESET_FSM();
                return tFSM;
            } else if (fsm_rt_cpl == tFSM) {
                DEFAULT_MENU_ENGINE_RESET_FSM();
                return fsm_rt_cpl;
            }
            break;
    }

    return fsm_rt_on_going;
}

extern fsm_rt_t lv2_menu_item_a_handler(menu_item_t *ptItem);

def_menu(lv2_menu_A, &menu(TopMenu), default_menu_engine)
    menu_item(
        lv2_menu_item_a_handler, 
        NULL, 
        "Lv2 Menu A",
        "This is Lv2 Menu A"
    )
end_def_menu(lv2_menu_A, &menu(TopMenu), default_menu_engine)

 
 
fsm_rt_t lv2_menu_item_a_handler(menu_item_t *ptItem)
{
    return fsm_rt_cpl;
}

key_t get_key(void)
{
    return KEY_NULL;
}
 
fsm_rt_t menu_task(menu_engine_cb_t *ptThis)
{
    do {
        /* this validation code could be removed for release version */
        if (NULL == ptThis) {
            break;
        } else if (NULL == this.ptCurrentMenu) {
            break;
        } else if (NULL == this.ptCurrentMenu->fnEngine) {
            break;
        } else if (NULL == this.ptCurrentMenu->ptItems) {
            break;
        } else if (0 == this.ptCurrentMenu->chCount) {
            break;
        }
        
        return this.ptCurrentMenu->fnEngine(ptThis);
        
    } while(false);
    
    return fsm_rt_err;
}
 
int main (void) 
{
    system_init();
    app_init();
    
    while (true) {
    
    #if false
        if (fsm_rt_cpl == ES_SIMPLE_FRAME.Task(&s_tFrame)) {
            STREAM_OUT.Stream.Flush();
        }
    #else
        uint8_t chByte;
        if (STREAM_IN.Stream.ReadByte(&chByte)) {
            STREAM_OUT.Stream.WriteByte(chByte);
        } else {
            STREAM_OUT.Stream.Flush();
        }
    #endif

        //MULTIPLE_DELAY.Task(&s_tDelayService);
        
    }
}




