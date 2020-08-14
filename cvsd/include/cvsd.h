/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef __CVSD_H__
#define __CVSD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#pragma push_align_members(4)
    typedef struct cvsd_s
    {
        int accumulator;
        int step_size;
        unsigned int output_byte;
    } cvsd_t;
#pragma pop_align_members()

    short cvsdInit(cvsd_t *cvsd);
    void cvsdEncode(cvsd_t *restrict cvsd,  const short *restrict in, unsigned int input_len,  unsigned int * restrict out);
    void cvsdDecode(cvsd_t *restrict cvsd,  const unsigned char *restrict in, unsigned int input_len,  short *restrict out);

#ifdef __cplusplus
}
#endif

#endif // __CVSD_H__
