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

#include "arm_mal_f32.h"

static const float32_t INV_SQRT2 = 0.7071067811865475f;

int fwt_gen_b(
  const float32_t* restrict c,
  float32_t* restrict b,
  const size_t NC)
{
     for (int i = 0; i < NC; i += 1)
     {
          if (i & 0b1) b[i] = -c[NC - i - 1];
          else b[i] = c[NC - i - 1];
     }

     return 0;
}

int fwt_malat(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t len,
  size_t dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t clen)
{
     float32_t tmp_a[len / 2], tmp_b[len / 2];

     const float32_t* restrict _in = in;
     float32_t* restrict _out = tmp_a;

     while (dec--)
     {
          fwt_fir_dec(_in, out + len / 2, len, b, clen);

          if (dec) fwt_fir_dec(_in, _out, len, c, clen);
          else fwt_fir_dec(_in, out, len, c, clen);

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

          len /= 2;
     }

     return 0;
}

int fwt_malat_ext(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t len,
  size_t dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t clen)
{
     float32_t tmp_a[len / 2 + clen], tmp_b[len / 2 + clen];

     const float32_t* restrict _in = in;
     float32_t* restrict _out = tmp_a;

     while (dec--)
     {
          fwt_fir_dec_ext(_in, out + len / 2, len, b, clen);

          if (!dec) fwt_fir_dec_ext(_in, out, len, c, clen);
          else
          {
               fwt_fir_dec_ext(_in, _out, len, c, clen);
               memcpy(_out + len / 2, _out, clen * sizeof(float32_t));
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

          len /= 2;
     }

     return 0;
}

int fwt_malat_ext_par(
  const float32_t* restrict in,
  float32_t* restrict out,
  size_t len,
  size_t dec,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t clen)
{
     float32_t tmp_a[len / 2 + clen], tmp_b[len / 2 + clen];

     const float32_t* restrict _in = in;
     float32_t* restrict _out = tmp_a;

     while (dec--)
     {
          if (!dec) fwt_fir_dec_ext_par(_in, out, out + len / 2, len, c, b, clen);
          else
          {
               fwt_fir_dec_ext_par(_in, _out, out + len / 2, len, c, b, clen);
               memcpy(_out + len / 2, _out, clen * sizeof(float32_t));
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

          len /= 2;
     }

     return 0;
}

// wersja bazowa, zgodnie ze wzorem
int fwt_fir_dec(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     for (size_t i = 0; i < len / 2; ++i)
     {
          out[i] = 0.00000000;

          for (size_t j = 0; j < clen; ++j)
          {
               out[i] += c[j] * in[(2 * i + j) % len];
          }

          out[i] *= INV_SQRT2;
     }

     return 0;
}

int fwt_fir_dec_ext(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     for (size_t i = 0; i < len / 2; ++i)
     {
          out[i] = 0.00000000;

          for (size_t j = 0; j < clen; ++j)
          {
               out[i] += c[j] * in[2 * i + j];
          }

          out[i] *= INV_SQRT2;
     }

     return 0;
}

int fwt_fir_dec_ext_par(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t clen)
{
     for (size_t i = 0; i < len; i += 2)
     {
          const float32_t* restrict ip = in + i;

          const float32_t* restrict cp = c;
          const float32_t* restrict bp = b;

          float32_t acc_hp = 0.0f;
          float32_t acc_lp = 0.0f;

          size_t blk = clen >> 2;
          size_t rem = clen & 3;

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

     return 0;
}

int fwt_adc_scale(
  const uint16_t* in,
  float32_t* out,
  const size_t N,
  const float32_t scale)
{
     size_t blk = N >> 2;
     size_t rem = N & 3;

     while (blk--)
     {
          *out++ = *in++ * scale;
          *out++ = *in++ * scale;
          *out++ = *in++ * scale;
          *out++ = *in++ * scale;
     }

     while (rem--)
     {
          *out++ = *in++ * scale;
     }

     return 0;
}

int fwt_adc_scale_ext(
  const uint16_t* in,
  float32_t* out,
  const size_t N,
  const size_t NC,
  const float32_t scale)
{
     const uint16_t* start = in;

     size_t blk = N >> 2;
     size_t rem = N & 3;

     while (blk--)
     {
          *out++ = *in++ * scale;
          *out++ = *in++ * scale;
          *out++ = *in++ * scale;
          *out++ = *in++ * scale;
     }

     while (rem--)
     {
          *out++ = *in++ * scale;
     }

     blk = NC >> 2;
     rem = NC & 3;

     while (blk--)
     {
          *out++ = *start++ * scale;
          *out++ = *start++ * scale;
          *out++ = *start++ * scale;
          *out++ = *start++ * scale;
     }

     while (rem--)
     {
          *out++ = *start++ * scale;
     }

     return 0;
}
