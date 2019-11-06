/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include "wrapalloc.h"

void *wrapalloc(int size, int num)
{
#ifdef NATIVE_CYCLE_PROFILING
    return memset(mem_alloc(size * num), 0, size * num);
#else
    return calloc(num, size);
#endif
}

void wrapfree(void* ptr)
{
#ifdef NATIVE_CYCLE_PROFILING
    mem_free(ptr);
#else
    free(ptr);
#endif
}