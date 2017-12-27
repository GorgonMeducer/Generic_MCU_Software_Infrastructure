/****************************************************************************
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

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_COMPONENT_KEY == ENABLED

#include ".\key_interface.h"
#include ".\key_queue\key_queue.h"

/*============================ MACROS ========================================*/
//! \brief Key Long Press Counts
#ifndef KEY_LONG_TIME
#warning You haven't define the MAXIMUM number of Key Long Press Counters, \
Default value 100 is used. You should define it with macro KEY_LONG_TIME.
#define KEY_LONG_TIME  (100u)
#endif

//! \brief Key Repeat Press Counts
#ifndef KEY_REPEAT_TIME
#warning You haven't define the MAXIMUM number of Key Repeat Press Counters, \
Default value 100 is used. You should define it with macro KEY_REPEAT_TIME.
#define KEY_REPEAT_TIME  (100u)
#endif

//! \brief Key Queue Size
#ifndef KEY_QUEUE_SIZE
#warning You haven't define the MAXIMUM number of Key Queue Buffer Size Counters, \
Default value 16 is used. You should define it with macro KEY_QUEUE_SIZE.
#define KEY_QUEUE_SIZE  (16u)
#endif

//! \brief Key Counter
#ifndef KEY_COUNT
#warning You haven't define the MAXIMUM number of Key Press Counters, \
Default value 100 is used. You should define it with macro KEY_COUNT.
#define KEY_COUNT  (100u)
#endif

//! \brief get key scan value
#ifndef GET_KEY_SCAN_VALUE
#error   You haven't define the function of key value scan, the function prototype should be \
uint8_t GET_KEY_SCAN_VALUE(void), and should define it with macro GET_KEY_SCAN_VALUE.
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
//! \brief Key Frontend Message Buffer
static key_t s_tKeyFrontendBuffer[KEY_QUEUE_SIZE];
//! \brief Key Detector message Buffer
static key_t s_tKeyDetectorBuffer[KEY_QUEUE_SIZE];

//! \brief Key Frontend Queue 
static key_queue_t s_tKeyFrontendQueue;
//! \brief Key Detector Queue 
static key_queue_t s_tKeyDetectorQueue;

/*============================ PROTOTYPES ====================================*/

#define CHECK_KEY_START 					0
#define CHECK_KEY_RESET_FSM()    			do{s_chState = 0;}while(0)
#define CHECK_KEY_CHECKING					1
#define CHECK_KEY_SCAN	    				2
#define CHECK_KEY_FINISH					3

/*! \brief check key value
 *! \param pchKey key value
 *! \return state machine status
 */
static fsm_rt_t check_key(uint8_t* pchKey)
{
    static uint8_t s_chState = CHECK_KEY_START;
    static uint16_t s_hwCount= 0;
    static uint8_t s_chKeyValue = KEY_NULL;
    static uint8_t s_chHisKeyValue = KEY_NULL;

    switch (s_chState) {
        case CHECK_KEY_START:
            s_hwCount= 0;
            s_chState = CHECK_KEY_CHECKING;
            //break;

        case CHECK_KEY_CHECKING:
            s_chKeyValue = GET_KEY_SCAN_VALUE();
            if (s_chKeyValue == s_chHisKeyValue) {
                s_hwCount++;
                s_chState = CHECK_KEY_FINISH;
            } else {
                s_chState = CHECK_KEY_SCAN;
            }
            break;                        

        case CHECK_KEY_SCAN:
            if (0 == s_hwCount) {
                s_chHisKeyValue = s_chKeyValue;
            } else {
                s_hwCount--;
            }
            s_chState = CHECK_KEY_CHECKING;
            break;

        case CHECK_KEY_FINISH:
            if (KEY_COUNT > s_hwCount) {
                s_chState = CHECK_KEY_CHECKING;
            } else {
                *pchKey = s_chKeyValue;
                CHECK_KEY_RESET_FSM();
                return fsm_rt_cpl;
            }
            break;

    }

    return fsm_rt_on_going;
}

