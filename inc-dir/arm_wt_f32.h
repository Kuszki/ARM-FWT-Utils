/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  {description}                                                          *
 *  Copyright (C) 2022  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
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

#ifndef ARM_WT_F32_H
#define ARM_WT_F32_H

#include "arm_math_types.h"

/**
 *
 * @brief WT status code.
 *
 * Specify current WT function success or fail reason.
 *
 */
typedef enum
{

     WT_STATUS_SUCCESS, //!< Everything is fine
     WT_STATUS_ERROR, //!< Other error occurred

     WT_WRONG_NLEN, //!< Data length not set - check arm_wt_f32_instance::n_len
     WT_WRONG_CLEN, //!< Coefs length not set - check arm_wt_f32_instance::c_len
     WT_WRONG_NDEC, //!< Decimation level is wrong - check check arm_wt_f32_instance::n_dec
     WT_WRONG_CPTR, //!< Coefs pointer not set - check arm_wt_f32_instance::c and arm_wt_f32_instance::b

} arm_wt_status;

/**
 * @brief Wavelet transform object.
 * @see arm_fwt_f32_init, arm_cwt_f32_init, arm_wt_f32_free, arm_wt_f32_run
 *
 * This structure holds the WT algorithm parameters and internal buffers used in data processing.
 *
 * Wavelet Transform instance is prepared to use with raw uint16_t ADC outputs. Data in question
 * is scaled and shifted calling arm_wt_f32_run. This algorithm is designed to run in cycle,
 * between subsequent ADC interrupts signed after conversion of N samples is done.
 *
 * Before using arm_wt_f32_run required fields must be filled and constructor arm_fwt_f32_init or
 * arm_cwt_f32_init must be executed. Fields to fill before running constructor are:
 * - arm_wt_f32_instance::c - low-pass FIR coefficients (stored in reverse order)
 * - arm_wt_f32_instance::b - high-pass FIR coefficients (stored in reverse order)
 * - arm_wt_f32_instance::scale - scale factor for ADC results (X = scale * ADC + shift)
 * - arm_wt_f32_instance::shift - shift value for ADC results (X = scale * ADC + shift)
 * - arm_wt_f32_instance::n_dec - decomposition levels (at least 1)
 * - arm_wt_f32_instance::n_len - input data length (number of ADC output values)
 * - arm_wt_f32_instance::c_len - number of FIR taps (length of c must be equal length of b)
 *
 * To init decimated (FWT) version you need to run arm_fwt_f32_init, where arm_cwt_f32_init
 * setup the undecimated (UFWT or CWT) version of algorithm.
 *
 * Fields set up after calling constructor are:
 * - arm_wt_f32_instance::out - pointer to n_dec+1 vectors containing usable output values
 * - arm_wt_f32_instance::buf - pointer to internal buffer containing all FIR outputs
 * - arm_wt_f32_instance::lens - vector of n_dec+1 elements with subsequent out item lengths
 * - arm_wt_f32_instance::in - internal input vector with extra items to store current FIR state
 * - arm_wt_f32_instance::offset - extra item count of buffers to store subsequent FIR states
 * - arm_wt_f32_instance::dec - decimation factor (1 in case of CWT and 2 in case of FWT)
 *
 * After using this object a destructor arm_wt_f32_free must be called to free all used resources.
 *
 */
typedef struct
{

     const float32_t* c; //!< Pointer to low-pass FIR coefficients (stored in reverse order)
     const float32_t* b; //!< Pointer to high-pass FIR coefficients (stored in reverse order)

     float32_t scale; //!< Scale factor for ADC results (X = scale * ADC + shift)
     float32_t shift; //!< Shift value for ADC results (X = scale * ADC + shift)

     size_t n_dec; //!< Number of decomposition levels (where n_dec > 1)
     size_t n_len; //!< Input vector data length (number of ADC output values)
     size_t c_len; //!< Number of FIR taps (length of c must be equal length of b)

     float32_t** out; //!< Public pointer to n_dec+1 read-only vectors containing usable output values
     float32_t** buf; //!< Private pointer to internal buffer containing all FIR outputs

     size_t* lens; //!< Public vector of n_dec+1 elements with subsequent output vectors length
     float32_t* in; //!< Private internal input vector with extra items to store current FIR state

     size_t offset; //!< Read-only extra item count of buffers to store subsequent FIR states
     size_t dec; //!< Read-only decimation option (0 in case of CWT and 1 in case of FWT)

} arm_wt_f32_instance;

/**
 * @brief Constructor of arm_wt_f32_instance in case of FWT
 * @param instance [in,out] Filled arm_wt_f32_instance structure
 * @return Error code or arm_wt_status::WT_STATUS_SUCCESS
 * @see arm_wt_f32_instance, arm_wt_status
 *
 * This function constructs FWT object according to pre-filled info. All required fields mentioned
 * in arm_wt_f32_instance must be filled before running this function.
 *
 * In case of FWT output values stored at arm_wt_f32_instance::out consists of N = n_dec+1 elements
 * containing various number of output coefficients. Check arm_wt_f32_instance::lens to determine
 * n-th output vector length.
 *
 */
arm_wt_status arm_fwt_f32_init(arm_wt_f32_instance* instance);

/**
 * @brief Constructor of arm_wt_f32_instance in case of CWT
 * @param instance [in,out] Filled arm_wt_f32_instance struct
 * @return Error code or arm_wt_status::WT_STATUS_SUCCESS
 * @see arm_wt_f32_instance, arm_wt_status
 *
 * This function constructs CWT object according to pre-filled info. All required fields mentioned
 * in arm_wt_f32_instance must be filled before running this function.
 *
 * In case of CWT output values stored at arm_wt_f32_instance::out consists of N = n_dec+1 elements
 * containing the same number of output coefficients equal to arm_wt_f32_instance::n_len. To enable
 * compatibility arm_wt_f32_instance::lens still can be used to determine n-th output vector length.
 *
 */
