/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef _TEST_ASSERT_H_
#define _TEST_ASSERT_H_

#ifdef _ARC
#ifndef TEST_ASSERT_DISABLED
#define TEST_ASSERT(cond)   { if(!(cond)) { DBG("TEST ASSERTION FAILED on %s:%d: %s\n", __FILE__, __LINE__, #cond); _brk(); } }
#else
#define TEST_ASSERT(cond)
#endif
#else
#include <assert.h>
#define TEST_ASSERT assert
#endif
#endif //TEST_ASSERT_H_
