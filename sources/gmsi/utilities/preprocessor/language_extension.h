/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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


#ifndef __LANGUAGE_EXTENSION_H__
#define __LANGUAGE_EXTENSION_H__

#include "../3rd-party/PLOOC/plooc.h"
#include "./connect.h"

#undef __using1
#undef __using2
#undef __using3
#undef __using4
#undef using

#define __using1(__declare)                                                     \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL;                   \
                )

#define __using2(__declare, __on_leave_expr)                                    \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL;                   \
                 (__on_leave_expr)                                              \
                )

#define __using3(__declare, __on_enter_expr, __on_leave_expr)                   \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )
                
#define __using4(__dcl1, __dcl2, __on_enter_expr, __on_leave_expr)              \
            for (__dcl1, __dcl2, *CONNECT3(__using_, __LINE__,_ptr) = NULL;     \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )
               
#define using(...)                                                              \
            CONNECT2(__using, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


#undef __with2
#undef __with3
#undef with

#define __with2(__type, __addr)                                                 \
            using(__type *_=(__addr))
#define __with3(__type, __addr, __item)                                         \
            using(__type *_=(__addr), *__item = _, _=_,_=_ )

#define with(...)                                                               \
            CONNECT2(__with, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#undef _

#undef foreach2
#undef foreach3
#undef foreach

#define foreach2(__type, __array)                                               \
            using(__type *_ = __array)                                         \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _++, CONNECT2(count,__LINE__)--                            \
                )

#define foreach3(__type, __array, __item)                                       \
            using(__type *_ = __array, *__item = _, _ = _, )                \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _++, __item = _, CONNECT2(count,__LINE__)--               \
                )

#define foreach(...)                                                            \
            CONNECT2(foreach, __PLOOC_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
            
#endif