arm_wt_status arm_cwt_f32_init(arm_wt_f32_instance* instance);

/**
 * @brief Destructor of arm_wt_f32_instance
 * @param instance [in] Instance of arm_wt_f32_instance to free
 *
 * This function frees all allocated data buffers. All fields remains untouched and can be used
 * again after calling arm_fwt_f32_init or arm_cwt_f32_init constructor.
 *
 */
void arm_wt_f32_free(arm_wt_f32_instance* instance);

/**
 * @brief Perform WT algorithm for given input data.
 * @param instance [in,out] Instance of arm_wt_f32_instance to use
 * @param in [in] Pointer to ADC results buffer
 * @see arm_wt_f32_instance, arm_fwt_f32_init, arm_cwt_f32_init
 *
 * This function performs single run of WT algorithm using given instance and input data.
 * The length of in must be equal to arm_wt_f32_instance::n_len. Provided input is scaled
 * and shifted according to formula in' = scale * in + shift. All FIR states are stored in
 * internal arm_wt_f32_instance buffer. Output is stored in arm_wt_f32_instance::out.
 *
 */
void arm_wt_f32_run(
  arm_wt_f32_instance* restrict instance,
  const uint16_t* restrict in);

/**
 * @brief Perform uint16_t -> float32_t conversion with scale and shift
 * @param in [in] Input data pointer
 * @param out [out] Output data pointer
 * @param len [in] Number of elements to convert
 * @param scale [in] Scale factor
 * @param shift [in] Shift value
 *
 * This function performs uint16_t -> float32_t conversion with scale and shift according
 * to formula out = scale * in + shift. Input and output buffers must be length of at least
 * len. This function unroll loop using 4 MAC operations per iteration.
 *
 */
void arm_wt_f32_scale(
  const uint16_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t scale,
  const float32_t shift);

/**
 * @brief Copy float32_t from source to destination without any changes
 * @param src [in] Source data pointer
 * @param dst [out] Destination data pointer
 * @param len [in] Number of elements to copy
 *
 * This function performs float32_t -> float32_t copy without any changes. Input and output buffers
 * must be length of at least len. This function unroll loop using 4 operations per iteration.
 *
 */
void arm_wt_f32_copy(
  const float32_t* restrict scr,
  float32_t* restrict dst,
  const size_t len);

/**
 * @brief Zero-fill float32_t matrix
 * @param dst [out] Destination data pointer
 * @param len [in] Number of elements to process
 *
 * This function performs float32_t zero set. Output buffer must be length of at least len. This
 * function unroll loop using 4 operations per iteration.
 *
 */
void arm_wt_f32_zero(
  float32_t* restrict dst,
  const size_t len);

/**
 * @brief Perform FIR operation for pair of filters
 * @param in [in] Input data pointer
 * @param out_lp [out] Output data pointer for low-pass coefficients
 * @param out_hp [out] Output data pointer for high-pass coefficients
 * @param n_len [in] Number of input data items
 * @param c [in] Pointer to low-pass FIR coefficients
 * @param b [in] Pointer to high-pass FIR coefficients
 * @param c_len [in] Number FIR coefficients
 *
 * This function perform parallel input signal decomposition using given FIR coefficients. The length
 * of out_lp and out_hp must be at least n_len. The length of c and b must be the same and equal
 * to c_len. The actual length of in must be n_len+c_len-1 where the first c_len-1 elements contains
 * previous input values (equivalent to given boundary conditions).
 *
 */
void arm_wt_f32_cwt(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t n_len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len);

/**
 * @brief Perform FIR with decimation for pair of filters
 * @param in [in] Input data pointer
 * @param out_lp [out] Output data pointer for low-pass coefficients
 * @param out_hp [out] Output data pointer for high-pass coefficients
 * @param n_len [in] Number of input data items
 * @param c [in] Pointer to low-pass FIR coefficients
 * @param b [in] Pointer to high-pass FIR coefficients
 * @param c_len [in] Number FIR coefficients
 *
 * This function perform parallel input signal decomposition using given FIR coefficients. The length
 * of out_lp and out_hp must be at least n_len / 2. The length of c and b must be the same and equal
 * to c_len. The actual length of in must be n_len+c_len-1 where the first c_len-1 elements contains
 * previous input values (equivalent to given boundary conditions).
 *
 */
void arm_wt_f32_fwt(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t n_len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len);

/**
 * @brief Perform single-run FWT algorithm
 * @param in [in] Input data pointer
 * @param out [out] Output coefficients pointer
 * @param n_len [in] Number of input data items
 * @param n_dec [in] Number of decomposition iterations
 * @param c [in] Pointer to low-pass FIR coefficients
 * @param b [in] Pointer to high-pass FIR coefficients
 * @param c_len [in] Number FIR coefficients
 *
 * This function perform parallel input signal decomposition using given FIR coefficients. Decomposition
 * is repeated n_dec times, where input of next level is low-pass output from previous level. The length
 * of out_lp and out_hp must be at least n_len / inc. The length of c and b must be the same and equal
 * to c_len. The actual length of in must be n_len+c_len-1 where the last c_len-1 elements contains
 * previous input values (equivalent to given boundary conditions).
 *
 */
void arm_wt_f32_mallat(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t n_len,
  size_t n_dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len);

#endif
