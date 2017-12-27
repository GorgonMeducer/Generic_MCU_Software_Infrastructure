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
#include ".\app_cfg.h"
#include ".\interface.h"

/*============================ MACROS ========================================*/
#define this            (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name memory page size
//! @{
typedef enum {
    PAGE_SIZE_4,                //!< 4      units
    PAGE_SIZE_8,                //!< 8      units
    PAGE_SIZE_16,               //!< 16     units
    PAGE_SIZE_32,               //!< 32     units
    PAGE_SIZE_64,               //!< 64     units
    PAGE_SIZE_128,              //!< 128    units
    PAGE_SIZE_256,              //!< 256    units
    PAGE_SIZE_512,              //!< 512    units
    PAGE_SIZE_1024,             //!< 1024   units
    PAGE_SIZE_2048,             //!< 2048   units
    PAGE_SIZE_4096              //!< 4096   units
}page_size_t;
//! @}

//! \name page configuration
//! @{
typedef struct {
    i_page_t    tIO;                    //!< IO
    uint8_t     *pchBuffer;             //!< page buffer
    page_size_t tPageSize;              //!< page size(buffer size)
    void        *pObj;
}page_cfg_t;
//! @}

DECLARE_CLASS(page_t)
//! \name class page_t
//! @{
DEF_CLASS(page_t)
    const i_page_t  tIO;
    void            *pObj;
    uint8_t         *pchBuffer;
    const uint16_t  hwPageSize;
    locker_t        bCritical;
    uint8_t         bModified;
    uint8_t         chFSMWrite;
    uint8_t         chFSMRead;
    uint8_t         *pchStream;
    uint16_t        hwCount;
    uint32_t        wAddress;
    uint32_t        wPageAddress;
