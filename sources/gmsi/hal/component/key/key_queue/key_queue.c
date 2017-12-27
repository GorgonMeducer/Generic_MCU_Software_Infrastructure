/***************************************************************************
 *   Copyright(C)2013-2014 by TanekLiang<y574824080@gmail.com>              *
 *                                                                          *
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

#include ".\app_cfg.h"

#if USE_COMPONENT_KEY == ENABLED
#include "..\key_interface.h"

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DECLARE_CLASS(key_queue_t)
//! \name key queue interface
//! @{
DEF_CLASS(key_queue_t)
	key_t *ptBuffer;
	uint16_t hwSize;
	uint16_t hwHead;
	uint16_t hwTail;
	uint16_t hwLength;
END_DEF_CLASS(key_queue_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief init key queue
 *! \param ptQueue address of the target queue object
 *! \param ptKeyBuffer address of the key message buffer
 *! \param size key size
 *! \return true key queue initialization succeed
 *! \return false key queue initialization failed
 */
bool key_queue_init(key_queue_t * ptQueue, key_t *ptKeyBuffer, uint16_t hwsize)
{
	CLASS(key_queue_t) *ptQ = (CLASS(key_queue_t) *)ptQueue;
    bool bReturn = false;

    SAFE_ATOM_CODE(
        if((NULL != ptQ) && (NULL != ptKeyBuffer)){
            ptQ->ptBuffer = ptKeyBuffer;
            ptQ->hwSize = hwsize;
            ptQ->hwHead = 0;
            ptQ->hwTail = 0;
            ptQ->hwLength = 0;                    
            bReturn = true;                     //!< update return value
        }                   
    );

	return bReturn;
}

/*! \brief enqueue 
 *! \param ptQueue address of the target queue object
 *! \param ptKeyBuffer address of the key message
 *! \return true key event enqueue succeed
 *! \return false key event enqueue failed
 */
bool key_enqueue(key_queue_t* ptQueue, key_t* ptKey)
{
	CLASS(key_queue_t) *ptQ = (CLASS(key_queue_t) *)ptQueue;
    bool bReturn = false;
	
	if ((NULL == ptQueue) && (NULL == ptKey)) {
		return false;
	}

    SAFE_ATOM_CODE(
        if(ptQ->hwSize != ptQ->hwLength){       //!< the queue full
            //! add data to queue tail
            ptQ->ptBuffer[ptQ->hwTail] = *ptKey;            
            ptQ->hwLength++;                    //!< update length            
            if(ptQ->hwSize == ++ptQ->hwTail){   //!< update queue tail pointer
                ptQ->hwTail = 0;
            }   
            bReturn = true;                     //!< update return value
        }                        
    );    
	
	return bReturn;
}

/*! \brief dequeue 
 *! \param ptQueue address of the target queue object
 *! \param ptKeyBuffer address of the key message
 *! \return true key event dequeue succeed
 *! \return false key event dequeue failed
 */
bool key_dequeue(key_queue_t* ptQueue, key_t* ptKey)
{
	CLASS(key_queue_t) *ptQ = (CLASS(key_queue_t) *)ptQueue;
    bool bReturn = false;
	
	if ((NULL == ptQueue) && (NULL == ptKey)) {
		return false;
	}
	
    SAFE_ATOM_CODE(
        if(0 != ptQ->hwLength){                 //!< the queue empty
            //! read date form queue head
            *ptKey = ptQ->ptBuffer[ptQ->hwHead];        
            ptQ->hwLength--;                    //!< update length        
            if(ptQ->hwSize == ++ptQ->hwHead){   //!< update queue head pointer
                 ptQ->hwHead = 0;
            }	             
            bReturn = true;                     //!< update return value
        }
    );
		
	return bReturn;	
}

#endif
/* EOF */
