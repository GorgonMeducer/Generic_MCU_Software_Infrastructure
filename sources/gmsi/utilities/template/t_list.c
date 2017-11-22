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
#include "..\ooc.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

def_structure( __single_list_node_t )
    __single_list_node_t     *ptNext;
end_def_structure( __single_list_note_t )

def_structure( __dual_list_node_t )
    __dual_list_node_t       *ptNext;
    __dual_list_node_t       *ptPrevious;
end_def_structure( __dual_list_note_t )


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


bool list_find(__single_list_node_t **ppList, __single_list_node_t *ptItem)
{
    bool bResult = false;
    
    do {
        if (NULL == ppList || NULL == ptItem) {
            break;
        } else if (NULL == (*ppList)) {
            break;
        }
    
        do {
            if ((*ppList) == ptItem) {
                bResult = true; 
                break;
            }
            
            ppList = &((*ppList)->ptNext);
        
        } while(NULL != (*ppList));
    
    } while(false);
    
    return bResult;
}
/*EOF*/

