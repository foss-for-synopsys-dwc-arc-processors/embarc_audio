/*
* Copyright 2019-2020, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include "cvsd.h"

#include "assert.h"
#ifdef _ARC
#include "arc/arc_intrinsics.h"
#endif
#ifdef __FXAPI__
#include "fxarc.h"
#else
#include "stdint.h"
#endif

#ifdef DEBUG_OUTPUT
#include "stdio.h"
#endif

const int PRECISION = 10;
const int BETA_EXP = 10;                 // Exponential of beta = 1024 = 2^10
const int ETA_EXP = 5;                   // Exponential of eta = 32 = 2^5
const int BIT_MASK = 0xF;                // Bit mask for J = 4 and K = 4
const int MAX_DELTA = 1280 << PRECISION;
const int MIN_DELTA = 10 << PRECISION;
const int POS_ACCUM_MAX = 32767 << PRECISION;
const int NEG_ACCUM_MAX = -POS_ACCUM_MAX;

short cvsdInit(cvsd_t *cvsd)
{
    cvsd->accumulator = 0;
    cvsd->step_size = MIN_DELTA;
    cvsd->output_byte = 0;
    return 0;
}

static inline void encode_bit(const int16_t *restrict *restrict in, register int32_t *restrict accum,
        register int32_t *restrict step_size, register uint32_t *restrict output_byte)
{
    if ((*(*in)++ << PRECISION) >= *accum)
    {
        *output_byte <<= 1;
        *accum += *step_size;
        *accum = _min(POS_ACCUM_MAX, *accum);
    }
    else
    {
        *output_byte = (*output_byte << 1) | 1;
        *accum -= *step_size;
        *accum = _max(NEG_ACCUM_MAX, *accum);
    }
    *accum -= (*accum >> ETA_EXP);
    uint32_t tmp = *output_byte & BIT_MASK;
    if ((tmp == BIT_MASK) || (tmp == 0))
    {
        *step_size += MIN_DELTA;
        *step_size = _min(MAX_DELTA, *step_size);
    }
    else
    {
        *step_size -= (*step_size >> BETA_EXP);
        *step_size = _max(MIN_DELTA, *step_size);
    }
}
static inline void swap_endian(uint32_t * const restrict out_pos, const uint32_t *restrict output_byte)
{
#ifdef __Xswap
    *out_pos = _swape(*output_byte);
#else
    int8_t *s_out = (int8_t *)out_pos;
    *s_out++ = (*output_byte >> 24) & 0xff;
    *s_out++ = (*output_byte >> 16) & 0xff;
    *s_out++ = (*output_byte >> 8) & 0xff;
    *s_out++ = *output_byte & 0xff;
#endif
}

void cvsdEncode(cvsd_t *cvsd,  const short *restrict in, register unsigned int input_frame_len,  unsigned int *restrict out)
{
    register const int double_word_len = 32;
    register const uint32_t double_word = (double_word_len - 1);
    register const int32_t rest_samples = input_frame_len & double_word;
    register const int32_t input_frame_len_multiplied_32 = input_frame_len / double_word_len;
    uint32_t output_byte = cvsd->output_byte;
    int32_t accum = cvsd->accumulator;
    int32_t step_size = cvsd->step_size;
    uint8_t *out_pos = (uint8_t *)out;
    //After interpolation minimum input_frame_len can not be less then 8 samples
    assert((input_frame_len % 8) == 0);

    for (int i = 0; i < input_frame_len_multiplied_32; i++)
    {
        for (int i = 0; i < double_word_len; i++)
        {
            encode_bit(&in, &accum, &step_size, &output_byte);
        }
        swap_endian((uint32_t *)out_pos, &output_byte);
        out_pos += sizeof(int32_t);
    }

    if (rest_samples)
    {
        for (int i = 0; i < rest_samples; i++)
        {
            encode_bit(&in, &accum, &step_size, &output_byte);
        }

        const uint32_t shifted_output_byte = output_byte << (double_word_len - rest_samples);
        swap_endian((uint32_t *)out_pos, &shifted_output_byte);
    }

    cvsd->output_byte = output_byte;
    cvsd->accumulator = accum;
    cvsd->step_size = step_size;
}

void cvsdDecode(cvsd_t *restrict cvsd,  const unsigned char *restrict in, register unsigned int input_frame_len,  short *restrict out)
{
    register uint32_t runner = cvsd->output_byte;
    register int32_t accum = cvsd->accumulator;
    register int32_t step_size = cvsd->step_size;
    for (int i = 0; i < input_frame_len; i++)
    {
#pragma clang loop unroll(full)
        for (int bit_counter = 128; bit_counter > 0; (bit_counter >>= 1))
        {
            *out++ = (accum >> PRECISION);
            if (in[i] & bit_counter)
            {
                runner = (runner << 1) | 1;
                accum -= step_size;
                accum = _max(NEG_ACCUM_MAX, accum);
            }
            else
            {
                runner <<= 1;
                accum += step_size;
                accum = _min(POS_ACCUM_MAX, accum);
            }
            accum -= (accum >> ETA_EXP);
            int32_t tmp = runner & BIT_MASK;
            if ((tmp == BIT_MASK) || (tmp == 0))
            {
                step_size += MIN_DELTA;
                step_size = _min(MAX_DELTA, step_size);
            }
            else
            {
                step_size -= (step_size >> BETA_EXP);
                step_size = _max(MIN_DELTA, step_size);
            }
        }
    }

    cvsd->accumulator = accum;
    cvsd->step_size = step_size;
    cvsd->output_byte = runner;
}
