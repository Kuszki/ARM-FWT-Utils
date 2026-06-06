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

#include "arm_wt_f32.h"

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

int main(int argc, char* argv[])
{
     const char* path = argc > 1 ? argv[1] : "mallat_out.hdf";

     hdf_matrix mA, mC, mB, mX, mY;

     if (hdf_load_matrix(path, "/C/value", &mC)) return 2;
     if (hdf_load_matrix(path, "/B/value", &mB)) return 3;
     if (hdf_load_matrix(path, "/X/value", &mX)) return 4;
     if (hdf_load_matrix(path, "/Y/value", &mY)) return 5;

     printf("\nvec. C (%zu x %zu):\n", mC.rows, mC.cols);
     hdf_print_matrix(&mC);
     printf("\nvec. B (%zu x %zu):\n", mB.rows, mB.cols);
     hdf_print_matrix(&mB);
     printf("\nvec. X (%zu x %zu):\n", mX.rows, mX.cols);
     hdf_print_matrix(&mX);
     printf("\nvec. Y (%zu x %zu):\n", mY.rows, mY.cols);
     hdf_print_matrix(&mY);

     if (MAX(mC.rows, mC.cols) != MAX(mB.rows, mB.cols)) return 6;
     if (MAX(mX.rows, mX.cols) != MAX(mY.rows, mY.cols)) return 7;

     const size_t N = MAX(mX.rows, mX.cols);
     const size_t NC = MAX(mC.rows, mC.cols);

     uint16_t in_u[N];

     for (size_t i = 0; i < N; ++i) in_u[i] = mX.data[i];

     arm_wt_f32_instance fwt = {
          .c = mC.data,
          .b = mB.data,
          .c_len = NC,
          .n_len = N,
          .n_dec = 2,
          .scale = 1.0f,
          .bias = 0.0f,
     };

     arm_wt_status s = arm_fwt_f32_init(&fwt);

     printf("\nlens:\n");
     for (size_t i = 0; i <= fwt.n_dec; ++i)
     {
          printf("\t%zu", fwt.lens[i]);
     }

     if (s != WT_STATUS_SUCCESS) return 8;

     arm_wt_f32_run(&fwt, in_u);

     printf("\ninput:\n");
     mat_print_matrix_f32(fwt.in, 1, fwt.n_len + fwt.c_len - 1);
     printf("\nHP:\n");
     mat_print_matrix_f32(fwt.buf[0], 1, fwt.n_len / 2 + fwt.c_len - 1);
     printf("\nLP:\n");
     mat_print_matrix_f32(fwt.buf[1], 1, fwt.n_len / 2 + fwt.c_len - 1);

     for (size_t i = 0; i < fwt.n_dec; ++i)
     {
          printf("\nlvl %zu (%zu):\n", i, fwt.lens[i]);
          mat_print_matrix_f32(fwt.buf[2 * i], 1, fwt.lens[i] + fwt.c_len - 1);
          mat_print_matrix_f32(fwt.buf[2 * i + 1], 1, fwt.lens[i] + fwt.c_len - 1);
     }

     for (size_t i = 0; i <= fwt.n_dec; ++i)
     {
          printf("\nlvl %zu (%zu):\n", i, fwt.lens[i]);
          mat_print_matrix_f32(fwt.out[i], 1, fwt.lens[i]);
     }

     arm_wt_f32_run(&fwt, in_u);

     for (size_t i = 0; i < fwt.n_dec; ++i)
     {
          printf("\nlvl %zu (%zu):\n", i, fwt.lens[i]);
          mat_print_matrix_f32(fwt.buf[2 * i], 1, fwt.lens[i] + fwt.c_len - 1);
          mat_print_matrix_f32(fwt.buf[2 * i + 1], 1, fwt.lens[i] + fwt.c_len - 1);
     }

     for (size_t i = 0; i <= fwt.n_dec; ++i)
     {
          printf("\nlvl %zu (%zu):\n", i, fwt.lens[i]);
          mat_print_matrix_f32(fwt.out[i], 1, fwt.lens[i]);
     }

     arm_wt_f32_free(&fwt);

     hdf_free_matrix(&mC);
     hdf_free_matrix(&mB);
     hdf_free_matrix(&mX);
     hdf_free_matrix(&mY);

     return 0;
}
