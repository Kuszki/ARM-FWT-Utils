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
#include "hdf_helper.h"

static void clean(hdf_matrix* a, hdf_matrix* x, hdf_matrix* y)
{
	hdf_free_matrix(a);
	hdf_free_matrix(x);
	hdf_free_matrix(y);
}

int main(int argc, char* argv[])
{
	const char* path = argc > 1 ? argv[1] : "mul_out.hdf";

	hdf_matrix mA, mX, mY;

	if (hdf_load_matrix(path, "/A/value", &mA)) { clean(&mA, &mX, &mY); return 1; }
	if (hdf_load_matrix(path, "/X/value", &mX)) { clean(&mA, &mX, &mY); return 2; }
	if (hdf_load_matrix(path, "/Y/value", &mY)) { clean(&mA, &mX, &mY); return 3; }

	printf("A: %lu x %lu\n", mA.rows, mA.cols);
	printf("X: %lu x %lu\n", mX.rows, mX.cols);
	printf("Y: %lu x %lu\n", mY.rows, mY.cols);

	float32_t pY[mY.rows * mX.cols];

	arm_matrix_instance_f32 mat_A;
	arm_mat_init_f32(&mat_A, mA.rows, mA.cols, mA.data);

	arm_matrix_instance_f32 mat_X;
	arm_mat_init_f32(&mat_X, mX.rows, mX.cols, mX.data);

	arm_matrix_instance_f32 mat_Y;
	arm_mat_init_f32(&mat_Y, mY.rows, mY.cols, pY);

	const arm_status s = arm_mat_mult_f32(&mat_A, &mat_X, &mat_Y);
	const bool ok = mat_compare_relative(pY, mY.data, mY.rows * mX.cols, 1e-6);

	clean(&mA, &mX, &mY);

	return s == ARM_MATH_SUCCESS ? (ok ? 0 : -1) : -2;
}
