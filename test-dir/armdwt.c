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

#include "arm_dwt_f32.h"
#include "fwt_mallat.h"
#include "hdf_helper.h"
#include "mat_helper.h"

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
     mat_print_matrix(out, mY.rows, mY.cols);

     arm_dwt_instance_f32 pDWTInstance;
     arm_dwt_out_f32 dwtOut;
     arm_dwt_status s;

     float32_t buf[N * 2];

     s = arm_dwt_init_f32(4, mC.data, mB.data, ARM_DWT_EM_ZERO_PADDING, 2, buf, 2 * N, &pDWTInstance);

     if (s != DWT_STATUS_SUCCESS) return 8;

     s = arm_dwt_f32(&pDWTInstance, mX.data, N, &dwtOut);

     if (s != DWT_STATUS_SUCCESS) return 9;

     printf("\nout fwt_premade:\n");

     for (int i = 0; i < dwtOut.decLevel + 1; ++i)
          mat_print_matrix(dwtOut.coeffs[i].pCoeffs, 1, dwtOut.coeffs[i].size);

     hdf_free_matrix(&mA);
     hdf_free_matrix(&mC);
     hdf_free_matrix(&mB);
     hdf_free_matrix(&mX);
     hdf_free_matrix(&mY);

     return 0;
}