#define KEY_FRONTEND_START 							0
#define KEY_FRONTEND_RESET_FSM()	    			do{s_chState = 0;}while(0)
#define KEY_FRONTEND_CHECK			    			1
#define KEY_FRONTEND_SAME			     			2
#define KEY_FRONTEND_EDGE			     			3


/*! \brief key frontend
 *! \param none
 *! \return state machine status
 */
static bool key_frontend(void)
{
	static uint8_t s_chState = KEY_FRONTEND_START;
	static uint8_t s_chNowKeyValue = KEY_NULL;
	static uint8_t s_chLastKeyValue = KEY_NULL;
	key_t tKey = {KEY_NULL, KEY_UP};
	
	switch (s_chState) {
		case KEY_FRONTEND_START:
			s_chState = KEY_FRONTEND_CHECK;
			//break;
			
		case KEY_FRONTEND_CHECK:
			if (!check_key(&s_chNowKeyValue)) {
				s_chState = KEY_FRONTEND_SAME;
			}		
			break;
			
		case KEY_FRONTEND_SAME:
			if (s_chLastKeyValue == s_chNowKeyValue) {
				s_chState = KEY_FRONTEND_CHECK;
			} else {
				s_chState = KEY_FRONTEND_EDGE;
			}
			break;
			
		case KEY_FRONTEND_EDGE:
			if (0 != s_chLastKeyValue) {
				tKey.tEvent = KEY_UP;
				tKey.chKeyValue = s_chLastKeyValue;
				key_enqueue(&s_tKeyFrontendQueue, &tKey);
			}
			
			if (0 != s_chNowKeyValue) {
				tKey.tEvent = KEY_DOWN;
				tKey.chKeyValue = s_chNowKeyValue;
				key_enqueue(&s_tKeyFrontendQueue, &tKey);
			}
			s_chLastKeyValue = s_chNowKeyValue;
			
			KEY_FRONTEND_RESET_FSM();
			return fsm_rt_cpl;
			break;
	}
	
	return fsm_rt_on_going;
}

#define KEY_DECETOR_START 							0
#define KEY_DECETOR_RESET_FSM()	    				do{s_chState = 0;}while(0)
#define KEY_DECETOR_DEQUEUE1			    		1
#define KEY_DECETOR_DOWN			    			2
#define KEY_DECETOR_DEQUEUE2			     		3
#define KEY_DECETOR_IS_UP_1			     			4
#define KEY_DECETOR_LONG_TIME		     			5
#define KEY_DECETOR_DEQUEUE3			    		6
#define KEY_DECETOR_REPEAT_TIME		    			7
#define KEY_DECETOR_REPEAT_ENQUEUE		    		8
#define KEY_DECETOR_IS_UP_2			     			9

/*! \brief key frontend
 *! \param none
 *! \return state machine status
 */
