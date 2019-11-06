/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include "cvsd.h"
#ifdef _ARC
#include "arc/arc_intrinsics.h"
#ifdef __FXAPI__
#include "fxarc.h"
#endif
#endif

#ifdef DEBUG_OUTPUT
#include "stdio.h"
#endif

#define PRECISION 10
#define MAX_DELTA 0x140000          // 1280 << 10 (15q10)
#define MIN_DELTA 0x2800            // 10 << 10 (15q10)
#define BETA_EXP 10                 // Exponential of beta = 1024 = 2^10
#define ETA_EXP 5                   // Exponential of eta = 32 = 2^5
#define NEG_ACCUM_MAX -0x1FFFC00    // -32767 << 10 (15q10)
#define POS_ACCUM_MAX 0x1FFFC00     // 32767 << 10 (15q10)
#define BIT_MASK 0xF                // Bit mask for J = 4 and K = 4

short cvsdInit(cvsd_t *cvsd)
{
    cvsd->accumulator = 0;
    cvsd->step_size = MIN_DELTA;
    cvsd->output_byte = 0;
    return 0;
}

void cvsdEncode(cvsd_t *cvsd,  const short *restrict in, register unsigned int input_len,  unsigned int *restrict out)
{
    register unsigned int output_byte = cvsd->output_byte;
    register int local_accum = cvsd->accumulator;
    register int local_step_size = cvsd->step_size;

    for (int i = 0; i < input_len; i += 32)
    {
        for (int bit_counter = 0; bit_counter < 32; bit_counter++)
        {
            if ((*in++ << PRECISION) >= local_accum)
            {
                output_byte <<= 1;
#ifdef __FXAPI__
                local_accum = fx_add_q31(local_accum, local_step_size);
#else
                local_accum += local_step_size;
                local_accum = _max(NEG_ACCUM_MAX, local_accum);
#endif
            }
            else
            {
                output_byte = (output_byte << 1) | 1;
#ifdef __FXAPI__
                local_accum = fx_sub_q31(local_accum, local_step_size);
#else
                local_accum -= local_step_size;
                local_accum = _min(POS_ACCUM_MAX, local_accum);
#endif
            }
            local_accum -= (local_accum >> ETA_EXP);
            unsigned int tmp = output_byte & BIT_MASK;
            if ((tmp == BIT_MASK) || (tmp == 0))
            {
                local_step_size += MIN_DELTA;
                local_step_size = _min(MAX_DELTA, local_step_size);
            }
            else
            {
                local_step_size -= (local_step_size >> BETA_EXP);
                local_step_size = _max(MIN_DELTA, local_step_size);
            }
        }
#ifdef __Xswap
        *out = _swape(output_byte);
#else
        char *s_out = (char *)out;
        *s_out++ = (output_byte >> 24) & 0xff;
        *s_out++ = (output_byte >> 16) & 0xff;
        *s_out++ = (output_byte >> 8) & 0xff;
        *s_out++ = output_byte & 0xff;
#endif
        out++;
    }
    cvsd->output_byte = output_byte;
    cvsd->accumulator = local_accum;
    cvsd->step_size = local_step_size;
}

void cvsdDecode(cvsd_t *restrict cvsd,  const unsigned char *restrict in, register unsigned int input_len,  short *restrict out)
{
    register unsigned int runner = cvsd->output_byte;
    register int local_accum = cvsd->accumulator;
    register int local_step_s = cvsd->step_size;
    for (int i = 0; i < input_len; i++)
    {
#pragma clang loop unroll(full)
        for (unsigned int bit_counter = 128; bit_counter > 0; (bit_counter >>= 1))
        {
            *out++ = (local_accum >> PRECISION);
            if (in[i] & bit_counter)
            {
                runner = (runner << 1) | 1;
#ifdef __FXAPI__
                local_accum = fx_sub_q31(local_accum, local_step_s);
#else
                local_accum -= local_step_s;
                local_accum = _min(POS_ACCUM_MAX, local_accum);
#endif
            }
            else
            {
                runner <<= 1;
#ifdef __FXAPI__
                local_accum = fx_add_q31(local_accum, local_step_s);
#else
                local_accum += local_step_s;
                local_accum = _max(NEG_ACCUM_MAX, local_accum);
#endif
            }
            local_accum -= (local_accum >> ETA_EXP);
            int tmp = runner & BIT_MASK;
            if ((tmp == BIT_MASK) || (tmp == 0))
            {
                local_step_s += MIN_DELTA;
                local_step_s = _min(MAX_DELTA, local_step_s);
            }
            else
            {
                local_step_s -= (local_step_s >> BETA_EXP);
                local_step_s = _max(MIN_DELTA, local_step_s);
            }
        }
    }

    cvsd->accumulator = local_accum;
    cvsd->step_size = local_step_s;
    cvsd->output_byte = runner;
}
