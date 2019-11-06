/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef _MEMORY_CONTROL_
#define _MEMORY_CONTROL_

#include "helper_lib.h"

#ifdef NATIVE_CYCLE_PROFILING
#define CALLOC(p_array, len) memset(mem_alloc((len)), 0, (len))
#define MALLOC(len) mem_alloc((len))
#define FREE(ptr) mem_free(ptr)
#else
#define CALLOC(p_array, len) calloc(1, (len))
#define MALLOC(len) malloc((len))
#define FREE(ptr) free(ptr)
#endif

#endif //_MEMORY_CONTROL_