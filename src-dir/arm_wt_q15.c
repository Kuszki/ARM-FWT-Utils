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

#include "arm_wt_q15.h"

#include <stdlib.h>

arm_wt_status arm_fwt_q15_init(arm_wt_q15_instance* instance)
{
     if (!instance->n_len) return WT_WRONG_NLEN;
     if (!instance->c_len) return WT_WRONG_CLEN;

     if (!instance->c || !instance->b) return WT_WRONG_CPTR;

     if ((instance->n_len >> (instance->n_dec - 1)) < instance->c_len) return WT_WRONG_NDEC;

     instance->offset = instance->c_len - 1;
     instance->dec = 2;

     instance->buf = malloc((instance->n_dec * 2) * sizeof(q15_t*));
     instance->out = malloc((instance->n_dec + 1) * sizeof(q15_t*));

     instance->lens = malloc((instance->n_dec + 1) * sizeof(size_t));

     instance->in = malloc((instance->n_len + instance->offset) * sizeof(q15_t));

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          const size_t curr_len = instance->c_len + (instance->n_len >> (i + 1)) - 1;

          instance->buf[2 * i] = malloc(curr_len * sizeof(q15_t));
          instance->buf[2 * i + 1] = malloc(curr_len * sizeof(q15_t));

          arm_wt_q15_zero(instance->buf[2 * i], curr_len);
          arm_wt_q15_zero(instance->buf[2 * i + 1], curr_len);
     }

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->out[i] = instance->buf[2 * i] + instance->offset;
          instance->lens[i] = instance->n_len >> (i + 1);
     }

     instance->out[instance->n_dec] = instance->buf[2 * instance->n_dec - 1] + instance->offset;
     instance->lens[instance->n_dec] = instance->lens[instance->n_dec - 1];

     arm_wt_q15_zero(instance->in, instance->n_len + instance->offset);

     return WT_STATUS_SUCCESS;
}

arm_wt_status arm_cwt_q15_init(arm_wt_q15_instance* instance)
{
     if (!instance->n_len) return WT_WRONG_NLEN;
     if (!instance->c_len) return WT_WRONG_CLEN;

     if (!instance->c || !instance->b) return WT_WRONG_CPTR;

     if (instance->n_len < instance->c_len) return WT_WRONG_NDEC;

     instance->offset = instance->c_len - 1;
     instance->dec = 1;

     const size_t curr_len = (instance->n_len + instance->offset);

     instance->buf = malloc((instance->n_dec * 2) * sizeof(q15_t*));
     instance->out = malloc((instance->n_dec + 1) * sizeof(q15_t*));

     instance->lens = malloc((instance->n_dec + 1) * sizeof(size_t));

     instance->in = malloc(curr_len * sizeof(q15_t));

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->buf[2 * i] = malloc(curr_len * sizeof(q15_t));
          instance->buf[2 * i + 1] = malloc(curr_len * sizeof(q15_t));

          arm_wt_q15_zero(instance->buf[2 * i], curr_len);
          arm_wt_q15_zero(instance->buf[2 * i + 1], curr_len);
     }

     for (size_t i = 0; i < instance->n_dec; ++i)
     {
          instance->out[i] = instance->buf[2 * i] + instance->offset;
          instance->lens[i] = instance->n_len;
     }

     instance->out[instance->n_dec] = instance->buf[2 * instance->n_dec - 1] + instance->offset;
     instance->lens[instance->n_dec] = instance->lens[instance->n_dec - 1];

     arm_wt_q15_zero(instance->in, instance->n_len + instance->offset);

     return WT_STATUS_SUCCESS;
}

void arm_wt_q15_free(arm_wt_q15_instance* instance)
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

void arm_wt_q15_run(
  arm_wt_q15_instance* restrict instance,
  const uint16_t* restrict in)
{
     arm_wt_q15_copy(
       instance->in + instance->n_len,
       instance->in,
       instance->offset);

     // arm_wt_q15_scale(
     //   in,
     //   instance->in + instance->offset,
     //   instance->n_len,
     //   instance->scale,
     //   instance->shift);

     if (instance->n_dec > 1) arm_wt_q15_copy(
       instance->buf[1] + instance->lens[0],
       instance->buf[1],
       instance->offset);

     if (instance->dec) arm_wt_q15_fwt(
       instance->in,
       instance->buf[1] + instance->offset,
       instance->buf[0] + instance->offset,
       instance->n_len,
       instance->c,
       instance->b,
       instance->c_len);
     else arm_wt_q15_cwt(
       instance->in,
       instance->buf[1] + instance->offset,
       instance->buf[0] + instance->offset,
       instance->n_len,
       instance->c,
       instance->b,
       instance->c_len);

     for (size_t i = 1; i < instance->n_dec; ++i)
     {
          if (instance->n_dec != i + 1) arm_wt_q15_copy(
            instance->buf[2 * i + 1] + instance->lens[i],
            instance->buf[2 * i + 1],
            instance->offset);

          if (instance->dec > 1) arm_wt_q15_fwt(
            instance->buf[2 * i - 1],
            instance->buf[2 * i + 1] + instance->offset,
            instance->buf[2 * i] + instance->offset,
            instance->lens[i - 1],
            instance->c,
            instance->b,
            instance->c_len);
          else arm_wt_q15_cwt(
            instance->buf[2 * i - 1],
            instance->buf[2 * i + 1] + instance->offset,
            instance->buf[2 * i] + instance->offset,
            instance->lens[i - 1],
            instance->c,
            instance->b,
            instance->c_len);
     }
}

