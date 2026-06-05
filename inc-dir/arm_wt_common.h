/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  {description}                                                          *
 *  Copyright (C) 2022  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the  Free Software Foundation, either  version 3 of the  License, or   *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This  program  is  distributed  in the hope  that it will be useful,   *
 *  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
 *  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have  received a copy  of the  GNU General Public License   *
 *  along with this program. If not, see http://www.gnu.org/licenses/.     *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ARM_WT_COMMON_H
#define ARM_WT_COMMON_H

#include "cmsis_compiler.h"

#include "arm_math_memory.h"
#include "arm_math_types.h"

#ifndef ARM_MATH_DSP
#include "dsp/none.h"
#endif

/**
 *
 * @brief WT status code.
 *
 * Specify current WT function success or fail reason.
 *
 */
typedef enum
{

     WT_STATUS_SUCCESS, //!< Everything is fine
     WT_STATUS_ERROR, //!< Other error occurred

     WT_WRONG_NLEN, //!< Data length not set - check arm_wt_q15_instance::n_len
     WT_WRONG_CLEN, //!< Coefs length not set - check arm_wt_q15_instance::c_len
     WT_WRONG_NDEC, //!< Decimation level is wrong - check check arm_wt_q15_instance::n_dec
     WT_WRONG_CPTR, //!< Coefs pointer not set - check arm_wt_q15_instance::c and arm_wt_q15_instance::b

} arm_wt_status;

#endif // ARM_WT_COMMON_H
