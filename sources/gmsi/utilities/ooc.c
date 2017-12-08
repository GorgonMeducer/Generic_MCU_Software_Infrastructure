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
#ifndef __STORE_ENVIRONMENT_CFG_IN_PROJ__
#include "..\..\environment_cfg.h"
#endif

#include ".\compiler.h"

/*============================ MACROS ========================================*/
#ifndef this
#   define this             (*ptThis)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name general event handler
//! @{
//DEF_CLASS(DELEGATE_HANDLE)
typedef struct __delegate_handler CLASS(DELEGATE_HANDLE);
struct __delegate_handler {
    DELEGATE_HANDLE_FUNC    *fnHandler;         //!< event handler
    void                    *pArg;              //!< Argument
    CLASS(DELEGATE_HANDLE)  *ptNext;            //!< next 
};
//END_DEF_CLASS(DELEGATE_HANDLE)
//! @}

//! \name event
//! @{
//EXTERN_CLASS(DELEGATE)
typedef struct __delegate CLASS(DELEGATE);
struct __delegate {
    DELEGATE_HANDLE *ptEvent;
    DELEGATE_HANDLE *ptBlockedList;
    CLASS(DELEGATE_HANDLE) **pptHandler;
};
//END_EXTERN_CLASS(DELEGATE)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize event 
 *! \param ptEvent target event
 *! \return the address of event item
 */
DELEGATE *delegate_init(DELEGATE *ptEvent)
{
    class_internal(ptEvent, ptThis, DELEGATE);

    do {
        if (NULL == ptThis) {
            break;
        }

        this.ptEvent = NULL;
        this.ptBlockedList = NULL;
        this.pptHandler = (CLASS(DELEGATE_HANDLE) **)&(this.ptEvent);
        
    } while (0);

    return ptEvent;
}

/*! \brief initialize event handler item
 *! \param ptHandler the target event handler item
 *! \param fnRoutine event handler routine
 *! \param pArg handler extra arguments
 *! \return the address of event handler item
 */
DELEGATE_HANDLE *delegate_handler_init(
    DELEGATE_HANDLE *ptHandler, DELEGATE_HANDLE_FUNC *fnRoutine, void *pArg)
{
    
    class_internal(ptHandler, ptThis, DELEGATE_HANDLE);
    
    if (NULL == ptHandler || NULL == fnRoutine) {
        return NULL;
    }
    this.fnHandler = fnRoutine;
    this.pArg = pArg;
    this.ptNext = NULL;

    return ptHandler;
}


static CLASS(DELEGATE_HANDLE) **search_list(
    CLASS(DELEGATE_HANDLE) **pptHandler, CLASS(DELEGATE_HANDLE) *ptHND)
{
    //! search event handler chain
    while (NULL != (*pptHandler)) {
        if ((*pptHandler) == ptHND) {
            return pptHandler;
        }
        pptHandler = &((*pptHandler)->ptNext);      //!< get next item
    }
    return NULL;
}

