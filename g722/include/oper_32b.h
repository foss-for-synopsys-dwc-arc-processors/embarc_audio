/* ITU G.722 3rd Edition (2012-09) */

/* ITU-T G722 PLC Appendix IV - Reference C code for fixed-point implementation */
/* Version:       1.2                                                           */
/* Revision Date: Jul 3, 2007                                                   */

/*
   ITU-T G.722 PLC Appendix IV   ANSI-C Source Code
   Copyright (c) 2006-2007
   France Telecom
*/

#ifndef __G729EV_MAIN_OPER_32B_H__
#define __G729EV_MAIN_OPER_32B_H__

#include "stl.h"

/* Double precision operations */
Word32 Div_32 (Word32 L_num, Word16 denom_hi, Word16 denom_lo);
#ifdef REMAPITU_T
/*****************************************************************************
 *                                                                           *
 *  Function L_Extract()                                                     *
 *                                                                           *
 *  Extract from a 32 bit integer two 16 bit DPF.                            *
 *                                                                           *
 *  Arguments:                                                               *
 *                                                                           *
 *   L_32      : 32 bit integer.                                             *
 *               0x8000 0000 <= L_32 <= 0x7fff ffff.                         *
 *   hi        : b16 to b31 of L_32                                          *
 *   lo        : (L_32 - hi<<16)>>1                                          *
 *****************************************************************************
*/
static _Inline void L_Extract (Word32 L_32, Word16 *hi, Word16 *lo) {
    *hi = extract_h (L_32);
    *lo = extract_l ((L_32 & 0xFFFF) >> 1);
}
/*****************************************************************************
 *                                                                           *
 *  Function L_Comp()                                                        *
 *                                                                           *
 *  Compose from two 16 bit DPF a 32 bit integer.                            *
 *                                                                           *
 *     L_32 = hi<<16 + lo<<1                                                 *
 *                                                                           *
 *  Arguments:                                                               *
 *                                                                           *
 *   hi        msb                                                           *
 *   lo        lsf (with sign)                                               *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *             32 bit long signed integer (Word32) whose value falls in the  *
 *             range : 0x8000 0000 <= L_32 <= 0x7fff fff0.                   *
 *                                                                           *
 *****************************************************************************
*/
static _Inline Word32 L_Comp (Word16 hi, Word16 lo)
{
    return ((Word32)hi << 16) + ((Word32)lo << 1);
}
/*****************************************************************************
 * Function Mpy_32()                                                         *
 *                                                                           *
 *   Multiply two 32 bit integers (DPF). The result is divided by 2**31      *
 *                                                                           *
 *   L_32 = (hi1*hi2)<<1 + ( (hi1*lo2)>>15 + (lo1*hi2)>>15 )<<1              *
 *                                                                           *
 *   This operation can also be viewed as the multiplication of two Q31      *
 *   number and the result is also in Q31.                                   *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi1         hi part of first number                                      *
 *  lo1         lo part of first number                                      *
 *  hi2         hi part of second number                                     *
 *  lo2         lo part of second number                                     *
 *                                                                           *
 *****************************************************************************
*/

static _Inline Word32 Mpy_32 (Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2)
{
#ifdef REMAPITU_T
    accum32_t acc = fx_a32_mpy_q15(hi1, hi2);
    acc = fx_a32_mac_q15(acc, mult(hi1, lo2), 1);
    acc = fx_a32_mac_q15(acc, mult(lo1, hi2), 1);
    return fx_q31_cast_a32(acc);
#else
    Word32 L_32;

    L_32 = L_mult (hi1, hi2);
    L_32 = L_mac (L_32, mult (hi1, lo2), 1);
    L_32 = L_mac (L_32, mult (lo1, hi2), 1);

    return (L_32);
#endif
}

/*****************************************************************************
 * Function Mpy_32_16()                                                      *
 *                                                                           *
 *   Multiply a 16 bit integer by a 32 bit (DPF). The result is divided      *
 *   by 2**15                                                                *
 *                                                                           *
 *                                                                           *
 *   L_32 = (hi1*lo2)<<1 + ((lo1*lo2)>>15)<<1                                *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi          hi part of 32 bit number.                                    *
 *  lo          lo part of 32 bit number.                                    *
 *  n           16 bit number.                                               *
 *                                                                           *
 *****************************************************************************
*/
static _Inline Word32 Mpy_32_16 (Word16 hi, Word16 lo, Word16 n)
{
    return L_mult(hi, n) + (mult(lo, n) << 1);
}
#else
void      L_Extract(Word32 L_32, Word16 * hi, Word16 * lo);
Word32    L_Comp(Word16 hi, Word16 lo);
Word32    Mpy_32(Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);
Word32    Mpy_32_16(Word16 hi, Word16 lo, Word16 n);
#endif

#endif /* __G729EV_MAIN_OPER_32B_H__ */
