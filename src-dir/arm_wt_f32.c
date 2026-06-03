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

#include "arm_wt_f32.h"

#include <stdlib.h>

arm_wt_status arm_fwt_f32_init(arm_wt_f32_instance* instance)
{
     if (!instance->n_len) return WT_WRONG_NLEN;
     if (!instance->c_len) return WT_WRONG_CLEN;

     if (!instance->c || !instance->b) return WT_WRONG_CPTR;

     if ((instance->n_len >> (instance->n_dec - 1)) < instance->c_len) return WT_WRONG_NDEC;

     instance->offset = instance->c_len - 1;
     instance->dec = 2;

     instance->buf = malloc((instance->n_dec * 2) * sizeof(float32_t*));
     instance->out = malloc((instance->n_dec + 1) * sizeof(float32_t*));

     instance->lens = malloc((instance->n_dec + 1) * sizeof(size_t));

     instance->in = malloc((instance->n_len + instance->offset) * sizeof(float32_t));

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          const size_t curr_len = instance->c_len + (instance->n_len >> (i + 1)) - 1;

          instance->buf[2 * i] = malloc(curr_len * sizeof(float32_t));
          instance->buf[2 * i + 1] = malloc(curr_len * sizeof(float32_t));

          arm_wt_f32_zero(instance->buf[2 * i], curr_len);
          arm_wt_f32_zero(instance->buf[2 * i + 1], curr_len);
     }

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->out[i] = instance->buf[2 * i] + instance->offset;
          instance->lens[i] = instance->n_len >> (i + 1);
     }

     instance->out[instance->n_dec] = instance->buf[2 * instance->n_dec - 1] + instance->offset;
     instance->lens[instance->n_dec] = instance->lens[instance->n_dec - 1];

     arm_wt_f32_zero(instance->in, instance->n_len + instance->offset);

     return WT_STATUS_SUCCESS;
}

arm_wt_status arm_cwt_f32_init(arm_wt_f32_instance* instance)
{
     if (!instance->n_len) return WT_WRONG_NLEN;
     if (!instance->c_len) return WT_WRONG_CLEN;

     if (!instance->c || !instance->b) return WT_WRONG_CPTR;

     if (instance->n_len < instance->c_len) return WT_WRONG_NDEC;

     instance->offset = instance->c_len - 1;
     instance->dec = 1;

     const size_t curr_len = (instance->n_len + instance->offset);

     instance->buf = malloc((instance->n_dec * 2) * sizeof(float32_t*));
     instance->out = malloc((instance->n_dec + 1) * sizeof(float32_t*));

     instance->lens = malloc((instance->n_dec + 1) * sizeof(size_t));

     instance->in = malloc(curr_len * sizeof(float32_t));

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->buf[2 * i] = malloc(curr_len * sizeof(float32_t));
          instance->buf[2 * i + 1] = malloc(curr_len * sizeof(float32_t));

          arm_wt_f32_zero(instance->buf[2 * i], curr_len);
          arm_wt_f32_zero(instance->buf[2 * i + 1], curr_len);
     }

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->out[i] = instance->buf[2 * i] + instance->offset;
          instance->lens[i] = instance->n_len;
     }

     instance->out[instance->n_dec] = instance->buf[2 * instance->n_dec - 1] + instance->offset;
     instance->lens[instance->n_dec] = instance->lens[instance->n_dec - 1];

     arm_wt_f32_zero(instance->in, instance->n_len + instance->offset);

     return WT_STATUS_SUCCESS;
}

void arm_wt_f32_free(arm_wt_f32_instance* instance)
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

void arm_wt_f32_run(
  arm_wt_f32_instance* restrict instance,
  const uint16_t* restrict in)
{
     arm_wt_f32_copy(
       instance->in + instance->n_len,
       instance->in,
       instance->offset);

     arm_wt_f32_scale(
       in,
       instance->in + instance->offset,
       instance->n_len,
       instance->scale,
       instance->shift);

     if (instance->n_dec > 1) arm_wt_f32_copy(
       instance->buf[1] + instance->lens[0],
       instance->buf[1],
       instance->offset);

     if (instance->dec > 1) arm_wt_f32_firdec(
       instance->in,
       instance->buf[1] + instance->offset,
       instance->buf[0] + instance->offset,
       instance->n_len,
       instance->c,
       instance->b,
       instance->c_len,
       instance->dec);
     else arm_wt_f32_fir(
       instance->in,
       instance->buf[1] + instance->offset,
       instance->buf[0] + instance->offset,
       instance->n_len,
       instance->c,
       instance->b,
       instance->c_len);

     for (size_t i = 1; i < instance->n_dec; ++i)
     {
          if (i + 1 != instance->n_dec) arm_wt_f32_copy(
            instance->buf[2 * i + 1] + instance->lens[i],
            instance->buf[2 * i + 1],
            instance->offset);

          if (instance->dec > 1) arm_wt_f32_firdec(
            instance->buf[2 * i - 1],
            instance->buf[2 * i + 1] + instance->offset,
            instance->buf[2 * i] + instance->offset,
            instance->lens[i - 1],
            instance->c,
            instance->b,
            instance->c_len,
            instance->dec);
          else arm_wt_f32_fir(
            instance->buf[2 * i - 1],
            instance->buf[2 * i + 1] + instance->offset,
            instance->buf[2 * i] + instance->offset,
            instance->lens[i - 1],
            instance->c,
            instance->b,
            instance->c_len);
     }
}