END_DEF_CLASS(page_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief validate specified page_t object
 *! \param ptPage page_t object
 *! \retval true target object is valid
 *! \retval false target object is invalid
 */
static bool is_page_interface_valid(page_t *ptPage)
{
    CLASS(page_t) *ptThis = (CLASS(page_t) *)ptPage;
    if ( (0 == this.hwPageSize) 
            ||  (NULL == this.pchBuffer)
            ||  (NULL == this.tIO.fnPageErase)
            ||  (NULL == this.tIO.fnPageRead)
            ||  (NULL == this.tIO.fnPageWrite)) {
        return false;
    }

    return true;
}


/*! \brief initialize page service
 *! \param ptPage page_t object
 *! \param ptCFG configuration structure
 *! \retval true initialize succeed
 *! \retval false intizlization fail.
 */
bool page_init(page_t *ptPage, page_cfg_t *ptCFG)
{
    CLASS(page_t) *ptThis = (CLASS(page_t) *)ptPage;

    if (NULL == ptPage || NULL == ptCFG) {
        return false;
    } 

    //! initialize page_t object
    TYPE_CONVERT(&(this.hwPageSize), uint16_t) = 
                                        ((uint16_t)1 << (ptCFG->tPageSize + 2)) - 1;
    this.pchBuffer = ptCFG->pchBuffer;
    TYPE_CONVERT(&(this.tIO), i_page_t) = ptCFG->tIO;

    if (!is_page_interface_valid(ptPage)) {
        //! invalid interface
        return false;
    }
    this.pObj = ptCFG->pObj;
    this.chFSMRead = 0;
    this.chFSMWrite = 0;
    LOCK_INIT(this.bCritical);

    return true;
}

static void modify_leading_page(CLASS(page_t) *ptThis)
{
    if (this.bModified) {
        return ;
    }
    this.bModified = true;

    do {
        uint8_t *pchDes = this.pchBuffer;
        uint8_t *pchSrc = this.pchStream;
        uint32_t wAddress = this.wAddress;
        uint_fast16_t hwTemp = wAddress & this.hwPageSize;
        //! get data count
        uint_fast16_t hwCount = this.hwPageSize + 1 - hwTemp;
        hwCount = MIN(hwCount, this.hwCount);
        
        //! update data count
        this.hwCount -= hwCount;
        //! update address
        this.wAddress += hwCount;
        
        //! get destination address
        pchDes += hwTemp;
        if (NULL != this.pchStream) {
            //! copy content
            do {
                *pchDes++ = *pchSrc++;
            } while(--hwCount);
            
            //! update pointer
            this.pchStream = pchSrc;
        } else {
            //! fill 0xFF
            do {
                *pchDes++ = 0xFF;
            } while(--hwCount);
        }
    } while(false);
}

static void modify_final_page(CLASS(page_t) *ptThis)
{
    if (this.bModified) {
        return ;
    }
    this.bModified = true;
    do {
        uint8_t *pchDes = this.pchBuffer;
        uint8_t *pchSrc = this.pchStream;
        uint_fast16_t hwCount = this.hwCount;
        if (NULL != this.pchStream) {
            //! copy content
            do {
                *pchDes++ = *pchSrc++;
            } while (--hwCount);
        } else {
            //! fill 0xFF
            do {
                *pchDes++ = 0xFF;
            } while (--hwCount);
        }
    } while (false);
}

#define WRITE_START                     0
#define WRITE_READ_FRONT_PAGE           1
#define WRITE_ERASE_LEADING_PAGE        2
#define WRITE_MODIFY_LEADING_PAGE       3
#define WRITE_PROG_LEADING_PAGE         4
#define WRITE_PAGES                     5
#define WRITE_ERASE_PAGE                6
#define WRITE_PROG_PAGE                 7
#define WRITE_READ_FINAL_PAGE           8
#define WRITE_ERASE_FINAL_PAGE          9
#define WRITE_MODIFY_FINAL_PAGE         10
#define WRITE_PROG_FINAL_PAGE           11

#define WRITE_RESET_FSM()               do { chState = 0;} while(0)


/*! \brief write memory disregard page
 *! \param ptPage page_t object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
fsm_rt_t write_memory_disregard_page( page_t *ptPage, uint32_t wAddress,
     uint8_t *pchStream, uint_fast16_t hwSize )
{
    CLASS(page_t) *ptThis = (CLASS(page_t) *)ptPage;
    fsm_rt_t tfsm = fsm_rt_on_going;
    fsm_rt_t tSubFSM;
    
    if (NULL == ptPage) {
        return (fsm_rt_t)GSF_ERR_FAIL;
    }

    uint_fast8_t chState = this.chFSMWrite; //!< get state
    do {
        switch (chState) {
            case WRITE_START:
                //! validate page_t object
                if (/*NULL == pchStream ||*/ 0 == hwSize) {
                    tfsm = fsm_rt_cpl;
                    break;
                } else if (!is_page_interface_valid(ptPage)) {
                    tfsm = (fsm_rt_t)GSF_ERR_NOT_AVAILABLE;
                    break;
                } 
                if (!ENTER_LOCK(&this.bCritical)) {
                    break;
                }

                this.hwCount = hwSize;
                this.pchStream = pchStream;
                //this.hwSize = hwSize;
                this.wAddress = wAddress;

                //! write front unaligned
                if (wAddress & this.hwPageSize) {
                    //! the address isn't aligned to page address
                    this.wPageAddress = this.wAddress & (~this.hwPageSize);
                    chState = WRITE_READ_FRONT_PAGE;
                } else {
                    //! the address is aligned to page address
                    chState = WRITE_PAGES;
                    break;
                }
                
            case WRITE_READ_FRONT_PAGE:
                //! read target page
                tSubFSM = this.tIO.fnPageRead( this.pObj, this.wPageAddress, 
                                            this.pchBuffer/*, this.hwPageSize + 1*/);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    break;
                }
                this.bModified = false;
                chState = WRITE_ERASE_LEADING_PAGE;
                break;

            case WRITE_ERASE_LEADING_PAGE:
                //! erase target page
                tSubFSM = this.tIO.fnPageErase(this.pObj, this.wPageAddress);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    modify_leading_page(ptThis);
                    break;
                }
                chState = WRITE_MODIFY_LEADING_PAGE;
                break;

            case WRITE_MODIFY_LEADING_PAGE:
                modify_leading_page(ptThis);
                chState = WRITE_PROG_LEADING_PAGE;
                break;

            case WRITE_PROG_LEADING_PAGE:
                //! program target page
                tSubFSM = this.tIO.fnPageWrite(this.pObj, this.wPageAddress, 
                                            this.pchBuffer/*, this.hwPageSize + 1*/);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    break;
                }
                
                chState = WRITE_PAGES;

            case WRITE_PAGES:
                this.wPageAddress = this.wAddress & (~this.hwPageSize);
                if (this.hwCount > this.hwPageSize) {
                    chState = WRITE_ERASE_PAGE;
                } else if (this.hwCount) {
                    //! final page
                    chState = WRITE_READ_FINAL_PAGE;
                } else {
                    //! complete
                    tfsm = fsm_rt_cpl;
                    LEAVE_LOCK(&this.bCritical);
                    WRITE_RESET_FSM(); 
                }
                break;

            case WRITE_ERASE_PAGE:
                //! erase target page
                tSubFSM = this.tIO.fnPageErase(this.pObj, this.wPageAddress);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    break;
                }
                
                chState = WRITE_PROG_PAGE;
                break;

            case WRITE_PROG_PAGE:
                if (NULL != this.pchStream) {
                    tSubFSM = this.tIO.fnPageWrite(this.pObj, this.wPageAddress, 
                                                this.pchStream/*, this.hwPageSize + 1*/);
                    if (IS_FSM_ERR(tSubFSM)) {
                        tfsm = tSubFSM;
                        break;
                    } else if (fsm_rt_cpl != tSubFSM) {
                        break;
                    }
                }
                //! update 
                do {
                    uint_fast16_t hwTemp = this.hwPageSize + 1;
                    if (NULL != this.pchStream) {
                        this.pchStream += hwTemp;
                    }
                    this.wAddress += hwTemp;
                    this.hwCount -= hwTemp;
                } while (0);
                chState = WRITE_PAGES;
                break;
            
            case WRITE_READ_FINAL_PAGE:
                
                //! read target page
                tSubFSM = this.tIO.fnPageRead( this.pObj, this.wPageAddress, 
                                            this.pchBuffer/*, this.hwPageSize + 1*/);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    break;
                }
                this.bModified = false;
                chState = WRITE_ERASE_FINAL_PAGE;
                break;

            case WRITE_ERASE_FINAL_PAGE:
                //! erase target page
                tSubFSM = this.tIO.fnPageErase(this.pObj, this.wPageAddress);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    modify_final_page(ptThis);
                    break;
                }
                chState = WRITE_MODIFY_FINAL_PAGE;
                break;

            case WRITE_MODIFY_FINAL_PAGE:
                modify_final_page(ptThis);
