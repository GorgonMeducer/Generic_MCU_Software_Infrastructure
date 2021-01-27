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
//#include ".\app_platform\app_platform.h"
#include "utilities/compiler.h"
#include "utilities/ooc.h"

#undef this
#define this    (*ptThis)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

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

/*============================ TYPES =========================================*/

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


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/






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
        top_menu_item_b_handler, 
        NULL, 
        "Top Menu B",
        "This is Top Menu B"
    )
    menu_item(
        top_menu_item_c_handler, 
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
    default_menu_item_t *ptItem;
    
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
                        ptItem = &((default_menu_item_t *)this.ptCurrentMenu->ptItems) [this.chCurrentItemIndex];
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
            ptItem = &((default_menu_item_t *)this.ptCurrentMenu->ptItems) [this.chCurrentItemIndex];
            fsm_rt_t tFSM = ptItem->fnHandle((menu_item_t *)ptItem);
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


static menu_engine_cb_t s_tMyMenu = {
    .ptCurrentMenu = &menu(TopMenu),
};

void example_menu_task(void)
{
    menu_task(&s_tMyMenu);
}