void arm_wt_f32_scale(
  const uint16_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t scale,
  const float32_t shift)
{
     size_t blk = len >> 3;
     size_t rem = len & 7;

     while (blk--)
     {
          *out++ = *in++ * scale + shift;
          *out++ = *in++ * scale + shift;
          *out++ = *in++ * scale + shift;
          *out++ = *in++ * scale + shift;
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

void arm_wt_f32_firdec(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t n_len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len,
  const size_t inc)
{
     const float32_t *ip0, *ip1, *ip2, *ip3;
     const float32_t *cp, *bp;

     float32_t al0, al1, al2, al3;
     float32_t ah0, ah1, ah2, ah3;

     float32_t x0, x1, x2, x3;
     float32_t c0, b0;

     size_t cnt_a, cnt_b;

     cnt_a = (n_len / inc) >> 2;

     while (cnt_a)
     {
          cp = c;
          bp = b;

          ip0 = in;
          ip1 = ip0 + inc;
          ip2 = ip1 + inc;
          ip3 = ip2 + inc;

          al0 = 0.0f;
          al1 = 0.0f;
          al2 = 0.0f;
          al3 = 0.0f;

          ah0 = 0.0f;
          ah1 = 0.0f;
          ah2 = 0.0f;
          ah3 = 0.0f;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;
               x2 = *ip2++;
               x3 = *ip3++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;
               x2 = *ip2++;
               x3 = *ip3++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;
               x2 = *ip2++;
               x3 = *ip3++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;
               x2 = *ip2++;
               x3 = *ip3++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               cnt_b -= 1;
          }

          cnt_b = c_len & 3;

          while (cnt_b)
          {
               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;
               x2 = *ip2++;
               x3 = *ip3++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               cnt_b -= 1;
          }

          *out_lp++ = al0;
          *out_lp++ = al1;
          *out_lp++ = al2;
          *out_lp++ = al3;

          *out_hp++ = ah0;
          *out_hp++ = ah1;
          *out_hp++ = ah2;
          *out_hp++ = ah3;

          in += 4 * inc;

          cnt_a -= 1;
     }

     cnt_a = (n_len / inc) & 3;

     while (cnt_a)
     {
          cp = c;
          bp = b;

          ip0 = in;

          al0 = 0.0f;
          ah0 = 0.0f;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               x0 = *ip0++;
               x1 = *ip0++;
               x2 = *ip0++;
               x3 = *ip0++;

               al0 += *cp++ * x0;
               al0 += *cp++ * x1;
               al0 += *cp++ * x2;
               al0 += *cp++ * x3;

               ah0 += *bp++ * x0;
               ah0 += *bp++ * x1;
               ah0 += *bp++ * x2;
               ah0 += *bp++ * x3;

               cnt_b -= 1;
          }

          cnt_b = c_len & 3;

          while (cnt_b)
          {
               x0 = *ip0++;

               al0 += *cp++ * x0;
               ah0 += *bp++ * x0;

               cnt_b -= 1;
          }

          *out_lp++ = al0;
          *out_hp++ = ah0;

          in += inc;

          cnt_a -= 1;
     }
}

