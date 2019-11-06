/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef _TEST_HELPER_LIB_H_
#define _TEST_HELPER_LIB_H_

#include <stdint.h>
#define mem_alloc mem_alloc_1_0
#define mem_free mem_free_1_0


// get CACHE configuration from TCF file and
// translate it into TEST defines
#undef CACHE_LINE_SIZE
#undef DCACHE_DISABLED
#if defined __core_config_h && (core_config_dcache_present==1)
#define CACHE_LINE_SIZE core_config_dcache_line_size
#else
#define DCACHE_DISABLED
#endif

/* Function names replacement section - required 
   to support several Helper Libraries linkage into single binary */
uint32_t check_dcache_dirty(void *buffer, uint32_t size);

/* used to invalidate a block of cache at the given address.                */
void cache_invalidate(void* address,
                      uint32_t size);

/* used to flush a block of cache at the given address.                     */
void cache_flush(void* address,
                 uint32_t size);

/* used to flush and invalidate a block of cache at the given address.      */
void cache_flush_invalidate(void* address,
                            uint32_t size);

/* set context for statistics                                               */
void mem_set_context(void *app_context);

/* used by codec to dynamically allocate memory                             */
void*  mem_alloc(uint32_t size);

/* used by codec to free allocated memory                                   */
void mem_free(void *ptr);

/* debug messages printing support                                          */
void debug_print(const char *format, ...);

#endif //_TEST_HELPER_LIB_H_
