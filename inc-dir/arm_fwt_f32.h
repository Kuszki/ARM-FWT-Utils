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

#include "arm_math_types.h"

typedef enum
{

     FWT_STATUS_SUCCESS,
     FWT_STATUS_ERROR,

     FWT_WRONG_NLEN,
     FWT_WRONG_CLEN,
     FWT_WRONG_CPTR,
     FWT_WRONG_NDEC,

} arm_fwt_status;

typedef struct
{

     const float32_t* c;
     const float32_t* b;

     float32_t scale;
     float32_t shift;

     size_t n_dec;
     size_t n_len;
     size_t c_len;

     float32_t** out;
     float32_t** buf;

     size_t* lens;
     float32_t* in;

     size_t offset;

} arm_fwt_f32_instance;

arm_fwt_status arm_fwt_f32_init(arm_fwt_f32_instance* instance);
void arm_fwt_f32_free(arm_fwt_f32_instance* instance);

void arm_fwt_f32_run(
  arm_fwt_f32_instance* restrict instance,
  const uint32_t* restrict in);

void arm_fwt_f32_scale(
  const uint32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t scale,
  const float32_t shift);

void arm_fwt_f32_copy(
  const float32_t* restrict scr,
  float32_t* restrict dst,
  const size_t len);

void arm_fwt_f32_zero(
  float32_t* restrict dst,
  const size_t len);

void arm_fwt_f32_firdec(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t n_len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len);
