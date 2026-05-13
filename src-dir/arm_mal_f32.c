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
               arm_copy_f32(_out, _out + len / 2, clen);
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
               arm_copy_f32(_out, _out + len / 2, clen);
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

// wersja bazowa, z zawiniętymi danymi
int fwt_fir_dec_ext_A(
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

// wersja unroll-4, wskaźniki
int fwt_fir_dec_ext(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     const size_t M = len >> 1;

     for (size_t i = 0; i < M; ++i)
     {
          const float32_t* restrict cp = in + (i << 1);
          const float32_t* restrict xp = c;

          float32_t acc = 0.0f;

          size_t blk = clen >> 2;
          size_t rem = clen & 3;

          while (blk--)
          {
               acc += *xp++ * *cp++;
               acc += *xp++ * *cp++;
               acc += *xp++ * *cp++;
               acc += *xp++ * *cp++;
          }

          while (rem--)
          {
               acc += *xp++ * *cp++;
          }

          *out++ = acc * INV_SQRT2;
     }

     return 0;
}

int fwt_fir_dec_ext_par_A(
  const float32_t* restrict in,
  float32_t* restrict out_lp,
  float32_t* restrict out_hp,
  const size_t len,
  const float32_t* restrict c,
  const float32_t* restrict b,
  const size_t clen)
{
     const size_t M = len >> 1;

     for (size_t i = 0; i < M; ++i)
     {
          const float32_t* restrict ip = in + (i << 1);

          const float32_t* restrict cp = c;
          const float32_t* restrict bp = b;

          float32_t acc_hp = 0.0f;
          float32_t acc_lp = 0.0f;

          size_t blk = clen >> 2;
          size_t rem = clen & 3;

          float32_t x;

          while (blk--)
          {
               x = *ip++;
               acc_lp += *cp++ * x;
               acc_hp += *bp++ * x;

               x = *ip++;
               acc_lp += *cp++ * x;
               acc_hp += *bp++ * x;

               x = *ip++;
               acc_lp += *cp++ * x;
               acc_hp += *bp++ * x;

               x = *ip++;
               acc_lp += *cp++ * x;
               acc_hp += *bp++ * x;
          }

          while (rem--)
          {
               x = *ip++;
               acc_lp += *cp++ * x;
               acc_hp += *bp++ * x;
          }

          *out_hp++ = acc_hp * INV_SQRT2;
          *out_lp++ = acc_lp * INV_SQRT2;
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

int fwt_fir_dec_D(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     const size_t MASK = len - 1;
     const size_t M = len >> 1;

     for (size_t i = 0; i < M; ++i)
     {
          float32_t acc = 0.0f;
          size_t j = 0;

          size_t n0 = (i << 1) & MASK;
          size_t n1 = (n0 + 1) & MASK;
          size_t n2 = (n0 + 2) & MASK;
          size_t n3 = (n0 + 3) & MASK;

          while (j + 3 < clen)
          {
               acc += c[j] * in[n0];
               acc += c[j + 1] * in[n1];
               acc += c[j + 2] * in[n2];
               acc += c[j + 3] * in[n3];

               n0 = (n0 + 4) & MASK;
               n1 = (n1 + 4) & MASK;
               n2 = (n2 + 4) & MASK;
               n3 = (n3 + 4) & MASK;

               j += 4;
          }

          while (j < clen)
          {
               acc += c[j] * in[n0];
               n0 = (n0 + 1) & MASK;
               j += 1;
          }

          *out++ = acc * INV_SQRT2;
     }

     return 0;
}

int fwt_fir_dec_E(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     const size_t MASK = len - 1;
     const size_t M = len >> 1;

     for (size_t i = 0; i < M; ++i)
     {
          const float32_t* cp = c;
          float32_t acc = 0.0f;
          size_t j = 0;

          size_t n0 = (i << 1) & MASK;
          size_t n1 = (n0 + 1) & MASK;
          size_t n2 = (n0 + 2) & MASK;
          size_t n3 = (n0 + 3) & MASK;

          while (j + 3 < clen)
          {
               acc += *cp++ * in[n0];
               acc += *cp++ * in[n1];
               acc += *cp++ * in[n2];
               acc += *cp++ * in[n3];

               n0 = (n0 + 4) & MASK;
               n1 = (n1 + 4) & MASK;
               n2 = (n2 + 4) & MASK;
               n3 = (n3 + 4) & MASK;

               j += 4;
          }

          while (j++ < clen)
          {
               acc += *cp++ * in[n0];
               n0 = (n0 + 1) & MASK;
          }

          *out++ = acc * INV_SQRT2;
     }

     return 0;
}

int fwt_fir_dec_F(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     const size_t MASK = len - 1;
     const size_t M = len >> 1;

     for (size_t i = 0; i < M; ++i)
     {
          const float32_t* cp = c;
          float32_t acc = 0.0f;
          size_t j = 0;

          size_t n0 = (i << 1) & MASK;
          size_t n1 = (n0 + 1) & MASK;

          while (j + 1 < clen)
          {
               acc += *cp++ * in[n0];
               acc += *cp++ * in[n1];

               n0 = (n0 + 2) & MASK;
               n1 = (n1 + 2) & MASK;

               j += 2;
          }

          while (j++ < clen)
          {
               acc += *cp++ * in[n0];
               n0 = (n0 + 1) & MASK;
          }

          *out++ = acc * INV_SQRT2;
     }

     return 0;
}

int fwt_fir_dec_G(
  const float32_t* restrict in,
  float32_t* restrict out,
  const size_t len,
  const float32_t* restrict c,
  const size_t clen)
{
     const size_t M = len >> 1;

     for (size_t i = 0; i < M; ++i)
     {
          const float32_t* cp = c;
          float32_t acc = 0.0f;
          size_t j = 0;

          size_t n0 = (i << 1);
          size_t n1 = (n0 + 1);
          size_t n2 = (n0 + 2);
          size_t n3 = (n0 + 3);

          while (j + 3 < clen)
          {
               if (n0 >= len) n0 -= len;
               if (n1 >= len) n1 -= len;
               if (n2 >= len) n2 -= len;
               if (n3 >= len) n3 -= len;

               acc += *cp++ * in[n0];
               acc += *cp++ * in[n1];
               acc += *cp++ * in[n2];
               acc += *cp++ * in[n3];

               n0 += 4;
               n1 += 4;
               n2 += 4;
               n3 += 4;

               j += 4;
          }

          while (j++ < clen)
          {
               acc += *cp++ * in[n0++];

               if (n0 >= len) n0 -= len;
          }

          *out++ = acc * INV_SQRT2;
     }

     return 0;
}

int fwt_adc_scale(
  const uint32_t* in,
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
  const uint32_t* in,
  float32_t* out,
  const size_t N,
  const size_t NC,
  const float32_t scale)
{
     const uint32_t* start = in;

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