//                do {
//                    uint8_t *pchDes = this.pchBuffer;
//                    uint8_t *pchSrc = this.pchStream;
//                    uint_fast16_t hwCount = this.hwCount;
//                    if (NULL != this.pchStream) {
//                        //! copy content
//                        do {
//                            *pchDes++ = *pchSrc++;
//                        } while (--hwCount);
//                    } else {
//                        //! fill 0xFF
//                        do {
//                            *pchDes++ = 0xFF;
//                        } while (--hwCount);
//                    }
//                } while (0);
                chState = WRITE_PROG_FINAL_PAGE;
                break;

            case WRITE_PROG_FINAL_PAGE:
                tSubFSM = this.tIO.fnPageWrite(this.pObj, this.wPageAddress, 
                                            this.pchBuffer/*, this.hwPageSize + 1*/);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    break;
                }
                //! complete
                tfsm = fsm_rt_cpl;
                LEAVE_LOCK(&this.bCritical);
                WRITE_RESET_FSM(); 
                break;

        }
    } while(false);
    this.chFSMWrite = chState;              //!< update state;

    return tfsm;
}

#define READ_START              0
#define READ_PAGE               1
#define READ_SET_BUFFER         2
#define READ_RESET_FSM()        do { chState = 0; } while(0)

/*! \brief read memory disregard page
 *! \param ptPage page_t object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
fsm_rt_t read_memory_disregard_page( page_t *ptPage, uint32_t wAddress, 
    uint8_t *pchStream, uint_fast16_t hwSize )
{
    CLASS(page_t) *ptThis = (CLASS(page_t) *)ptPage;
    fsm_rt_t tfsm = fsm_rt_on_going;
    fsm_rt_t tSubFSM;

    if (NULL == ptPage) {
        return (fsm_rt_t)GSF_ERR_FAIL;
    }

    uint_fast8_t chState = this.chFSMRead;  //!< get state
    do {
        switch (chState) {
            case READ_START:
                //! validate page_t object
                if (NULL == pchStream || 0 == hwSize) {
                    tfsm = fsm_rt_cpl;
                    break;
                } else if (!is_page_interface_valid(ptPage)) {
                    tfsm = (fsm_rt_t)GSF_ERR_NOT_AVAILABLE;
                    break;
                }  
                if (!ENTER_LOCK(&this.bCritical)) {
                    break;
                }

                this.hwCount = hwSize;
                this.pchStream = pchStream;
                this.wAddress = wAddress;
                this.wPageAddress = this.wAddress & (~this.hwPageSize);
                //! read page
                chState = READ_PAGE;

            case READ_PAGE:
                //! read target page
                tSubFSM = this.tIO.fnPageRead( this.pObj, this.wPageAddress, 
                                            this.pchBuffer/*, this.hwPageSize + 1*/);
                if (IS_FSM_ERR(tSubFSM)) {
                    tfsm = tSubFSM;
                    break;
                } else if (fsm_rt_cpl != tSubFSM) {
                    break;
                }
                chState = READ_SET_BUFFER;
                break;

            case READ_SET_BUFFER:
                do {
                    uint8_t *pchSrc = this.pchBuffer;
                    uint8_t *pchDes = this.pchStream;
                    uint32_t wAddress = this.wAddress;
                    //! address mod pagesize
                    uint_fast16_t hwTemp = wAddress & this.hwPageSize;
                    //! get source address
                    pchSrc += hwTemp; 
                    //! calculate count
                    uint_fast16_t hwCount = this.hwPageSize + 1 - hwTemp;
                    hwCount = MIN(this.hwCount, hwCount);

                    //! update count
                    this.hwCount -= hwCount;
                    //! update address
                    this.wAddress += hwCount;

                    //! copy content
                    do {
                        *pchDes++ = *pchSrc++;
                    }while(--hwCount);

                    //! update pointer
                    this.pchStream = pchDes;

                    if (this.hwCount) {
                        this.wPageAddress = this.wAddress & (~this.hwPageSize);
                        chState = READ_PAGE; 
                    } else {
                        READ_RESET_FSM();
                        LEAVE_LOCK(&this.bCritical);
                        tfsm = fsm_rt_cpl;
                    }
                } while (0);
                break;

        }
    } while(false);
    this.chFSMRead = chState;              //!< update state;

    return tfsm;
}
  
/* EOF */