// void arm_wt_q15_scale(
//   const uint16_t* restrict in,
//   q15_t* restrict out,
//   const size_t len,
//   const q15_t scale,
//   const q15_t shift)
// {
//      size_t blk = len >> 3;
//      size_t rem = len & 7;

//      while (blk--)
//      {
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//           *out++ = *in++ * scale + shift;
//      }

//      while (rem--)
//      {
//           *out++ = *in++ * scale + shift;
//      }
// }

void arm_wt_q15_fwt_A(
  const q15_t* restrict in,
  q15_t* restrict out_lp,
  q15_t* restrict out_hp,
  const size_t n_len,
  const q15_t* restrict c,
  const q15_t* restrict b,
  const size_t c_len)
{
     const q15_t *ip0, *ip1;
     const q15_t *cp, *bp;

     q31_t al0, al1;
     q31_t ah0, ah1;

     q31_t x0, x1;
     q31_t c0, b0;

     size_t cnt_a, cnt_b;

     cnt_a = n_len >> 2;

     while (cnt_a)
     {
          ip0 = in;
          ip1 = in + 2;

          cp = c;
          bp = b;

          al0 = 0;
          al1 = 0;

          ah0 = 0;
          ah1 = 0;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               c0 = read_q15x2_ia(&cp);
               b0 = read_q15x2_ia(&bp);

               x0 = read_q15x2_ia(&ip0);
               x1 = read_q15x2_ia(&ip1);

               al0 = __SMLAD(x0, c0, al0);
               al1 = __SMLAD(x1, c0, al1);

               ah0 = __SMLAD(x0, b0, ah0);
               ah1 = __SMLAD(x1, b0, ah1);

               c0 = read_q15x2_ia(&cp);
               b0 = read_q15x2_ia(&bp);

               x0 = read_q15x2_ia(&ip0);
               x1 = read_q15x2_ia(&ip1);

               al0 = __SMLAD(x0, c0, al0);
               al1 = __SMLAD(x1, c0, al1);

               ah0 = __SMLAD(x0, b0, ah0);
               ah1 = __SMLAD(x1, b0, ah1);

               cnt_b -= 1;
          }

          *out_lp++ = (q15_t)(__SSAT((al0 >> 15), 16));
          *out_lp++ = (q15_t)(__SSAT((al1 >> 15), 16));

          *out_hp++ = (q15_t)(__SSAT((ah0 >> 15), 16));
          *out_hp++ = (q15_t)(__SSAT((ah1 >> 15), 16));

          in += 4;

          cnt_a -= 1;
     }

     cnt_a = (n_len >> 1) & 3;

     while (cnt_a)
     {
          ip0 = in;

          cp = c;
          bp = b;

          al0 = 0;
          ah0 = 0;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               c0 = read_q15x2_ia(&cp);
               b0 = read_q15x2_ia(&bp);

               x0 = read_q15x2_ia(&ip0);

               al0 = __SMLAD(x0, c0, al0);
               ah0 = __SMLAD(x0, b0, ah0);

               c0 = read_q15x2_ia(&cp);
               b0 = read_q15x2_ia(&bp);

               x0 = read_q15x2_ia(&ip0);

               al0 = __SMLAD(x0, c0, al0);
               ah0 = __SMLAD(x0, b0, ah0);

               cnt_b -= 1;
          }

          *out_lp++ = (q15_t)(__SSAT((al0 >> 15), 16));
          *out_hp++ = (q15_t)(__SSAT((ah0 >> 15), 16));

          in += 2;

          cnt_a -= 1;
     }
}

