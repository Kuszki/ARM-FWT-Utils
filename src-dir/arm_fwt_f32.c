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

#include "arm_fwt_f32.h"

#include <stdlib.h>

static const float32_t INV_SQRT2 = 0.7071067811865475f;

arm_fwt_status arm_fwt_f32_init(arm_fwt_f32_instance* instance)
{
     if (!instance->n_len) return FWT_WRONG_NLEN;

     if (!(instance->n_len >> instance->n_dec)) return FWT_WRONG_NDEC;

     instance->offset = instance->c_len - 1;

     instance->buf = malloc((instance->n_dec * 2) * sizeof(float32_t*));
     instance->out = malloc((instance->n_dec + 1) * sizeof(float32_t*));

     instance->lens = malloc((instance->n_dec + 1) * sizeof(size_t));

     instance->in = malloc((instance->n_len + instance->offset) * sizeof(float32_t));

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          const size_t curr_len = instance->c_len + (instance->n_len >> (i + 1)) - 1;

          instance->buf[2 * i] = malloc(curr_len * sizeof(float32_t));
          instance->buf[2 * i + 1] = malloc(curr_len * sizeof(float32_t));

          arm_fwt_f32_zero(instance->buf[2 * i], curr_len);
          arm_fwt_f32_zero(instance->buf[2 * i + 1], curr_len);
     }

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->out[i] = instance->buf[2 * i] + instance->offset;
          instance->lens[i] = instance->n_len >> (i + 1);
     }

     instance->out[instance->n_dec] = instance->buf[2 * instance->n_dec - 1] + instance->offset;
     instance->lens[instance->n_dec] = instance->lens[instance->n_dec - 1];

     arm_fwt_f32_zero(instance->in, instance->n_len + instance->offset);

     return FWT_STATUS_SUCCESS;
}

void arm_fwt_f32_free(arm_fwt_f32_instance* instance)
{
     for (int32_t i = instance->n_dec * 2 - 1; i >= 0; --i)
     {
          free(instance->buf[i]);
     }

     free(instance->in);
     free(instance->lens);
     free(instance->out);
     free(instance->buf);
}

void arm_fwt_f32_run(
  arm_fwt_f32_instance* restrict instance,
  const uint32_t* restrict in)
{
     arm_fwt_f32_copy(
       instance->in + instance->n_len,
       instance->in,
       instance->offset);

     arm_fwt_f32_scale(
       in,
       instance->in + instance->offset,
       instance->n_len,
       instance->scale,
       instance->shift);

     if (instance->n_dec > 1) arm_fwt_f32_copy(
       instance->buf[1] + instance->lens[0],
       instance->buf[1],
       instance->offset);

     arm_fwt_f32_firdec(
       instance->in,
       instance->buf[1] + instance->offset,
       instance->buf[0] + instance->offset,
       instance->n_len,
       instance->c,
       instance->b,
       instance->c_len);

     for (size_t i = 1; i < instance->n_dec; ++i)
     {
          if (i + 1 != instance->n_dec) arm_fwt_f32_copy(
            instance->buf[2 * i + 1] + instance->lens[i],
            instance->buf[2 * i + 1],
            instance->offset);

          arm_fwt_f32_firdec(
            instance->buf[2 * i - 1],
            instance->buf[2 * i + 1] + instance->offset,
            instance->buf[2 * i] + instance->offset,
            instance->lens[i - 1],
            instance->c,
            instance->b,
            instance->c_len);
     }
}

void arm_fwt_f32_scale(
  const uint32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t scale,
  const float32_t shift)
{
     const uint32_t* start = in;

     size_t blk = len >> 2;
     size_t rem = len & 3;

     while (blk--)
     {
          *out++ = *in++ * scale + shift;
          *out++ = *in++ * scale + shift;
          *out++ = *in++ * scale + shift;
          *out++ = *in++ * scale + shift;
     }

     while (rem--)
     {
          *out++ = *in++ * scale + shift;
     }
}

void arm_fwt_f32_firdec(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t n_len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len)
{
     for (size_t i = 0; i < n_len; i += 2)
     {
          const float32_t* restrict ip = in + i;

          const float32_t* restrict cp = c;
          const float32_t* restrict bp = b;

          float32_t acc_hp = 0.0f;
          float32_t acc_lp = 0.0f;

          size_t blk = c_len >> 2;
          size_t rem = c_len & 3;

          while (blk--)
          {
               acc_lp += *cp++ * *ip;
               acc_hp += *bp++ * *ip++;

               acc_lp += *cp++ * *ip;
               acc_hp += *bp++ * *ip++;

               acc_lp += *cp++ * *ip;
               acc_hp += *bp++ * *ip++;

               acc_lp += *cp++ * *ip;
               acc_hp += *bp++ * *ip++;
          }

          while (rem--)
          {
               acc_lp += *cp++ * *ip;
               acc_hp += *bp++ * *ip++;
          }

          *out_hp++ = acc_hp * INV_SQRT2;
          *out_lp++ = acc_lp * INV_SQRT2;
     }
}

void arm_fwt_f32_copy(
  const float32_t* restrict scr,
  float32_t* restrict dst,
  const size_t len)
{
     size_t blk = len >> 2;
     size_t rem = len & 3;

     while (blk--)
     {
          *dst++ = *scr++;
          *dst++ = *scr++;
          *dst++ = *scr++;
          *dst++ = *scr++;
     }

     while (rem--)
     {
          *dst++ = *scr++;
     }
}

void arm_fwt_f32_zero(
  float32_t* restrict dst,
  const size_t len)
{
     size_t blk = len >> 2;
     size_t rem = len & 3;

     while (blk--)
     {
          *dst++ = 0.0f;
          *dst++ = 0.0f;
          *dst++ = 0.0f;
          *dst++ = 0.0f;
     }

     while (rem--)
     {
          *dst++ = 0.0f;
     }
}
