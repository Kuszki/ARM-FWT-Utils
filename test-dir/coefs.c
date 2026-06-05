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

#include "hdf_helper.h"
#include "mat_helper.h"

#include "arm_mal_f32.h"

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

int main(int argc, char* argv[])
{
	const char* path = argc > 1 ? argv[1] : "mallat_out.hdf";

	hdf_matrix mA, mC, mB, mX, mY;

	if (hdf_load_matrix(path, "/C/value", &mC)) return 1;
	if (hdf_load_matrix(path, "/B/value", &mB)) return 2;

	printf("\nvec. C org:\n");
	hdf_print_matrix(&mC);
	printf("\nvec. B org:\n");
	hdf_print_matrix(&mB);

	if (mC.rows != mB.rows || mC.cols != mB.cols) return 3;

	const size_t NC = MAX(mC.rows, mC.cols);
	float32_t b[NC];

	printf("\nvec. B out:\n");
	fwt_gen_b(mC.data, b, NC);
        mat_print_matrix_f32(b, mC.rows, mC.cols);

        bool ok = mat_compare_relative(mB.data, b, NC, 1e-6);

	hdf_free_matrix(&mC);
	hdf_free_matrix(&mB);

	return ok ? 0 : -1;
}