static fsm_rt_t key_decetor(void)
{
	static uint8_t s_chState = KEY_DECETOR_START;
	static uint16_t s_hwLongCount = 0;
	static uint16_t s_hwRepeatCount = 0;
	
	static key_t s_tKeyDown;
	static key_t s_tKeyUp;
	
	switch (s_chState) {
		case KEY_DECETOR_START:
			s_hwLongCount = 0;
			s_hwRepeatCount = 0;
			s_chState = KEY_DECETOR_DEQUEUE1;			
			//break;
			
		case KEY_DECETOR_DEQUEUE1:
			if (key_dequeue(&s_tKeyFrontendQueue, &s_tKeyDown)) {
				s_chState = KEY_DECETOR_DOWN;	
			} /* else {
				
			} */				
			break;	
			
		case KEY_DECETOR_DOWN:
			if (KEY_DOWN == s_tKeyDown.tEvent) {
				key_enqueue(&s_tKeyDetectorQueue, &s_tKeyDown);
				s_chState = KEY_DECETOR_DEQUEUE2;	
			} else {
				s_chState = KEY_DECETOR_DEQUEUE1;
			} 				
			break;
			
		case KEY_DECETOR_DEQUEUE2:
			if (key_dequeue(&s_tKeyFrontendQueue, &s_tKeyUp)) {
				s_chState = KEY_DECETOR_IS_UP_1;	
			} else {
				s_hwLongCount++; 
				s_chState = KEY_DECETOR_LONG_TIME;
			}				
			break;
			
		case KEY_DECETOR_IS_UP_1:
			if (KEY_UP != s_tKeyUp.tEvent) {
				s_chState = KEY_DECETOR_DEQUEUE2;	
			} else {
				key_enqueue(&s_tKeyDetectorQueue, &s_tKeyUp);
				s_tKeyUp.tEvent = KEY_PRESSED;
				key_enqueue(&s_tKeyDetectorQueue, &s_tKeyUp);
				KEY_DECETOR_RESET_FSM();
				return fsm_rt_cpl;
			}				
			break;
		
		case KEY_DECETOR_LONG_TIME:
			if (KEY_LONG_TIME > s_hwLongCount) {
				s_chState = KEY_DECETOR_DEQUEUE2;	
			} else {
				s_tKeyDown.tEvent = KEY_LONG_PRESSED;
				key_enqueue(&s_tKeyDetectorQueue, &s_tKeyDown);
				s_chState = KEY_DECETOR_DEQUEUE3;
			}				
			break;
			
		case KEY_DECETOR_DEQUEUE3:
			if (key_dequeue(&s_tKeyFrontendQueue, &s_tKeyUp)) {
				s_chState = KEY_DECETOR_IS_UP_2;	
			} else {
				s_hwRepeatCount++; 
				s_chState = KEY_DECETOR_REPEAT_TIME;
			}				
			break;
			
		case KEY_DECETOR_IS_UP_2:
			if (KEY_UP != s_tKeyUp.tEvent) {
				s_chState = KEY_DECETOR_DEQUEUE3;	
			} else {
				key_enqueue(&s_tKeyDetectorQueue, &s_tKeyUp);
				KEY_DECETOR_RESET_FSM();
				return fsm_rt_cpl;
			}				
			break;			
			
		case KEY_DECETOR_REPEAT_TIME:
			if (KEY_REPEAT_TIME > s_hwRepeatCount) {
				s_chState = KEY_DECETOR_DEQUEUE3;	
			} else {
				s_hwRepeatCount = 0;
				s_chState = KEY_DECETOR_REPEAT_ENQUEUE;
			}				
			break;
			
		case KEY_DECETOR_REPEAT_ENQUEUE:		
			s_tKeyDown.tEvent = KEY_REPEAT;
			key_enqueue(&s_tKeyDetectorQueue, &s_tKeyDown);
			s_chState = KEY_DECETOR_DEQUEUE3;			
			break;						
			
	}
		
	return fsm_rt_on_going;
}


/*! \brief key task initialization
 *! \param none
 *! \return none
 */
void key_init(void)
{
	key_queue_init(&s_tKeyFrontendQueue, s_tKeyFrontendBuffer, KEY_QUEUE_SIZE);
	key_queue_init(&s_tKeyDetectorQueue, s_tKeyDetectorBuffer, KEY_QUEUE_SIZE);
}

/*! \brief key task
 *! \param none
 *! \return none
 */
void key_task(void)
{
	key_frontend();
	key_decetor();
}

/*! \brief get key
 *! \param ptKey Key Event
 *! \return true get key succeed
 *! \return false get key failed
 */
bool get_key(key_t* ptKey)
{
	return key_dequeue(&s_tKeyDetectorQueue, ptKey);
}

#endif

/* EOF */
