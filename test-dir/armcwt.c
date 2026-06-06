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
     const char* path = argc > 1 ? argv[1] : "filter_out.hdf";

     hdf_matrix mA, mC, mB, mX, mL, mH;

     if (hdf_load_matrix(path, "/C/value", &mC)) return 2;
     if (hdf_load_matrix(path, "/B/value", &mB)) return 3;
     if (hdf_load_matrix(path, "/X/value", &mX)) return 4;
     if (hdf_load_matrix(path, "/L/value", &mL)) return 5;
     if (hdf_load_matrix(path, "/H/value", &mH)) return 5;

     printf("\nvec. C (%zu x %zu):\n", mC.rows, mC.cols);
     hdf_print_matrix(&mC);
     printf("\nvec. B (%zu x %zu):\n", mB.rows, mB.cols);
     hdf_print_matrix(&mB);
     printf("\nvec. X (%zu x %zu):\n", mX.rows, mX.cols);
     hdf_print_matrix(&mX);
     printf("\nvec. L (%zu x %zu):\n", mL.rows, mL.cols);
     hdf_print_matrix(&mL);
     printf("\nvec. H (%zu x %zu):\n", mH.rows, mH.cols);
     hdf_print_matrix(&mH);

     if (MAX(mC.rows, mC.cols) != MAX(mB.rows, mB.cols)) return 6;
     if (MAX(mX.rows, mX.cols) != MAX(mL.rows, mH.cols) || MAX(mX.rows, mX.cols) != MAX(mL.rows, mH.cols)) return 7;

     const size_t N = MAX(mX.rows, mX.cols);
     const size_t NC = MAX(mC.rows, mC.cols);

     uint16_t in_u[N];

     for (size_t i = 0; i < N; ++i) in_u[i] = mX.data[i];

     arm_wt_f32_instance ufwt = {
          .c = mC.data,
          .b = mB.data,
          .c_len = NC,
          .n_len = N,
          .n_dec = 1,
          .scale = 1.0f,
          .bias = 0.0f,
     };

     arm_wt_status s = arm_cwt_f32_init(&ufwt);

     printf("\nlens:\n");
     for (size_t i = 0; i <= ufwt.n_dec; ++i)
     {
          printf("\t%zu", ufwt.lens[i]);
     }

     if (s != WT_STATUS_SUCCESS) return 8;

     arm_wt_f32_run(&ufwt, in_u);
     arm_wt_f32_run(&ufwt, in_u);

     printf("\nHP:\n");
     mat_print_matrix_f32(ufwt.out[0], 1, ufwt.n_len);
     printf("\nLP:\n");
     mat_print_matrix_f32(ufwt.out[1], 1, ufwt.n_len);

     const bool ok1 = mat_compare_epsilon(ufwt.out[0], mH.data, N, 1e-5);
     const bool ok2 = mat_compare_epsilon(ufwt.out[1], mL.data, N, 1e-5);

     printf("\narm_wt_f32_run HP: %s\n", ok1 ? "PASS" : "FAIL");
     printf("\narm_wt_f32_run LP: %s\n", ok2 ? "PASS" : "FAIL");

     arm_wt_f32_free(&ufwt);

     float32_t in_f[N + NC];
     float32_t hp_A[N], hp_B[N / 2];
     float32_t lp_A[N], lp_B[N / 2];

     for (size_t i = 0; i < NC; ++i) in_f[i] = mX.data[i];
     for (size_t i = 0; i < N; ++i) in_f[i + NC] = mX.data[i];

     arm_wt_f32_cwt(in_f, lp_A, hp_A, N, mC.data, mB.data, NC);
     arm_wt_f32_fwt(in_f, lp_B, hp_B, N, mC.data, mB.data, NC);

     printf("\nHP A/B:\n");
     mat_print_matrix_f32(hp_A, 1, N);
     mat_print_matrix_f32(hp_B, 1, N / 2);
     printf("\nLP A/B:\n");
     mat_print_matrix_f32(lp_A, 1, N);
     mat_print_matrix_f32(lp_B, 1, N / 2);

     bool ok3 = true;
     bool ok4 = true;

     for (size_t i = 0; i < N / 2; ++i) ok3 = ok3 && (lp_A[2 * i] == lp_B[i]);
     for (size_t i = 0; i < N / 2; ++i) ok4 = ok4 && (hp_A[2 * i] == hp_B[i]);

     printf("\arm_wt_f32_fir vs arm_wt_f32_firdec LP: %s\n", ok3 ? "PASS" : "FAIL");
     printf("\arm_wt_f32_fir vs arm_wt_f32_firdec HP: %s\n", ok4 ? "PASS" : "FAIL");

     hdf_free_matrix(&mC);
     hdf_free_matrix(&mB);
     hdf_free_matrix(&mX);
     hdf_free_matrix(&mL);
     hdf_free_matrix(&mH);

     return ok1 && ok2 && ok3 && ok4 ? 0 : 1;
}
