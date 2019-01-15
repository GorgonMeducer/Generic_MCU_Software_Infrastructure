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
#include "../compiler.h"
#include "../ooc.h"
#include "../app_type.h"
#include "signal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/




/*============================ PROTOTYPES ====================================*/
static void __default_code_region_atom_code_on_enter(void *pObj, void *pLocal);
static void __default_code_region_atom_code_on_leave(void *pObj,void *pLocal);
static void __default_code_region_none_on_enter(void *pObj, void *pLocal);
static void __default_code_region_none_on_leave(void *pObj,void *pLocal);
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/


static const i_code_region_t i_DefaultCodeRegionAtomCode = {
    .chLocalObjSize =   sizeof(istate_t),
    .OnEnter =          &__default_code_region_atom_code_on_enter,
    .OnLeave =          &__default_code_region_atom_code_on_leave,
};

static const i_code_region_t i_DefaultCodeRegionNone = {
    .chLocalObjSize = 4,
    .OnEnter =          &__default_code_region_none_on_enter,
    .OnLeave =          &__default_code_region_none_on_leave,
};

const code_region_t DEFAULT_CODE_REGION_ATOM_CODE = {
    .pTarget = NULL,
    .ptMethods = (i_code_region_t *)&i_DefaultCodeRegionAtomCode,
};

const code_region_t DEFAULT_CODE_REGION_NONE = {
    .pTarget = NULL,
    .ptMethods = (i_code_region_t *)&i_DefaultCodeRegionNone,
};

/*============================ IMPLEMENTATION ================================*/

static void __default_code_region_atom_code_on_enter(void *pObj, void *pLocal)
{
    istate_t *ptState = (istate_t *)pLocal;
    assert(NULL != pLocal);
    (*ptState) = DISABLE_GLOBAL_INTERRUPT();
}

static void __default_code_region_atom_code_on_leave(void *pObj,void *pLocal)
{
    istate_t *ptState = (istate_t *)pLocal;
    assert(NULL != pLocal);
    SET_GLOBAL_INTERRUPT_STATE(*ptState);
}

static void __default_code_region_none_on_enter(void *pObj, void *pLocal)
{
}

static void __default_code_region_none_on_leave(void *pObj,void *pLocal)
{
}

/*! \brief initialize a locker
 *! \param ptLock locker object
 *! \return none
 */
void init_lock(locker_t *ptLock)
{
    if (NULL == ptLock) {
        return ;
    }
    
    (*ptLock) = UNLOCKED;
}

/*! \brief try to enter a section
 *! \param ptLock locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
bool enter_lock(locker_t *ptLock)
{
    bool bResult = false;
    if (NULL == ptLock) {
        return true;
    }
    if (UNLOCKED == (*ptLock)) {
        SAFE_ATOM_CODE(){
            if (UNLOCKED == (*ptLock)) {
                (*ptLock) = LOCKED;
                bResult = true;
            }
        };
    }
        
    return bResult;
}


/*! \brief leave a section
 *! \param ptLock locker object
 *! \return none
 */
void leave_lock(locker_t *ptLock)
{
    if (NULL == ptLock) {
        return ;
    }
    
    (*ptLock) = UNLOCKED;
}

/*! \brief get locker status
 *! \param ptLock locker object
 *! \return locker status
 */
bool check_lock(locker_t *ptLock)
{
    if (NULL == ptLock) {
        return false;
    }

    return (*ptLock);
}

/* EOF */

