/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef __TEST_PROFILE_H__
#define __TEST_PROFILE_H__

#ifdef NATIVE_CYCLE_PROFILING
#include <stdint.h>
#include "testlib_types.h"

typedef Application_Settings_t Profiler_Settings_t;

void profile_init(Profiler_Settings_t *inst_settings, int * pargc, char * argv[]);
TEST_BOOL profile_frame_preprocess(Profiler_Settings_t * inst_settings);
TEST_BOOL profile_frame_postprocess(Profiler_Settings_t * inst_settings);
uint32_t profile_preprocess(Profiler_Settings_t * inst_settings);
void profile_postprocess(Profiler_Settings_t * inst_settings);
void profile_pause(Profiler_Settings_t * inst_settings);
void profile_resume(Profiler_Settings_t * inst_settings);

#ifdef NATIVE_BUSBANDWIDTH_PROFILING
#if (_ARCVER > 0x42)
#define MSS_PERFCTRL_MEM_IDX MSS_PERFCTRL_MEM_RGON0_IDX
#endif
#endif

#endif

#endif //__TEST_PROFILE_H__