void arm_wt_f32_fir(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t n_len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len)
{
     const float32_t *ip, *cp, *bp;

     float32_t al0, al1, al2, al3;
     float32_t ah0, ah1, ah2, ah3;

     float32_t x0, x1, x2, x3;
     float32_t c0, b0;

     size_t cnt_a, cnt_b;

     cnt_a = n_len >> 2;

     while (cnt_a)
     {
          ip = in;
          cp = c;
          bp = b;

          al0 = 0.0f;
          al1 = 0.0f;
          al2 = 0.0f;
          al3 = 0.0f;

          ah0 = 0.0f;
          ah1 = 0.0f;
          ah2 = 0.0f;
          ah3 = 0.0f;

          x0 = *ip++;
          x1 = *ip++;
          x2 = *ip++;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               c0 = *cp++;
               b0 = *bp++;
               x3 = *ip++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               c0 = *cp++;
               b0 = *bp++;
               x0 = *ip++;

               al0 += x1 * c0;
               al1 += x2 * c0;
               al2 += x3 * c0;
               al3 += x0 * c0;

               ah0 += x1 * b0;
               ah1 += x2 * b0;
               ah2 += x3 * b0;
               ah3 += x0 * b0;

               c0 = *cp++;
               b0 = *bp++;
               x1 = *ip++;

               al0 += x2 * c0;
               al1 += x3 * c0;
               al2 += x0 * c0;
               al3 += x1 * c0;

               ah0 += x2 * b0;
               ah1 += x3 * b0;
               ah2 += x0 * b0;
               ah3 += x1 * b0;

               c0 = *cp++;
               b0 = *bp++;
               x2 = *ip++;

               al0 += x3 * c0;
               al1 += x0 * c0;
               al2 += x1 * c0;
               al3 += x2 * c0;

               ah0 += x3 * b0;
               ah1 += x0 * b0;
               ah2 += x1 * b0;
               ah3 += x2 * b0;

               cnt_b -= 1;
          }

          cnt_b = c_len & 3;

          while (cnt_b)
          {
               c0 = *cp++;
               b0 = *bp++;
               x3 = *ip++;

               al0 += x0 * c0;
               al1 += x1 * c0;
               al2 += x2 * c0;
               al3 += x3 * c0;

               ah0 += x0 * b0;
               ah1 += x1 * b0;
               ah2 += x2 * b0;
               ah3 += x3 * b0;

               x0 = x1;
               x1 = x2;
               x2 = x3;

               cnt_b -= 1;
          }

          *out_lp++ = al0;
          *out_lp++ = al1;
          *out_lp++ = al2;
          *out_lp++ = al3;

          *out_hp++ = ah0;
          *out_hp++ = ah1;
          *out_hp++ = ah2;
          *out_hp++ = ah3;

          in += 4;

          cnt_a -= 1;
     }

     cnt_a = n_len & 3;

     while (cnt_a)
     {
          cp = c;
          bp = b;

          ip = in;

          al0 = 0.0f;
          ah0 = 0.0f;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               x0 = *ip++;
               x1 = *ip++;
               x2 = *ip++;
               x3 = *ip++;

               al0 += *cp++ * x0;
               al0 += *cp++ * x1;
               al0 += *cp++ * x2;
               al0 += *cp++ * x3;

               ah0 += *bp++ * x0;
               ah0 += *bp++ * x1;
               ah0 += *bp++ * x2;
               ah0 += *bp++ * x3;

               cnt_b -= 1;
          }

          cnt_b = c_len & 3;

          while (cnt_b)
          {
               x0 = *ip++;

               al0 += *cp++ * x0;
               ah0 += *bp++ * x0;

               cnt_b -= 1;
          }

          *out_lp++ = al0;
          *out_hp++ = ah0;

          in += 1;

          cnt_a -= 1;
     }
}

void arm_wt_f32_copy(
  const float32_t* restrict src,
  float32_t* restrict dst,
  const size_t len)
{
     size_t blk = len >> 2;
     size_t rem = len & 3;

     while (blk--)
     {
          *dst++ = *src++;
          *dst++ = *src++;
          *dst++ = *src++;
          *dst++ = *src++;
     }

     while (rem--)
     {
          *dst++ = *src++;
     }
}

void arm_wt_f32_zero(
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

void arm_wt_f32_mallat(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t n_len,
  size_t n_dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t c_len)
{
     const size_t c_len_m1 = c_len - 1;

     float32_t
       tmp_a[n_len / 2 + c_len_m1],
       tmp_b[n_len / 2 + c_len_m1];

     const float32_t* restrict _in = in;
     float32_t* restrict _out = tmp_a;

     while (n_dec--)
     {
          const size_t n_len_p2 = n_len >> 1;

          if (n_dec)
          {
               arm_wt_f32_firdec(_in, _out, out + n_len_p2, n_len, c, b, c_len, 2);
               arm_wt_f32_copy(_out, _out + n_len_p2, c_len_m1);
          }
          else
          {
               arm_wt_f32_firdec(_in, out, out + n_len_p2, n_len, c, b, c_len, 2);
          }

          if (_out == tmp_a)
          {
               _out = tmp_b;
               _in = tmp_a;
          }
          else
          {
               _out = tmp_a;
               _in = tmp_b;
          }

          n_len = n_len_p2;
     }
}