void arm_wt_q15_fwt(
  const q15_t* restrict in,
  q15_t* restrict out_lp,
  q15_t* restrict out_hp,
  const size_t n_len,
  const q15_t* restrict c,
  const q15_t* restrict b,
  const size_t c_len)
{
     const q31_t *ip0, *ip1;
     const q31_t *cp, *bp;

     q31_t al0, al1;
     q31_t ah0, ah1;

     q31_t x0, x1;
     q31_t c0, b0;

     size_t cnt_a, cnt_b;

     cnt_a = n_len >> 2;

     while (cnt_a)
     {
          ip0 = (q31_t*)(in);
          ip1 = (q31_t*)(in + 2);

          cp = (q31_t*)(c);
          bp = (q31_t*)(b);

          al0 = 0;
          al1 = 0;

          ah0 = 0;
          ah1 = 0;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;

               al0 = __SMLAD(x0, c0, al0);
               al1 = __SMLAD(x1, c0, al1);

               ah0 = __SMLAD(x0, b0, ah0);
               ah1 = __SMLAD(x1, b0, ah1);

               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;
               x1 = *ip1++;

               al0 = __SMLAD(x0, c0, al0);
               al1 = __SMLAD(x1, c0, al1);

               ah0 = __SMLAD(x0, b0, ah0);
               ah1 = __SMLAD(x1, b0, ah1);

               cnt_b -= 1;
          }

          *out_lp++ = (q15_t)(__SSAT((al0 >> 15), 16));
          *out_lp++ = (q15_t)(__SSAT((al1 >> 15), 16));

          *out_hp++ = (q15_t)(__SSAT((ah0 >> 15), 16));
          *out_hp++ = (q15_t)(__SSAT((ah1 >> 15), 16));

          in += 4;

          cnt_a -= 1;
     }

     cnt_a = (n_len >> 1) & 3;

     while (cnt_a)
     {
          ip0 = (q31_t*)in;

          cp = (q31_t*)c;
          bp = (q31_t*)b;

          al0 = 0;
          ah0 = 0;

          cnt_b = c_len >> 2;

          while (cnt_b)
          {
               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;

               al0 = __SMLAD(x0, c0, al0);
               ah0 = __SMLAD(x0, b0, ah0);

               c0 = *cp++;
               b0 = *bp++;

               x0 = *ip0++;

               al0 = __SMLAD(x0, c0, al0);
               ah0 = __SMLAD(x0, b0, ah0);

               cnt_b -= 1;
          }

          *out_lp++ = (q15_t)(__SSAT((al0 >> 15), 16));
          *out_hp++ = (q15_t)(__SSAT((ah0 >> 15), 16));

          in += 2;

          cnt_a -= 1;
     }
}

void arm_wt_q15_fwt_S(
  const q15_t* restrict in,
  q15_t* restrict out_lp,
  q15_t* restrict out_hp,
  const size_t n_len,
  const q15_t* restrict c,
  const q15_t* restrict b,
  const size_t c_len)
{

     for (size_t i = 0; i < n_len; i += 2)
     {
          q31_t acc_hp = 0, acc_lp = 0;

          for (size_t j = 0; j < c_len; j += 1)
          {
               acc_lp += (q31_t)(in[i + j] * c[j]);
               acc_hp += (q31_t)(in[i + j] * b[j]);
          }

          *out_lp++ = (q15_t)(__SSAT((acc_lp >> 15), 16));
          *out_hp++ = (q15_t)(__SSAT((acc_hp >> 15), 16));
     }
}

void arm_wt_q15_cwt(
  const q15_t* restrict in,
  q15_t* restrict out_lp,
  q15_t* restrict out_hp,
  const size_t n_len,
  const q15_t* restrict c,
  const q15_t* restrict b,
  const size_t c_len)
{
     // const q15_t *ip, *cp, *bp;

     // q15_t al0, al1, al2, al3;
     // q15_t ah0, ah1, ah2, ah3;

     // q15_t x0, x1, x2, x3;
     // q15_t c0, b0;

     // size_t cnt_a, cnt_b;

     // cnt_a = n_len >> 2;

     // while (cnt_a)
     // {
     // }
}

void arm_wt_q15_copy(
  const q15_t* restrict src,
  q15_t* restrict dst,
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

void arm_wt_q15_zero(
  q15_t* restrict dst,
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

void arm_wt_q15_mallat(
  const q15_t* restrict in,
  q15_t* restrict out,
  size_t n_len,
  size_t n_dec,
  const q15_t* restrict c,
  const q15_t* restrict b,
  const size_t c_len)
{
     const size_t c_len_m1 = c_len - 1;

     q15_t
       tmp_a[n_len / 2 + c_len_m1],
       tmp_b[n_len / 2 + c_len_m1];

     const q15_t* restrict _in = in;
     q15_t* restrict _out = tmp_a;

     while (n_dec--)
     {
          const size_t n_len_p2 = n_len >> 1;

          if (n_dec)
          {
               arm_wt_q15_fwt(_in, _out, out + n_len_p2, n_len, c, b, c_len);
               arm_wt_q15_copy(_out, _out + n_len_p2, c_len_m1);
          }
          else
          {
               arm_wt_q15_fwt(_in, out, out + n_len_p2, n_len, c, b, c_len);
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
