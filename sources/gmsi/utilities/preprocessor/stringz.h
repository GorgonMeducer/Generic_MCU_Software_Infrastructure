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


#ifndef _STRINGZ_H_
#define _STRINGZ_H_


/*! \brief Stringize.
 *
 * Stringize a preprocessing token, this token being allowed to be \#defined.
 *
 * May be used only within macros with the token passed as an argument if the token is \#defined.
 *
 * For example, writing STRINGZ(PIN) within a macro \#defined by PIN_NAME(PIN)
 * and invoked as PIN_NAME(PIN0) with PIN0 \#defined as A0 is equivalent to
 * writing "A0".
 */
#define STRINGZ(x)                                #x

/*! \brief Absolute stringize.
 *
 * Stringize a preprocessing token, this token being allowed to be \#defined.
 *
 * No restriction of use if the token is \#defined.
 *
 * For example, writing ASTRINGZ(PIN0) anywhere with PIN0 \#defined as A0 is
 * equivalent to writing "A0".
 */
#define ASTRINGZ(x)                               STRINGZ(x)


#endif  // _STRINGZ_H_
