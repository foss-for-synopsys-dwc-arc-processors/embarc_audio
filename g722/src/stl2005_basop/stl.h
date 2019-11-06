/* ITU G.722 3rd Edition (2012-09) */

/*
  ===========================================================================
   File: STL.H                                           v.2.2 - 06.Dec.2006
  ===========================================================================

            ITU-T STL  BASIC OPERATORS

            MAIN      HEADER      FILE

   History:
   07 Nov 04   v2.0     Incorporation of new 32-bit / 40-bit / control
                        operators for the ITU-T Standard Tool Library as 
                        described in Geneva, 20-30 January 2004 WP 3/16 Q10/16
                        TD 11 document and subsequent discussions on the
                        wp3audio@yahoogroups.com email reflector.
   March 06   v2.1      Changed to improve portability.                        

  ============================================================================
*/


#ifndef _STL_H
#define _STL_H

#include "patch.h"
#include "typedef.h"
#ifdef REMAPITU_T
#include <basop32.h>
#else
#include "basop32_.h"
#endif
#include "count.h"
#include "move.h"
#include "control.h"
#include "enh1632.h" 










#endif /* ifndef _STL_H */


/* end of file */
