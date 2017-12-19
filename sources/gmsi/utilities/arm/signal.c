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
#include "..\..\..\environment_cfg.h"
#endif

#include "..\compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize a locker
 *! \param ptLock locker object
 *! \return none
 */
void init_lock(locker_t *ptLock)
{
    if (NULL == ptLock) {
        return ;
    }
    SAFE_ATOM_CODE (
        (*ptLock) = UNLOCKED;
    )
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
    SAFE_ATOM_CODE(
        if (!(*ptLock)) {
            (*ptLock) = LOCKED;
            bResult = true;
        }
    )
        
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
    SAFE_ATOM_CODE(
        (*ptLock) = UNLOCKED;
    )
}

/*! \brief get locker status
 *! \param ptLock locker object
 *! \return locker status
 */
bool check_lock(locker_t *ptLock)
{
    bool bResult = UNLOCKED;
    if (NULL == ptLock) {
        return false;
    }
    
    SAFE_ATOM_CODE(
        bResult = (*ptLock);
    )
    
    return bResult;
}

/* EOF */

