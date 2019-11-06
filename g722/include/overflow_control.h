/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef overflow_control_h
#define overflow_control_h

#if defined(_ARC) && defined(REMAPITU_T)
#include "arc/arc_intrinsics.h"
// Overflow flag is placed in DSP_CTRL register 
// for this reason we should work with register instead of origin variable Overflow
static inline void reset_overflow_flag() {
   _sr(_lr(DSP_CTRL) & ~0x10000, DSP_CTRL);
}
static inline void set_overflow_flag(int ov_save) {
   _sr((_lr(DSP_CTRL) & ~0x10000) | (ov_save << 16), DSP_CTRL);
}
static inline int get_overflow_flag() {
   return (_lr(DSP_CTRL) & 0x10000);
}
#else
extern Flag Overflow;
static inline void reset_overflow_flag() {
   Overflow = 0;
}
static inline void set_overflow_flag(int ov_save) {
   Overflow = ov_save;
}
static inline int get_overflow_flag() {
   return Overflow;
}
#endif
#endif