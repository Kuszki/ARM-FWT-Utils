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

#include "arm_wt_f32.h"
#include "arm_wt_q15.h"

#include "arm_math.h"

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

int main(int argc, char* argv[])
{
     const q15_t a[] = { 1, 2 }, b[] = { 3, 4 };
     const q15_t x0 = a[0] * b[0] + a[1] * b[1];

     q31_t x1 = 0,
           A = read_q15x2(a),
           B = read_q15x2(b);

     x1 = __SMLAD(A, B, x1);

     if (x1 != x0) return -1;

     const float32_t in_f32[] = { 0.00f, 0.00f, 0.00f, 0.15f, 0.20f, 0.25f, 0.20f, -0.05f, -0.10f, -0.15f, -0.20f, -0.25f, -0.05f, 0.0f, 0.20f, 0.25f, -0.55f, 0.55f };
     const float32_t cp_f32[] = { +0.068301, +0.118301, +0.031699, -0.018301 };

     const size_t NC = sizeof(cp_f32) / sizeof(float32_t);
     const size_t N = sizeof(in_f32) / sizeof(float32_t) - NC;

     float32_t bp_f32[NC];
     q15_t in_q15[N + NC], bp_q15[NC], cp_q15[NC];

     fwt_gen_b(cp_f32, bp_f32, NC);

     arm_float_to_q15(in_f32, in_q15, N);
     arm_float_to_q15(cp_f32, cp_q15, NC);
     arm_float_to_q15(bp_f32, bp_q15, NC);

     float32_t out_hp_f32[N / 2], out_lp_f32[N / 2], out_arm_f32[N / 2];
     q15_t out_hp_q15[N / 2], out_lp_q15[N / 2], out_arm_q15[N / 2];

     memset(out_hp_f32, 0, sizeof(out_hp_f32));
     memset(out_lp_f32, 0, sizeof(out_lp_f32));

     memset(out_hp_q15, 0, sizeof(out_hp_q15));
     memset(out_lp_q15, 0, sizeof(out_lp_q15));

     memset(out_arm_f32, 0, sizeof(out_arm_f32));
     memset(out_arm_q15, 0, sizeof(out_arm_q15));

     arm_wt_f32_fwt(in_f32, out_lp_f32, out_hp_f32, N, cp_f32, bp_f32, NC);
     arm_wt_q15_fwt(in_q15, out_lp_q15, out_hp_q15, N, cp_q15, bp_q15, NC);

     printf("\nf32 coefs. LP / HP\n");
     mat_print_matrix_f32(cp_f32, 1, NC);
     mat_print_matrix_f32(bp_f32, 1, NC);

     printf("\nf32 LP / HP\n");
     mat_print_matrix_f32(out_lp_f32, 1, N / 2);
     mat_print_matrix_f32(out_hp_f32, 1, N / 2);

     printf("\nq15 coefs. LP / HP\n");
     mat_print_matrix_q15(cp_q15, 1, NC);
     mat_print_matrix_q15(bp_q15, 1, NC);

     printf("\nq15 LP / HP\n");
     mat_print_matrix_q15(out_lp_q15, 1, N / 2);
     mat_print_matrix_q15(out_hp_q15, 1, N / 2);

     arm_q15_to_float(out_lp_q15, out_lp_f32, N / 2);
     arm_q15_to_float(out_lp_q15, out_lp_f32, N / 2);

     printf("\nq15 -> f32 LP / HP\n");
     mat_print_matrix_f32(out_lp_f32, 1, N / 2);
     mat_print_matrix_f32(out_hp_f32, 1, N / 2);

     arm_fir_decimate_instance_f32 S_f32;
     arm_fir_decimate_instance_q15 S_q15;

     float32_t state_f32[NC + N];
     q15_t state_q15[NC + N];

     memset(state_f32, 0, sizeof(state_f32));
     memset(state_q15, 0, sizeof(state_q15));

     arm_fir_decimate_init_f32(&S_f32, NC, 2, cp_f32, state_f32, N);
     arm_fir_decimate_init_q15(&S_q15, NC, 2, cp_q15, state_q15, N);

     arm_fir_decimate_f32(&S_f32, in_f32 + (NC - 1), out_arm_f32, N);
     arm_fir_decimate_q15(&S_q15, in_q15 + (NC - 1), out_arm_q15, N);

     printf("\nNo. | out LP | q15_t  | f -> q | float32_t | q -> f    | rel. err.\n");

     bool ok = true;

     for (int i = 0; i < N / 2; i++)
     {
          const q15_t out = out_lp_q15[i];

          const q15_t q = out_arm_q15[i];
          const float32_t f = out_arm_f32[i];

          const float q_f = (float)(q) / 32768.0f;
          const q31_t f_q = (q31_t)(f * 32768.0f);

          float err = 100.0f * (q_f - f) / f;

          const bool check = q == out;

          printf("%3d | %+6d | %+6d | %+6d | %+1.6f | %+1.6f | %+3.3f%% -> %s\n", i, out, q, f_q, f, q_f, err, check ? "OK" : "FAIL");

          ok = ok && check;
     }

     printf("\nq15 implementation status: %s\n\n", ok ? "OK" : "FAIL");

     return ok ? 0 : 1;
}
