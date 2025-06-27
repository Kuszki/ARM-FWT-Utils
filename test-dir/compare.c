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

int main(int argc, char* argv[])
{
	const float32_t a[] = { 1.100, 2.200, 3.300 };
	const float32_t b[] = { 1.101, 2.201, 3.301 };

	const size_t n1 = sizeof(a) / sizeof(float32_t);
	const size_t n2 = sizeof(b) / sizeof(float32_t);
	const size_t n = n1 > n2 ? n2 : n1;

	if (!mat_compare_epsilon(a, b, n, 0.00101)) return 1;
	if (mat_compare_epsilon(a, b, n,  0.00099)) return 2;

	if (!mat_compare_relative(a, b, n, 0.00091)) return 3;
	if (mat_compare_relative(a, b, n,  0.00089)) return 4;

	return 0;
}
