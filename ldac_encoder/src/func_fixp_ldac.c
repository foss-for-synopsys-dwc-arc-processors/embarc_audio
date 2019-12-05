/*
* Copyright 2019-present, Synopsys, Inc.
* All rights reserved.
*
* Synopsys modifications to this source code is licensed under
* the Apache-2.0 license found in the LICENSE file in the
* root directory of this source tree.
*/
/*
 * Copyright (C) 2003 - 2016 Sony Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ldac.h"
#ifdef __FXAPI__
#include <fxarc.h>
#include <stdfix.h>
#endif

/*******************************************************************************
    Subfunction: Check Saturation
*******************************************************************************/
__inline static INT32 check_sature_ldac(
INT64 val)
{

    return (INT32)val;
}

/*******************************************************************************
    Shift and Round
*******************************************************************************/
DECLFUNC INT32 sftrnd_ldac(
INT32 in,
int shift)
{
#ifdef __FXAPI__
    return (int)fx_asr_rnd_q31((q31_t)(in), shift);
#else
    INT64 out;

    if (shift > 0) {
        out = ((INT64)in + ((INT64)1 << (shift-1))) >> shift;
    }
    else {
        out = (INT64)in << (-shift);
    }

    return check_sature_ldac(out);
#endif
}


/*******************************************************************************
    Get Bit Length of Value
*******************************************************************************/
DECLFUNC int get_bit_length_ldac(
INT32 val)
{
#ifdef __FXAPI__
    return 31 - fx_norm_q31((q31_t)(val));
#else
    int len;

    len = 0;
    while (val > 0) {
        val >>= 1;
        len++;
    }

    return len;
#endif
}

/*******************************************************************************
    Get Maximum Absolute Value
*******************************************************************************/
DECLFUNC INT32 get_absmax_ldac(
INT32 *p_x,
int num)
{
    int i;
    INT32 abmax, val;

    abmax = abs(p_x[0]);
    for (i = 1; i < num; i++) {
        val = abs(p_x[i]);
        if (abmax < val) {
            abmax = val;
        }
    }

    return abmax;
}

