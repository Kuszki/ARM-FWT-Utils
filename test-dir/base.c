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

#include "arm_math.h"

const size_t N = 1024;

int main(int argc, char* argv[])
{
	float32_t A[N][N];
	float32_t X[N];
	float32_t Y[N];
	float32_t M[N / 2];

	arm_matrix_instance_f32 mat_A;
	arm_mat_init_f32(&mat_A, N, N, (float32_t*) A);

	arm_matrix_instance_f32 mat_X;
	arm_mat_init_f32(&mat_X, N, 1, X);

	arm_matrix_instance_f32 mat_Y;
	arm_mat_init_f32(&mat_Y, N, 1, Y);

	arm_rfft_fast_instance_f32 S;
	arm_rfft_fast_init_f32(&S, N);

	arm_mat_mult_f32(&mat_A, &mat_X, &mat_Y);
	arm_rfft_fast_f32(&S, X, Y, 0);
	arm_cmplx_mag_f32(Y, M, N / 2);

	return 0;
}