/*! \brief register event handler to specified event
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
gsf_err_t register_delegate_handler(DELEGATE *ptEvent, DELEGATE_HANDLE *ptHandler)
{
    class_internal(ptEvent, ptThis, DELEGATE);
    class_internal(ptHandler, ptHND, DELEGATE_HANDLE);

    if ((NULL == ptEvent) || (NULL == ptHandler) || (NULL == ptHND->fnHandler)) {
        return GSF_ERR_INVALID_PTR;
    } else if (NULL != ptHND->ptNext) {     
        //! search ready list
        class(DELEGATE_HANDLE) **pptHandler = search_list(   
            (class(DELEGATE_HANDLE) **)&(this.ptBlockedList), ptHND );

        if (NULL != pptHandler) {
            //! safe to remove
            (*pptHandler) = ptHND->ptNext;
            ptHND->ptNext = NULL;
        } else {        
            return GSF_ERR_REQ_ALREADY_REGISTERED;
        }
    }

    //! add handler to the ready list
    ptHND->ptNext = (class(DELEGATE_HANDLE) *)(this.ptEvent);
    this.ptEvent = ptHandler;

    return GSF_ERR_NONE;
}



/*! \brief unregister a specified event handler
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
gsf_err_t unregister_delegate_handler( DELEGATE *ptEvent, DELEGATE_HANDLE *ptHandler)
{
    class_internal(ptEvent, ptThis, DELEGATE);
    class_internal(ptHandler, ptHND, DELEGATE_HANDLE);
    
    class(DELEGATE_HANDLE) **pptHandler;
    if ((NULL == ptEvent) || (NULL == ptHandler)) {
        return GSF_ERR_INVALID_PTR;
    } 

    do {
        //! search ready list
        pptHandler = search_list(   (CLASS(DELEGATE_HANDLE) **)&(this.ptEvent), 
                                    ptHND );
        if (NULL != pptHandler) {
            //! safe to remove
            (*pptHandler) = ptHND->ptNext;
            ptHND->ptNext = NULL;
            if (this.pptHandler == &(ptHND->ptNext)) {
                this.pptHandler = pptHandler;
            }
            break;
        }
        //! search ready list
        pptHandler = search_list(   (class(DELEGATE_HANDLE) **)&(this.ptBlockedList), 
                                    ptHND );
        if (NULL != pptHandler) {
            //! safe to remove
            (*pptHandler) = ptHND->ptNext;
            ptHND->ptNext = NULL;
            if (this.pptHandler == &(ptHND->ptNext)) {
                this.pptHandler = pptHandler;
            }
            break;
        }
    } while(false);
    
    return GSF_ERR_NONE;
}

static fsm_rt_t __move_to_block_list(class(DELEGATE) *ptThis, class(DELEGATE_HANDLE) *ptHandler)
{
    class(DELEGATE_HANDLE) *ptHND = ptHandler;
    //! remove handler from ready list
    (*this.pptHandler) = ptHND->ptNext;
    //! add handler to block list
    ptHND->ptNext = (class(DELEGATE_HANDLE) *)this.ptBlockedList;
    this.ptBlockedList = (DELEGATE_HANDLE *)ptHND;

    if (NULL == this.ptEvent) {
        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

#define RAISE_EVENT_START               0
#define RAISE_EVENT_CHECK_HANDLER       1
#define RAISE_EVENT_RUN_HANDLER         2
#define RAISE_EVENT_RESET_FSM()     do { this.chState = 0; } while (0)

/*! \brief raise target event
 *! \param ptEvent the target event
 *! \param pParam event parameter
 *! \return access result
 */
fsm_rt_t invoke_delegate( DELEGATE *ptEvent, void *pParam)
{
    class_internal(ptEvent, ptThis, DELEGATE);
    if (NULL == ptThis) {
        return (fsm_rt_t)GSF_ERR_INVALID_PTR;
    }

    if (NULL == this.ptEvent) {
        if (NULL == this.ptBlockedList) {
            //! nothing to do
            return fsm_rt_cpl;
        }
        
        //! initialize state
        this.ptEvent = this.ptBlockedList;
        this.ptBlockedList = NULL;
        this.pptHandler = (class(DELEGATE_HANDLE) **)&(this.ptEvent);
    } 

    if (NULL == (*this.pptHandler)) {
        //! finish visiting the ready list
        this.pptHandler = (class(DELEGATE_HANDLE) **)&(this.ptEvent);
        if (NULL == (*this.pptHandler)) {
            //! complete
            return fsm_rt_cpl;
        }
    } else {
        class(DELEGATE_HANDLE) *ptHandler = (*this.pptHandler);
        
        if (NULL != ptHandler->fnHandler) {
            //! run the event handler
            fsm_rt_t tFSM = ptHandler->fnHandler(ptHandler->pArg,pParam);

            if (fsm_rt_on_going == tFSM) { 
                this.pptHandler = &(ptHandler->ptNext);    //!< get next item
            } else if (EVENT_RT_UNREGISTER == tFSM) {
                //! return EVENT_RT_UNREGISTER means event handler could be removed
                class(DELEGATE_HANDLE) *ptHND = ptHandler;
                (*this.pptHandler) = ptHND->ptNext;
                ptHND->ptNext = NULL;
            } else {
                return __move_to_block_list(ptThis, ptHandler);
            }
        } else {
            return __move_to_block_list(ptThis, ptHandler);
        }
    }

    return fsm_rt_on_going;
}




/* EOF */

