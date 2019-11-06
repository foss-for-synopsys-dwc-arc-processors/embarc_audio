/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef _WRAPALLOC_H_
#define _WRAPALLOC_H_

#include <stdlib.h>
#ifdef NATIVE_CYCLE_PROFILING
#include <string.h>
#include "arc_profile.h"
#include "helper_lib.h"
#endif

void *wrapalloc(int size, int num);

void wrapfree(void*);
#endif