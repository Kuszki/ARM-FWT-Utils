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

#include "arm_mal_f32.h"
#include "arm_wt_f32.h"

#include "mat_helper.h"
#include "hdf_helper.h"

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

int main(int argc, char* argv[])
{
     const char* path = argc > 1 ? argv[1] : "mallat_out.hdf";

     hdf_matrix mA, mC, mB, mX, mY;

     if (hdf_load_matrix(path, "/A/value", &mA)) return 1;
     if (hdf_load_matrix(path, "/C/value", &mC)) return 2;
     if (hdf_load_matrix(path, "/B/value", &mB)) return 3;
     if (hdf_load_matrix(path, "/X/value", &mX)) return 4;
     if (hdf_load_matrix(path, "/Y/value", &mY)) return 5;

     printf("\nmat. A (%zu x %zu):\n", mA.rows, mA.cols);
     hdf_print_matrix(&mA);
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

     float32_t out[N];

     memset(out, 0, N * sizeof(float32_t));
     fwt_malat(mX.data, out, N, 2, mC.data, mB.data, NC);

     printf("\nout fwt_malat:\n");
     mat_print_matrix_f32(out, mY.rows, mY.cols);

     bool ok1 = mat_compare_epsilon(mY.data, out, N, 1e-6);

     printf("fwt_malat: %s", ok1 ? "OK" : "FAIL");

     float32_t ext[N + NC];
     memcpy(ext, mX.data, N * sizeof(float32_t));
     memcpy(ext + N, ext, NC * sizeof(float32_t));

     memset(out, 0, N * sizeof(float32_t));
     fwt_malat_ext(ext, out, N, 2, mC.data, mB.data, NC);

     printf("\nout fwt_malat_ext:\n");
     mat_print_matrix_f32(out, mY.rows, mY.cols);

     bool ok2 = mat_compare_epsilon(mY.data, out, N, 1e-6);

     printf("fwt_malat_ext: %s", ok2 ? "OK" : "FAIL");

     memset(out, 0, N * sizeof(float32_t));
     fwt_malat_ext_par(ext, out, N, 2, mC.data, mB.data, NC);

     printf("\nout fwt_malat_ext_par:\n");
     mat_print_matrix_f32(out, mY.rows, mY.cols);

     bool ok3 = mat_compare_epsilon(mY.data, out, N, 1e-6);

     printf("fwt_malat_ext_par: %s", ok3 ? "OK" : "FAIL");

     memset(out, 0, N * sizeof(float32_t));

     const float32_t inv_sq2 = 1 / sqrt(2);
     float32_t sqC[NC], sqB[NC];

     for (size_t i = 0; i < NC; ++i)
     {
          sqC[i] = mC.data[i] * inv_sq2;
          sqB[i] = mB.data[i] * inv_sq2;
     }

     arm_wt_f32_mallat(ext, out, N, 2, sqC, sqB, NC);

     printf("\nout arm_wt_f32_mallat:\n");
     mat_print_matrix_f32(out, mY.rows, mY.cols);

     bool ok4 = mat_compare_epsilon(mY.data, out, N, 1e-6);

     printf("arm_wt_f32_mallat: %s", ok4 ? "OK" : "FAIL");

     uint16_t uint_in[N];
     const float32_t scale = 0.696969f;

     for (size_t i = 0; i < N; ++i) uint_in[i] = i;

     fwt_adc_scale_ext(uint_in, ext, N, NC, scale);

     for (size_t i = 0; i < N; ++i)
          if (scale * uint_in[i] != ext[i]) return 8;

     for (size_t i = 0; i < NC; ++i)
          if (scale * uint_in[i] != ext[N + i]) return 9;

     hdf_free_matrix(&mA);
     hdf_free_matrix(&mC);
     hdf_free_matrix(&mB);
     hdf_free_matrix(&mX);
     hdf_free_matrix(&mY);

     return ok1 && ok2 && ok3 && ok4 ? 0 : -1;
}
