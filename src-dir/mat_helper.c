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

#include "mat_helper.h"

bool mat_compare_epsilon(
  const float32_t* a,
  const float32_t* b,
  const size_t n,
  const float32_t e)
{
	for (size_t i = 0; i < n; ++i)
	{
		if (fabsf(a[i] - b[i]) > e) return false;
	}

	return true;
}

bool mat_compare_relative(
  const float32_t* a,
  const float32_t* b,
  const size_t n,
  const float32_t e)
{
	for (size_t i = 0; i < n; ++i)
	{
		if (fabsf((a[i] - b[i]) / b[i]) > e) return false;
	}

	return true;
}

void mat_print_matrix_f32(
  const float32_t* data,
  size_t rows,
  size_t cols)
{
	for (size_t i = 0; i < rows; ++i)
	{
		for (size_t j = 0; j < cols; ++j)
		{
			printf("%+5.5f\t", data[j*rows + i]);
		}
		printf("\n");
	}
}

void mat_print_matrix_q15(
  const q15_t* data,
  size_t rows,
  size_t cols)
{
     for (size_t i = 0; i < rows; ++i)
     {
          for (size_t j = 0; j < cols; ++j)
          {
               printf("%+d\t", data[j * rows + i]);
          }
          printf("\n");
     }
}
