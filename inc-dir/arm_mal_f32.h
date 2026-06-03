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

int fwt_gen_b(
  const float32_t* restrict c,
  float32_t* restrict b,
  const size_t NC);

int fwt_malat(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t N,
  size_t dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t NC);

int fwt_fir_dec(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t N,
  const float32_t* restrict c,
  const size_t NC);

int fwt_malat_ext(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t N,
  size_t dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t NC);

int fwt_malat_ext_par(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t N,
  size_t dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t NC);

int fwt_fir_dec_ext(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t N,
  const float32_t* restrict c,
  const size_t NC);

int fwt_fir_dec_ext_par(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t N,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t NC);

int fwt_adc_scale(
  const uint16_t* restrict in,
  float32_t* restrict out,
  const size_t N,
  const float32_t scale);

int fwt_adc_scale_ext(
  const uint16_t* restrict in,
  float32_t* restrict out,
  const size_t N,
  const size_t NC,
  const float32_t scale);
