/*
* Copyright 2019-present, Synopsys, Inc.
* All rights reserved.
*
* Synopsys modifications to this source code is licensed under
* the ITU-T SOFTWARE TOOLS' GENERAL PUBLIC LICENSE found in the
* LICENSE.md file in the root directory of this source tree.
*/
/* ITU G.722 3rd Edition (2012-09) */

/*
  ===========================================================================
   File: TYPEDEF.H                                       v.2.2 - 06.Dec.2006
  ===========================================================================

            ITU-T STL  BASIC OPERATORS

            TYPE DEFINITION PROTOTYPES

   History:
   26.Jan.00   v1.0     Incorporated to the STL from updated G.723.1/G.729 
                        basic operator library (based on basic_op.h)

   03 Nov 04   v2.0     Incorporation of new 32-bit / 40-bit / control
                        operators for the ITU-T Standard Tool Library as 
                        described in Geneva, 20-30 January 2004 WP 3/16 Q10/16
                        TD 11 document and subsequent discussions on the
                        wp3audio@yahoogroups.com email reflector.
   March 06   v2.1      Changed to improve portability.                        

  ============================================================================
*/


/*_____________________
 |                     |
 | Basic types.        |
 |_____________________|
*/


#ifndef _TYPEDEF_H
#define _TYPEDEF_H "$Id $"


/* #undef ORIGINAL_TYPEDEF_H */ /* Undefine to get the "new" version *
                                 * of typedef.h (see typedefs.h).    */

#define ORIGINAL_TYPEDEF_H      /* Define to get "original" version  *
                                 * of typedef.h (this file).         */



#ifdef ORIGINAL_TYPEDEF_H
/*
 * This is the original code from the file typedef.h
 */

#if defined(__BORLANDC__) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(__ZTC__)
typedef signed char Word8;
typedef short Word16;
typedef long Word32;
typedef __int64 Word40;
typedef unsigned short UWord16;
typedef unsigned long UWord32;
typedef int Flag;

#elif defined(__CYGWIN__)
typedef signed char Word8;
typedef short Word16;
typedef long Word32;
typedef long long Word40;
typedef unsigned short UWord16;
typedef unsigned long UWord32;
typedef int Flag;

#elif defined(__sun)
typedef signed char Word8;
typedef short Word16;
typedef long Word32;
/*#error "The 40-bit operations have not been tested on __sun : need to define Word40"*/
typedef long long Word40;
typedef unsigned short UWord16;
typedef unsigned long UWord32;
typedef int Flag;

#elif defined(__unix__) || defined(__unix)
typedef signed char Word8;
typedef short Word16;
typedef int Word32;
/*#error "The 40-bit operations have not been tested on unix : need to define Word40"*/
typedef long long Word40;
typedef unsigned short UWord16;
typedef unsigned int UWord32;
typedef int Flag;
#elif defined(_ARC) && defined(REMAPITU_T)
#include <stdint.h>
#include <basop32.h>
#elif defined(_ARC)
typedef char Word8;
typedef short  Word16;
typedef long  Word32;
typedef int   Flag;
typedef long long Word40;
typedef unsigned short UWord16;
typedef unsigned int UWord32;
#else
#endif


#else /* ifdef ORIGINAL_TYPEDEF_H */ /* not original typedef.h */




/*
 * Use (improved) type definition file typdefs.h and add a "Flag" type.
 */
#include "typedefs.h"
typedef int Flag;


#endif /* ifdef ORIGINAL_TYPEDEF_H */


#endif /* ifndef _TYPEDEF_H */


/* end of file */
