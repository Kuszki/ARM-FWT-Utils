/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  ARM-FWT-Test -- FWT implementation for ARM using CMSIS-DSP library     *
 *  Copyright (C) 2025  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
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

#ifndef MAT_HELPER_H
#define MAT_HELPER_H

#include "arm_math_types.h"

#include <stdbool.h>
#include <stdio.h>

bool mat_compare_epsilon(
  const float32_t* a,
  const float32_t* b,
  const size_t n,
  const float32_t e);

bool mat_compare_relative(
  const float32_t* a,
  const float32_t* b,
  const size_t n,
  const float32_t e);

void mat_print_matrix_f32(
  const float32_t* data,
  size_t rows,
  size_t cols);

void mat_print_matrix_q15(
  const q15_t* data,
  size_t rows,
  size_t cols);

#endif // HDF_HELPER_H
