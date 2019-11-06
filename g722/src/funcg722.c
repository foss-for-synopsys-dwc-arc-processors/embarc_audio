/* ITU G.722 3rd Edition (2012-09) */

/*                     v3.0 beta - 23/Aug/2006
  ============================================================================

                          U    U   GGG    SSSS  TTTTT
                          U    U  G       S       T
                          U    U  G  GG   SSSS    T
                          U    U  G   G       S   T
                           UUU     GG     SSS     T

                   ========================================
                    ITU-T - USER'S GROUP ON SOFTWARE TOOLS
                   ========================================


       =============================================================
       COPYRIGHT NOTE: This source code, and all of its derivations,
       is subject to the "ITU-T General Public License". Please have
       it  read  in    the  distribution  disk,   or  in  the  ITU-T
       Recommendation G.191 on "SOFTWARE TOOLS FOR SPEECH AND  AUDIO
       CODING STANDARDS". 
       ** This code has  (C) Copyright by CNET Lannion A TSS/CMC **
       =============================================================


MODULE:         G722.C 7kHz ADPCM AT 64 KBIT/S MODULE ENCODER AND 
                DECODER FUNCTIONS

ORIGINAL BY:
    J-P PETIT 
    CNET - Centre Lannion A
    LAA-TSS                         Tel: +33-96-05-39-41
    Route de Tregastel - BP 40      Fax: +33-96-05-13-16
    F-22301 Lannion CEDEX           Email: petitjp@lannion.cnet.fr
    FRANCE
    
   History:
History:
~~~~~~~~
14.Mar.95  v1.0       Released for use ITU-T UGST software package Tool
                      based on the CNET's 07/01/90 version 2.00
01.Jul.95  v2.0       Changed function declarations to work with many compilers;
                      reformated <simao@ctd.comsat.com>
23.Aug.06  v3.0 beta  Updated with STL2005 v2.2 basic operators and G.729.1 methodology
                      <{balazs.kovesi,stephane.ragot}@orange-ft.com>
  ============================================================================
*/

#include <stdio.h>

/* Include state variable definition, function and operator prototypes */
#include "g722.h"
#ifdef REMAPITU_T
#define shift_l_x3(x) (x << 3)
#include "basop32_.h"
#else
#define shift_l_x3(x) (x) //source code assume shl by 3 during programm run
#endif
/*___________________________________________________________________________
                                                                            
    Function Name : lsbcod                                                  
                                                                            
    Purpose :                                                               
                                                                            
     Encode lower subband of incomung speech/music.                         
                                                                            
    Inputs :                                                                
      xh - Low sub-band portion of the input sample                         
      rs - reset flag (read-only)                                           
      s  - pointer to state variable (read/write)                           
                                                                            
    Return Value :                                                          
     Encoded sample as a 16-bit word                                        
 ___________________________________________________________________________
*/
#define AL   s->al
#define BL   s->bl
#define DETL s->detl
#define DLT  s->dlt
#define NBL  s->nbl
#define PLT  s->plt
#define RLT  s->rlt
#define SL   s->sl
#define SPL  s->spl
#define SZL  s->szl

Word16 
lsbcod (Word16 xl, Word16 rs, g722_state *s)
{
  Word16          el, nbpl, il;

/* Complexity optimization : note that the IF part could be moved to a separate reset function */
#ifdef REMAPITU_T
  IF (rs == 1)
#else
  IF (sub(rs, 1) == 0)
#endif
  {
    DETL = 32;
    SL = SPL = SZL = NBL = 0;
    AL[1] = AL[2] = 0;
    BL[1] = BL[2] = BL[3] = BL[4] = BL[5] = BL[6] = 0;
    DLT[0] = DLT[1] = DLT[2] = DLT[3] = DLT[4] = DLT[5] = DLT[6] = 0;
    PLT[0] = PLT[1] = PLT[2] = 0;
    RLT[0] = RLT[1] = RLT[2] = 0;
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif
    return (0);
  }

  el = sub (xl, SL);           /* subtra */
  il = quantl (el, DETL);
  DLT[0] = invqal (il, DETL);
  nbpl = logscl (il, NBL);
  NBL = nbpl;
  DETL = scalel (nbpl);
  PLT[0] = add (DLT[0], SZL);   /* parrec */
  RLT[0] = add (SL, DLT[0]);    /* recons */
  upzero (DLT, BL);
  uppol2 (AL, PLT);
  uppol1 (AL, PLT);
  SZL = filtez (DLT, BL);
  SPL = filtep (RLT, AL);
  SL = add (SPL, SZL);          /* predic */
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif

  /* Return encoded sample */
  return (il);
}
#undef AL
#undef BL
#undef DETL
#undef DLT
#undef NBL
#undef PLT
#undef RLT
#undef SL
#undef SPL
#undef SZL
/* ........................ End of lsbcod() ........................ */


/*___________________________________________________________________________
                                                                            
    Function Name : hsbcod                                                  
                                                                            
    Purpose :                                                               
                                                                            
     Encode upper sub-band of incoming speech or music.                     
                                                                            
    Inputs :                                                                
      xh - High sub-band portion of the input sample                        
      rs - reset flag (read-only)                                           
      s  - pointer to state variable (read/write)                           
                                                                            
    Return Value :                                                          
     ADPCM code for the high-band portion of the input sample as a          
     16-bit word.                                                           
                                                                            
 ___________________________________________________________________________
*/
#define AH   s->ah
#define BH   s->bh
#define DETH s->deth
#define DH  s->dh 
#define NBH  s->nbh
#define PH  s->ph 
#define RH  s->rh 
#define SH   s->sh
#define SPH  s->sph 
#define SZH  s->szh

Word16 hsbcod (Word16 xh, Word16 rs, g722_state *s)
{
  Word16          eh, nbph, ih;

/* Complexity optimization : note that the IF part could be moved to a separate reset function */
#ifdef REMAPITU_T
  IF (rs == 1)
#else
  IF (sub(rs, 1) == 0)
#endif
  {
    DETH = 8;
    SH = SPH = SZH = NBH = 0;
    AH[1] = AH[2] = 0;
    BH[1] = BH[2] = BH[3] = BH[4] = BH[5] = BH[6] = 0;
    DH[0] = DH[1] = DH[2] = DH[3] = DH[4] = DH[5] = DH[6] = 0;
    PH[0] = PH[1] = PH[2] = 0;
    RH[0] = RH[1] = RH[2] = 0;
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif

    return (0);
  }

  eh = sub (xh, SH);         /* subtra */
  ih = quanth (eh, DETH);
  DH[0] = invqah (ih, DETH);
  nbph = logsch (ih, NBH);
  NBH = nbph;
  DETH = scaleh (nbph);
  PH[0] = add (DH[0], SZH);   /* parrec */
  RH[0] = add (SH, DH[0]);    /* recons */
  upzero (DH, BH);
  uppol2 (AH, PH);
  uppol1 (AH, PH);
  SZH = filtez (DH, BH);
  SPH = filtep (RH, AH);
  SH = add (SPH, SZH);        /* predic */
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif

  return (ih);

}
#undef AH
#undef BH
#undef DETH
#undef DH
#undef NBH
#undef PH
#undef RH
#undef SH
#undef SPH
#undef SZH
/* ........................ End of hsbcod() ........................ */



/*___________________________________________________________________________
                                                                            
    Function Name : lsbdec                                                  
                                                                            
    Purpose :                                                               
                                                                            
     Decode lower subband of incomung speech/music.                         
                                                                            
    Inputs :                                                                
      ilr - ADPCM encoding of the low sub-band                              
      mode - G.722 operation mode                                           
      rs  - reset flag (read-only)                                          
      s   - pointer to state variable (read/write)                          
                                                                            
    Return Value :                                                          
     Decoded low-band portion of the recovered sample as a 16-bit word      
 ___________________________________________________________________________
*/
#define AL   s->al
#define BL   s->bl
#define DETL s->detl
#define DLT  s->dlt
#define NBL  s->nbl
#define PLT  s->plt
#define RLT  s->rlt
#define SL   s->sl
#define SPL  s->spl
#define SZL  s->szl

Word16 lsbdec (Word16 ilr, Word16 mode, Word16 rs, g722_state *s)
{
  Word16          dl, rl, nbpl, yl;

/* Complexity optimization : note that the IF part could be moved to a separate reset function */
  IF (sub(rs,1) == 0)
  {
    DETL = 32;
    SL = SPL = SZL = NBL = 0;
    AL[1] = AL[2] = 0;
    BL[1] = BL[2] = BL[3] = BL[4] = BL[5] = BL[6] = 0;
    DLT[0] = DLT[1] = DLT[2] = DLT[3] = DLT[4] = DLT[5] = DLT[6] = 0;
    PLT[0] = PLT[1] = PLT[2] = 0;
    RLT[0] = RLT[1] = RLT[2] = 0;
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif

    return (0);
  }

  dl = invqbl (ilr, DETL, mode);
  rl = add (SL, dl);              /* recons */
  yl = limit (rl);
  DLT[0] = invqal (ilr, DETL);
  nbpl = logscl (ilr, NBL);
  NBL = nbpl;
  DETL = scalel (nbpl);
  PLT[0] = add (DLT[0], SZL);     /* parrec */
  RLT[0] = add (SL, DLT[0]);      /* recons */
  upzero (DLT, BL);
  uppol2 (AL, PLT);
  uppol1 (AL, PLT);
  SZL = filtez (DLT, BL);
  SPL = filtep (RLT, AL);
  SL = add (SPL, SZL);            /* predic */
#ifdef WMOPS /*for structures and arrays only*/
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif

  return (yl);

}
#undef AL
#undef BL
#undef DETL
#undef DLT
#undef NBL
#undef PLT
#undef RLT
#undef SL
#undef SPL
#undef SZL
/* ........................ End of lsbdec() ........................ */


/*___________________________________________________________________________
                                                                            
    Function Name : hsbdec                                                  
                                                                            
    Purpose :                                                               
                                                                            
     Decode lower subband of incomung speech/music.                         
                                                                            
    Inputs :                                                                
      ih  - ADPCM encoding of the high sub-band                             
      rs  - reset flag (read-only)                                          
      s   - pointer to state variable (read/write)                          
                                                                            
    Return Value :                                                          
     Decoded high-band portion of the recovered sample as a 16-bit word     
 ___________________________________________________________________________
*/
#define AH   s->ah
#define BH   s->bh
#define DETH s->deth
#define DH  s->dh 
#define NBH  s->nbh
#define PH  s->ph 
#define RH  s->rh 
#define SH   s->sh
#define SPH  s->sph 
#define SZH  s->szh

Word16 hsbdec (Word16 ih, Word16 rs, g722_state *s)
{
  Word16          nbph, yh;

/* Complexity optimization : note that the IF part could be moved to a separate reset function */
#ifdef REMAPITU_T
  IF (rs == 1)
#else
  IF (sub(rs, 1) == 0)
#endif
  {
    DETH = 8;
    SH = SPH = SZH = NBH = 0;
    AH[1] = AH[2] = 0;
    BH[1] = BH[2] = BH[3] = BH[4] = BH[5] = BH[6] = 0;
    DH[0] = DH[1] = DH[2] = DH[3] = DH[4] = DH[5] = DH[6] = 0;
    PH[0] = PH[1] = PH[2] = 0;
    RH[0] = RH[1] = RH[2] = 0;
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif
    return (0);
  }

  DH[0] = invqah (ih, DETH);
  nbph = logsch (ih, NBH);
  NBH = nbph;
  DETH = scaleh (nbph);
  PH[0] = add (DH[0], SZH);    /* add */
  RH[0] = add (SH, DH[0]);     /* recons */
  upzero (DH, BH);
  uppol2 (AH, PH);
  uppol1 (AH, PH);
  SZH = filtez (DH, BH);
  SPH = filtep (RH, AH);
  SH = add (SPH, SZH);         /* predic */
  yh = limit (RH[0]);
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
    move16();
#endif

  return (yh);
}
#undef AH
#undef BH
#undef DETH
#undef DH
#undef NBH
#undef PH
#undef RH
#undef SH
#undef SPH
#undef SZH
/* ........................ End of hsbdec() ........................ */


/*___________________________________________________________________________
                                                                            
    Function Name : quantl                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 
quantl (Word16 el, Word16 detl)
{
 /* table to read IL frorm SIL and MIL: misil(sil(0,1),mil(1,31)) */

  static const Word16   misil[2][32] =
  {
    {0x0000, 0x003F, 0x003E, 0x001F, 0x001E, 0x001D, 0x001C, 0x001B,
     0x001A, 0x0019, 0x0018, 0x0017, 0x0016, 0x0015, 0x0014, 0x0013,
     0x0012, 0x0011, 0x0010, 0x000F, 0x000E, 0x000D, 0x000C, 0x000B,
     0x000A, 0x0009, 0x0008, 0x0007, 0x0006, 0x0005, 0x0004, 0x0000},
    {0x0000, 0x003D, 0x003C, 0x003B, 0x003A, 0x0039, 0x0038, 0x0037,
     0x0036, 0x0035, 0x0034, 0x0033, 0x0032, 0x0031, 0x0030, 0x002F,
     0x002E, 0x002D, 0x002C, 0x002B, 0x002A, 0x0029, 0x0028, 0x0027,
     0x0026, 0x0025, 0x0024, 0x0023, 0x0022, 0x0021, 0x0020, 0x0000}
  };

 /* 6 levels quantizer level od decision */
#if defined(REMAPITU_T) && defined(core_config_xy_x_base) && defined(USE_XCCM)
#pragma data(".Xdata")
#endif
  static const Word16 q6[31] = {
      shift_l_x3(0),    shift_l_x3(35),   shift_l_x3(72),   shift_l_x3(110),
      shift_l_x3(150),  shift_l_x3(190),  shift_l_x3(233),  shift_l_x3(276),
      shift_l_x3(323),  shift_l_x3(370),  shift_l_x3(422),  shift_l_x3(473),
      shift_l_x3(530),  shift_l_x3(587),  shift_l_x3(650),  shift_l_x3(714),
      shift_l_x3(786),  shift_l_x3(858),  shift_l_x3(940),  shift_l_x3(1023),
      shift_l_x3(1121), shift_l_x3(1219), shift_l_x3(1339), shift_l_x3(1458),
      shift_l_x3(1612), shift_l_x3(1765), shift_l_x3(1980), shift_l_x3(2195),
      shift_l_x3(2557), shift_l_x3(2919), shift_l_x3(3200)
  };

  Word16          sil,  wd, val;
  Word32 mil;

  sil = shr (el, 15);
  wd = sub (MAX_16, s_and(el, MAX_16));
  if(sil == 0)
  {
    wd = el;
#ifdef WMOPS
    move16();
#endif
  }

  mil = 0;
#ifdef WMOPS
    move16();
#endif
#ifdef REMAPITU_T
  S_xy Word16* pq6 = (S_xy Word16*)q6;
  val = mult (*pq6++, detl);
  for(mil = 1; ((val <= wd) && (mil < 31)); mil++)
#else
  val = mult (shl(q6[mil], 3), detl);
#pragma clang loop unroll(disable)
  WHILE (sub(val,wd) <= 0)
#endif
  {
#ifdef REMAPITU_T
    val = mult (q6[mil], detl);
#else
    IF (sub(mil, 30) == 0)
    {
      BREAK;
    }
    ELSE
    {
      mil = add(mil,1);
      val = mult (shl(q6[mil],3), detl);
    }
#endif
  }
#ifdef REMAPITU_T
  mil--;
  sil++;
#else
  sil = add(sil,1);
#endif
#ifdef WMOPS
    move16(); /*for the 2 dimensional array addressing */
#endif
  return (misil[sil][mil]);
}
/* ..................... End of quantl() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name quanth:                                                   
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 quanth (Word16 eh, Word16 deth)
{
  static const Word16   misih[2][3] =
  {
    {0, 1, 0},
    {0, 3, 2}
  };
#ifdef REMAPITU_T
  static const Word16   q2 = 564 << 3;
#else
  static const Word16   q2 = 564;
#endif
  Word16          sih, mih, wd;

  sih = shr (eh, 15);
  wd = sub (MAX_16, s_and(eh, MAX_16));

  if(sih == 0)
  {
    wd = eh;
#ifdef WMOPS
    move16();
#endif
  }
  mih = 1;
#ifdef WMOPS
  move16();
#endif
#ifdef REMAPITU_T
  if (wd >= mult (q2, deth))
#else
  if (sub(wd, mult (shl (q2, 3), deth)) >= 0)
#endif
  {
    mih = 2;
#ifdef WMOPS
    move16();
#endif
  }

  sih = add(sih, 1);
#ifdef WMOPS
    move16(); /*for the 2 dimensional array addressing */
#endif
  return (misih[sih][mih]);
}
/* ..................... End of quanth() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : filtep                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 filtep (Word16 *rlt, Word16  *al)
{

  Word16          wd1, wd2, spl;

  /* shift of rlt */
  rlt[2] = rlt[1];
  rlt[1] = rlt[0];
#ifdef WMOPS
    move16();
    move16();
#endif

  wd1 = add (rlt[1], rlt[1]);
  wd1 = mult (al[1], wd1);
  wd2 = add (rlt[2], rlt[2]);
  wd2 = mult (al[2], wd2);
  spl = add (wd1, wd2);

  return (spl);
}
/* ..................... End of filtep() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : filtez                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 filtez (Word16 dlt[], Word16 bl[])
{
  Word16          szl, wd;
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i;
#endif

  szl = 0;
#ifdef WMOPS
    move16();
#endif
  S_xy Word16 *pbl = (S_xy Word16 *)&bl[6];
  FOR (i = 6; i > 0; i--)
  {
    wd = add (dlt[i], dlt[i]);
    wd = mult (wd, *pbl--);
    szl = add (szl, wd);
  }
  return (szl);
}
/* ..................... End of filtez() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : invqal                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 invqal (Word16 il, Word16 detl)
{

  static const Word16   risil[16] =
  {0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
  /* inverse quantizer 4 bits for encoder or decoder */
  static const Word16   ril4[16] =
  {0, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2, 1, 0};

  static const Word16 oq4[8] = {
      shift_l_x3(0),   shift_l_x3(150),  shift_l_x3(323),  shift_l_x3(530),
      shift_l_x3(786), shift_l_x3(1121), shift_l_x3(1612), shift_l_x3(2557),
  };

  /* Local variables */
  Word16          ril, wd1, wd2;


  ril = shr (il, 2);
#ifdef REMAPITU_T
  wd1 = oq4[ril4[ril]];
#else
  wd1 = shl (oq4[ril4[ril]], 3);
#endif
#ifdef WMOPS
    move16(); /*for the 2 dimensional array addressing */
#endif
  wd2 = negate (wd1);
#ifdef WMOPS
    move16(); /*for the test on array variable*/
#endif
  if(risil[ril] == 0)
  {
    wd2 = wd1;
#ifdef WMOPS
    move16();
#endif
  }
  
    return (mult (detl, wd2));
}
/* ..................... End of invqal() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : invqbl                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 invqbl (Word16 ilr, Word16 detl, Word16 mode)
{

  /* Inverse quantizer 4, 5, and 6 bit tables for the decoder */
  static const Word16   ril4[16] = {0, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2, 1, 0};
  static const Word16   risi4[16] = {0, -1, -1, -1, -1, -1, -1, -1, 
                               0, 0, 0, 0, 0, 0, 0, 0};
  static const Word16   ril5[32] = {
    1, 1, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2,
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 1};

  static const Word16   risi5[32] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1};
  static const Word16   ril6[64] = {
    1, 1, 1, 1, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,
    30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 1};

  static const Word16   risi6[64] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1};
static const Word16 oq4[8] = {shift_l_x3(0),    shift_l_x3(150), shift_l_x3(323),
                        shift_l_x3(530),  shift_l_x3(786), shift_l_x3(1121),
                        shift_l_x3(1612), shift_l_x3(2557)};
static const Word16 oq5[16] = {
    shift_l_x3(0),    shift_l_x3(35),   shift_l_x3(110),  shift_l_x3(190),
    shift_l_x3(276),  shift_l_x3(370),  shift_l_x3(473),  shift_l_x3(587),
    shift_l_x3(714),  shift_l_x3(858),  shift_l_x3(1023), shift_l_x3(1219),
    shift_l_x3(1458), shift_l_x3(1765), shift_l_x3(2195), shift_l_x3(2919)};
static const Word16 oq6[31] = {
    shift_l_x3(0),    shift_l_x3(17),   shift_l_x3(54),   shift_l_x3(91),
    shift_l_x3(130),  shift_l_x3(170),  shift_l_x3(211),  shift_l_x3(254),
    shift_l_x3(300),  shift_l_x3(347),  shift_l_x3(396),  shift_l_x3(447),
    shift_l_x3(501),  shift_l_x3(558),  shift_l_x3(618),  shift_l_x3(682),
    shift_l_x3(750),  shift_l_x3(822),  shift_l_x3(899),  shift_l_x3(982),
    shift_l_x3(1072), shift_l_x3(1170), shift_l_x3(1279), shift_l_x3(1399),
    shift_l_x3(1535), shift_l_x3(1689), shift_l_x3(1873), shift_l_x3(2088),
    shift_l_x3(2376), shift_l_x3(2738), shift_l_x3(3101)};

  /* Local variables */
  Word16          ril, wd1, wd2;

  SWITCH (mode)
  {
  case 0:
  case 1:
    ril = ilr;
#ifdef WMOPS
    move16();
#endif
#ifdef REMAPITU_T
    wd1 = oq6[ril6[ril]];
#else
    wd1 = shl (oq6[ril6[ril]], 3);
#endif
#ifdef WMOPS
    move16(); /*for double array indexing*/
#endif
#ifdef REMAPITU_T
    if(risi6[ril] != 0)
    {
      wd2 = negate(wd1);
    } else {
      wd2 = wd1;
    }
#else
    wd2 = sub (0, wd1);
#ifdef WMOPS
    move16(); /*for the test on array variable*/
#endif
    if(risi6[ril] == 0)
    {
      wd2 = add (0, wd1);
    }
#endif
    BREAK;
  case 2:
    ril = shr (ilr, 1);
#ifdef REMAPITU_T
    wd1 = oq5[ril5[ril]];
#else
    wd1 = shl (oq5[ril5[ril]], 3);
#endif
#ifdef WMOPS
    move16(); /*for double array indexing*/
#endif
#ifdef REMAPITU_T
    if(risi5[ril] != 0)
    {
      wd2 = negate(wd1);
    } else {
      wd2 = wd1;
    }
#else
    wd2 = sub (0, wd1);
#ifdef WMOPS
    move16(); /*for the test on array variable*/
#endif
    if(risi5[ril] == 0)
    {
      wd2 = add (0, wd1);
    }
#endif
    BREAK;
  case 3:
  default:
    /* default to mode 3 */
    ril = shr (ilr, 2);
#ifdef REMAPITU_T
    wd1 = oq4[ril4[ril]];
#else
    wd1 = shl (oq4[ril4[ril]], 3);
#endif
#ifdef WMOPS
    move16(); /*for double array indexing*/
#endif
#ifdef REMAPITU_T
    if(risi4[ril] != 0)
    {
      wd2 = negate(wd1);
    } else {
      wd2 = wd1;
    }
#else
    wd2 = sub (0, wd1);
#ifdef WMOPS
    move16(); /*for the test on array variable*/
#endif
    if(risi4[ril] == 0)
    {
      wd2 = add (0, wd1);
    }
#endif
    BREAK;
  }

  return (mult (detl, wd2));
}
/* ..................... End of invqbl() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : invqah                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 invqah (Word16 ih, Word16 deth)
{
#ifdef REMAPITU_T
  static const Word16   oq2[4] = {shift_l_x3(926), shift_l_x3(202), shift_l_x3(926), shift_l_x3(202)};
#else
  static const Word16   ih2[4] = {2, 1, 2, 1};
  static const Word16   sih[4] = {-1, -1, 0, 0};
  static const Word16   oq2[3] = {0, 202, 926};
#endif

  Word16          wd1;
#ifdef REMAPITU_T
  wd1 = oq2[ih];
#else
  wd1 = shl (oq2[ih2[ih]], 3);
#endif
#ifdef WMOPS
    move16(); /*for double array indexing*/
#endif

  if(ih < 2)
  {
    // wd2 = wd1;
    return mult (negate (wd1), deth);
#ifdef WMOPS
    move16();
#endif
  } else {
    return mult (wd1, deth);
#ifdef WMOPS
    move16(); /*for the test on array variable*/
#endif
  }


}
/* ..................... End of invqah() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : limit                                                   
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 limit (Word16 rl)
{
#ifdef REMAPITU_T
  return MAX(MIN(rl, 16383), -16384);
#else
  if(sub(rl, 16383) > 0)
  {
    rl = 16383;
#ifdef WMOPS
    move16();
#endif
  }
  
  if(sub(rl, -16384) < 0)
  {
    rl = -16384;
#ifdef WMOPS
    move16();
#endif
  }

  return (rl);
#endif
}
/* ..................... End of limit() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : logsch                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 logsch (Word16 ih, Word16 nbh)
{
#ifdef REMAPITU_T
  static const Word16   wh[4] =  {798, -214, 798, -214};
#else
  static const Word16   ih2[4] = {2, 1, 2, 1};
  static const Word16   wh[3] =  {0, -214, 798};
#endif
  Word16          wd, nbph;

  wd = mult (nbh, 32512);
#ifdef REMAPITU_T
  nbph = add (wd, wh[ih]);
#else
nbph = add (wd, wh[ih2[ih]]);
#endif
#ifdef WMOPS
    move16(); /*for double array indexing*/
#endif
#ifdef REMAPITU_T
  return MIN(MAX(nbph, 0), 22528);
#else
  if(nbph < 0)
  {
    nbph = 0;
#ifdef WMOPS
    move16();
#endif
  }
  if(sub(nbph, 22528) > 0)
  {
    nbph = 22528;
#ifdef WMOPS
    move16();
#endif
  }

  return (nbph);
#endif
}
/* ..................... End of logsch() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : logscl                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 logscl (Word16 il, Word16 nbl )
{
  static const Word16   ril4[16] = {0, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2, 1, 0};
  static const Word16   wl[8] =    {-60, -30, 58, 172, 334, 538, 1198, 3042};

  Word16          ril, wd, nbpl, il4;

  ril = shr (il, 2);
  wd = mult (nbl, 32512);
  il4 = ril4[ril];
  nbpl = add (wd, wl[il4]);
#ifdef REMAPITU_T
  return MIN(MAX(nbpl, 0), 18432);
#else
  if(nbpl < 0)
  {
    nbpl = 0;
#ifdef WMOPS
    move16();
#endif
  }
  if(sub(nbpl, 18432) > 0)
  {
    nbpl = 18432;
#ifdef WMOPS
    move16();
#endif
  }

  return (nbpl);
#endif
}
/* ..................... End of logscl() ..................... */


/* ************** Table ILA used by scalel and scaleh ******************** */
#ifdef REMAPITU_T
#define add1_shl2(x) (x + 1) << 2
#else
#define add1_shl2(x) (x)
#endif
static const Word16 ila[353] = {
    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),
    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),
    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),
    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),
    add1_shl2(1),    add1_shl2(1),    add1_shl2(1),    add1_shl2(2),
    add1_shl2(2),    add1_shl2(2),    add1_shl2(2),    add1_shl2(2),
    add1_shl2(2),    add1_shl2(2),    add1_shl2(2),    add1_shl2(2),
    add1_shl2(2),    add1_shl2(2),    add1_shl2(2),    add1_shl2(2),
    add1_shl2(3),    add1_shl2(3),    add1_shl2(3),    add1_shl2(3),
    add1_shl2(3),    add1_shl2(3),    add1_shl2(3),    add1_shl2(3),
    add1_shl2(3),    add1_shl2(3),    add1_shl2(3),    add1_shl2(4),
    add1_shl2(4),    add1_shl2(4),    add1_shl2(4),    add1_shl2(4),
    add1_shl2(4),    add1_shl2(4),    add1_shl2(4),    add1_shl2(5),
    add1_shl2(5),    add1_shl2(5),    add1_shl2(5),    add1_shl2(5),
    add1_shl2(5),    add1_shl2(5),    add1_shl2(6),    add1_shl2(6),
    add1_shl2(6),    add1_shl2(6),    add1_shl2(6),    add1_shl2(6),
    add1_shl2(7),    add1_shl2(7),    add1_shl2(7),    add1_shl2(7),
    add1_shl2(7),    add1_shl2(7),    add1_shl2(8),    add1_shl2(8),
    add1_shl2(8),    add1_shl2(8),    add1_shl2(8),    add1_shl2(9),
    add1_shl2(9),    add1_shl2(9),    add1_shl2(9),    add1_shl2(10),
    add1_shl2(10),   add1_shl2(10),   add1_shl2(10),   add1_shl2(11),
    add1_shl2(11),   add1_shl2(11),   add1_shl2(11),   add1_shl2(12),
    add1_shl2(12),   add1_shl2(12),   add1_shl2(13),   add1_shl2(13),
    add1_shl2(13),   add1_shl2(13),   add1_shl2(14),   add1_shl2(14),
    add1_shl2(15),   add1_shl2(15),   add1_shl2(15),   add1_shl2(16),
    add1_shl2(16),   add1_shl2(16),   add1_shl2(17),   add1_shl2(17),
    add1_shl2(18),   add1_shl2(18),   add1_shl2(18),   add1_shl2(19),
    add1_shl2(19),   add1_shl2(20),   add1_shl2(20),   add1_shl2(21),
    add1_shl2(21),   add1_shl2(22),   add1_shl2(22),   add1_shl2(23),
    add1_shl2(23),   add1_shl2(24),   add1_shl2(24),   add1_shl2(25),
    add1_shl2(25),   add1_shl2(26),   add1_shl2(27),   add1_shl2(27),
    add1_shl2(28),   add1_shl2(28),   add1_shl2(29),   add1_shl2(30),
    add1_shl2(31),   add1_shl2(31),   add1_shl2(32),   add1_shl2(33),
    add1_shl2(33),   add1_shl2(34),   add1_shl2(35),   add1_shl2(36),
    add1_shl2(37),   add1_shl2(37),   add1_shl2(38),   add1_shl2(39),
    add1_shl2(40),   add1_shl2(41),   add1_shl2(42),   add1_shl2(43),
    add1_shl2(44),   add1_shl2(45),   add1_shl2(46),   add1_shl2(47),
    add1_shl2(48),   add1_shl2(49),   add1_shl2(50),   add1_shl2(51),
    add1_shl2(52),   add1_shl2(54),   add1_shl2(55),   add1_shl2(56),
    add1_shl2(57),   add1_shl2(58),   add1_shl2(60),   add1_shl2(61),
    add1_shl2(63),   add1_shl2(64),   add1_shl2(65),   add1_shl2(67),
    add1_shl2(68),   add1_shl2(70),   add1_shl2(71),   add1_shl2(73),
    add1_shl2(75),   add1_shl2(76),   add1_shl2(78),   add1_shl2(80),
    add1_shl2(82),   add1_shl2(83),   add1_shl2(85),   add1_shl2(87),
    add1_shl2(89),   add1_shl2(91),   add1_shl2(93),   add1_shl2(95),
    add1_shl2(97),   add1_shl2(99),   add1_shl2(102),  add1_shl2(104),
    add1_shl2(106),  add1_shl2(109),  add1_shl2(111),  add1_shl2(113),
    add1_shl2(116),  add1_shl2(118),  add1_shl2(121),  add1_shl2(124),
    add1_shl2(127),  add1_shl2(129),  add1_shl2(132),  add1_shl2(135),
    add1_shl2(138),  add1_shl2(141),  add1_shl2(144),  add1_shl2(147),
    add1_shl2(151),  add1_shl2(154),  add1_shl2(157),  add1_shl2(161),
    add1_shl2(165),  add1_shl2(168),  add1_shl2(172),  add1_shl2(176),
    add1_shl2(180),  add1_shl2(184),  add1_shl2(188),  add1_shl2(192),
    add1_shl2(196),  add1_shl2(200),  add1_shl2(205),  add1_shl2(209),
    add1_shl2(214),  add1_shl2(219),  add1_shl2(223),  add1_shl2(228),
    add1_shl2(233),  add1_shl2(238),  add1_shl2(244),  add1_shl2(249),
    add1_shl2(255),  add1_shl2(260),  add1_shl2(266),  add1_shl2(272),
    add1_shl2(278),  add1_shl2(284),  add1_shl2(290),  add1_shl2(296),
    add1_shl2(303),  add1_shl2(310),  add1_shl2(316),  add1_shl2(323),
    add1_shl2(331),  add1_shl2(338),  add1_shl2(345),  add1_shl2(353),
    add1_shl2(361),  add1_shl2(369),  add1_shl2(377),  add1_shl2(385),
    add1_shl2(393),  add1_shl2(402),  add1_shl2(411),  add1_shl2(420),
    add1_shl2(429),  add1_shl2(439),  add1_shl2(448),  add1_shl2(458),
    add1_shl2(468),  add1_shl2(478),  add1_shl2(489),  add1_shl2(500),
    add1_shl2(511),  add1_shl2(522),  add1_shl2(533),  add1_shl2(545),
    add1_shl2(557),  add1_shl2(569),  add1_shl2(582),  add1_shl2(594),
    add1_shl2(607),  add1_shl2(621),  add1_shl2(634),  add1_shl2(648),
    add1_shl2(663),  add1_shl2(677),  add1_shl2(692),  add1_shl2(707),
    add1_shl2(723),  add1_shl2(739),  add1_shl2(755),  add1_shl2(771),
    add1_shl2(788),  add1_shl2(806),  add1_shl2(823),  add1_shl2(841),
    add1_shl2(860),  add1_shl2(879),  add1_shl2(898),  add1_shl2(918),
    add1_shl2(938),  add1_shl2(958),  add1_shl2(979),  add1_shl2(1001),
    add1_shl2(1023), add1_shl2(1045), add1_shl2(1068), add1_shl2(1092),
    add1_shl2(1115), add1_shl2(1140), add1_shl2(1165), add1_shl2(1190),
    add1_shl2(1216), add1_shl2(1243), add1_shl2(1270), add1_shl2(1298),
    add1_shl2(1327), add1_shl2(1356), add1_shl2(1386), add1_shl2(1416),
    add1_shl2(1447), add1_shl2(1479), add1_shl2(1511), add1_shl2(1544),
    add1_shl2(1578), add1_shl2(1613), add1_shl2(1648), add1_shl2(1684),
    add1_shl2(1721), add1_shl2(1759), add1_shl2(1797), add1_shl2(1837),
    add1_shl2(1877), add1_shl2(1918), add1_shl2(1960), add1_shl2(2003),
    add1_shl2(2047), add1_shl2(2092), add1_shl2(2138), add1_shl2(2185),
    add1_shl2(2232), add1_shl2(2281), add1_shl2(2331), add1_shl2(2382),
    add1_shl2(2434), add1_shl2(2488), add1_shl2(2542), add1_shl2(2598),
    add1_shl2(2655), add1_shl2(2713), add1_shl2(2773), add1_shl2(2833),
    add1_shl2(2895), add1_shl2(2959), add1_shl2(3024), add1_shl2(3090),
    add1_shl2(3157), add1_shl2(3227), add1_shl2(3297), add1_shl2(3370),
    add1_shl2(3443), add1_shl2(3519), add1_shl2(3596), add1_shl2(3675),
    add1_shl2(3755), add1_shl2(3837), add1_shl2(3921), add1_shl2(4007),
    add1_shl2(4095)};
/* ************************* End of Table ILA **************************** */


/*___________________________________________________________________________
                                                                            
    Function Name : scalel                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 scalel (Word16 nbpl)
{
  Word16          wd1, wd2;

  wd1 = s_and(shr(nbpl, 6), 511);
  wd2 = add(wd1, 64);
#ifdef REMAPITU_T
  return ila[wd2];
#else
  return (shl(add(ila[wd2], 1), 2));
#endif
}
/* ..................... End of scalel() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : scaleh                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
                                                                            
 ___________________________________________________________________________
*/
Word16 scaleh (Word16 nbph)
{
  Word16          wd;

  wd = s_and(shr(nbph, 6), 511);

#ifdef REMAPITU_T
  return ila[wd];
#else
  return (shl(add(ila[wd], 1), 2));
#endif
}
/* ..................... End of scaleh() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : uppol1                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
     None.                                                                  
 ___________________________________________________________________________
*/
void uppol1 (Word16 al[], Word16 plt[])
{
  Word16          sg0, sg1, wd1, wd2, wd3, apl1;

  sg0 = shr (plt[0], 15);
  sg1 = shr (plt[1], 15);
  wd1 = -192;
#ifdef WMOPS
  move16();
#endif
#ifdef REMAPITU_T
  if(sg0 == sg1) {
    wd1 = 192;
#ifdef WMOPS
    move16();
#endif
  }
  wd2 = mult (al[1], 32640);
  apl1 = add (wd1, wd2);
  wd3 = sub (15360, al[2]);
#else
  if(sub(sg0, sg1) == 0)
  {
    wd1 = 192;
#ifdef WMOPS
    move16();
#endif
  }
  wd2 = mult (al[1], 32640);
  apl1 = add (wd1, wd2);
  wd3 = sub (15360, al[2]);
#endif
#ifdef REMAPITU_T
  IF(apl1 > wd3)
  {
    apl1 = wd3;
#ifdef WMOPS
    move16();
#endif
  }
  ELSE
  {
    Word16 neg_wd3 = negate(wd3);
    if(apl1 < neg_wd3)
    {
      apl1 = neg_wd3;
    }
  }
#else
  IF(sub(apl1, wd3) > 0)
  {
    apl1 = wd3;
#ifdef WMOPS
    move16();
#endif
  }
  ELSE
  {
    if(add(apl1, wd3) < 0)
    {
      apl1 = negate (wd3);
    }
  }
#endif

/*  apl1 = (apl1 > wd3) ? wd3 : ((apl1 < -wd3) ? negate (wd3) : apl1);*/

  /* Shift of the plt signals */
  plt[2] = plt[1];
  plt[1] = plt[0];
  al[1] = apl1;
#ifdef WMOPS
    move16();
    move16();
    move16();
#endif
}
/* ..................... End of uppol1() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : uppol2                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
     None.                                                                  
 ___________________________________________________________________________
*/
void uppol2 (Word16 al[], Word16 plt[])
{
  Word16          sg0, sg1, sg2, wd1, wd2, wd3, wd4, wd5, apl2;

  sg0 = shr (plt[0], 15);
  sg1 = shr (plt[1], 15);
  sg2 = shr (plt[2], 15);
  wd1 = shl (al[1], 2);
#ifdef REMAPITU_T
  if(sg0 == sg1)
  {
    wd2 = fx_asr_q15 (fx_sub_q15(0, wd1), 7);
  } else {
    wd2 = shr (wd1, 7);
  }
#else
  wd2 = add (0, wd1);
  if(sub(sg0, sg1) == 0)
  {
    wd2 = sub (0, wd1);
  }
  wd2 = shr (wd2, 7);
#endif
  wd3 = -128;
#ifdef WMOPS
  move16();
#endif
#ifdef REMAPITU_T
  if(sg0 == sg2)
#else
  if(sub(sg0, sg2) == 0)
#endif
  {
    wd3 = 128;
#ifdef WMOPS
    move16();
#endif
  }
  wd4 = add (wd2, wd3);
  wd5 = mult (al[2], 32512);
  apl2 = add (wd4, wd5);
#ifdef REMAPITU_T
  al[2] = MAX(MIN(apl2, 12288), -12288);
#else
  if(sub(apl2, 12288) > 0)
  {
    apl2 = 12288;
#ifdef WMOPS
    move16();
#endif
  }
  if(sub(apl2, -12288) < 0)
  {
    apl2 = -12288;
#ifdef WMOPS
    move16();
#endif
  }
  al[2] = apl2;
#ifdef WMOPS
  move16();
#endif
#endif
}
/* ..................... End of uppol2() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : upzero                                                  
                                                                            
    Purpose :                                                               
                                                                            
     .                  
                                                                            
    Inputs :                                                                
                                                                            
                                                                            
    Outputs :                                                               
                                                                            
     none                                                                   
                                                                            
    Return Value :                                                          
     None.                                                                  
                                                                            
 ___________________________________________________________________________
*/
void upzero (Word16 dlt[], Word16 bl[])
{

  Word16          sg0, sgi, wd1, wd3;
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i, wd2;
#endif

  /* shift of the dlt line signal and update of bl */

  wd1 = 128;
#ifdef WMOPS
  move16();
#endif
  if(dlt[0] == 0)
  {
    wd1 = 0;
#ifdef WMOPS
    move16();
#endif
  }
  sg0 = shr (dlt[0], 15);
#ifdef REMAPITU_T
  FOR (i = 6; i > 0; i--)
  {
    sgi = shr (dlt[i], 15);
    wd3 = mult (bl[i], 32640);
    if(sg0 == sgi) {
      bl[i] = add (wd1, wd3);
    } else {
      bl[i] = add (-wd1, wd3);
    }
    dlt[i] = dlt[i - 1];

  }
#else
  FOR (i = 6; i > 0; i--)
  {
    sgi = shr (dlt[i], 15);
    wd3 = mult (bl[i], 32640);
#ifdef REMAPITU_T
    if(sg0 == sgi) {
      bl[i] = add (wd1, wd3);
    } else {
      bl[i] = add (-wd1, wd3);
    }
#else
    wd2 = sub (0, wd1);
    if(sg0 == sgi)
    {
      wd2 = add (0, wd1);
    }
    bl[i] = add (wd2, wd3);
#endif
    dlt[i] = dlt[i - 1];

#ifdef WMOPS
    move16();
    move16();
#endif
  }
#endif
}
/* ..................... End of upzero() ..................... */


/* **** Coefficients for both transmission and reception QMF **** */
static const Word16   coef_qmf[24] =
  {
    3 * 2, -11 * 2, -11 * 2, 53 * 2, 12 * 2, -156 * 2,
    32 * 2, 362 * 2, -210 * 2, -805 * 2, 951 * 2, 3876 * 2,
    3876 * 2, 951 * 2, -805 * 2, -210 * 2, 362 * 2, 32 * 2,
    -156 * 2, 12 * 2, 53 * 2, -11 * 2, -11 * 2, 3 * 2};
#if defined(REMAPITU_T) && defined(core_config_xy_x_base) && defined(USE_XCCM)
#pragma data()
#endif
/* ..................... End of table coef_qmf[] ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : qmf_tx                                                  
                                                                            
    Purpose :                                                               
                                                                            
     G722 QMF analysis (encoder) filter. Uses coefficients in array         
     coef_qmf[] defined above.                                              
                                                                            
    Inputs :                                                                
     xin0 - first sample for the QMF filter (read-only)                     
     xin1 - secon sample for the QMF filter (read-only)                     
     xl   - lower band portion of samples xin0 and xin1 (write-only)        
     xh   - higher band portion of samples xin0 and xin1 (write-only)       
     s    - pointer to state variable structure (read/write)                
                                                                            
    Return Value :                                                          
     None.                                                                  
 ___________________________________________________________________________
*/
#define delayx s->qmf_tx_delayx
void qmf_tx (Word16 xin0, Word16 xin1, Word16 *xl, Word16 *xh, g722_state *s)
{

  /* Local variables */
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i;
  Word32          accuma, accumb;
#endif
  Word32          comp_low, comp_high;
  S_xy Word16 *pcoef, *pdelayx;

  /* Saving past samples in delay line */
  delayx[1] = xin1;
  delayx[0] = xin0;
#ifdef WMOPS
  move16();
  move16();
#endif

  /* QMF filtering */
  pcoef = (S_xy Word16 *)coef_qmf;
  pdelayx = (S_xy Word16 *)delayx;
#ifdef WMOPS
  move16();
  move16();
#endif

#ifdef REMAPITU_T
  v2accum32_t accum_ab = {0, 0};
  S_xy v2q15_t *pv2coef = (S_xy v2q15_t *)pcoef;
  S_xy v2q15_t *pv2delayx = (S_xy v2q15_t *)pdelayx;
#pragma clang loop unroll_count(2)
  FOR (i = 0; i < 12; i++)
  {
    accum_ab = fx_v2a32_mac_nf_v2q15(accum_ab, *pv2coef++, *pv2delayx++);

  }
#else
  accuma = L_mult0(*pcoef++, *pdelayx++);
  accumb = L_mult0(*pcoef++, *pdelayx++);
  FOR (i = 1; i < 12; i++)
  {
    accuma = L_mac0(accuma, *pcoef++, *pdelayx++);
    accumb = L_mac0(accumb, *pcoef++, *pdelayx++);
  }
#endif
  /* Descaling and shift of the delay line */
  FOR (i = 0; i < 22; i++)
  {
    delayx[23 - i] = delayx[21 - i];
#ifdef WMOPS
    move16();
#endif
  }
#ifdef REMAPITU_T
  comp_low = fx_q31_cast_asl_rnd_a32(fx_add_a32(fx_get_v2a32(accum_ab, 0), fx_get_v2a32(accum_ab, 1)), 1);
  comp_high = fx_q31_cast_asl_rnd_a32(fx_sub_a32(fx_get_v2a32(accum_ab, 0), fx_get_v2a32(accum_ab, 1)), 1);
#else
  comp_low = L_add (accuma, accumb);
  comp_low = L_add (comp_low, comp_low);

  comp_high = L_sub (accuma, accumb);
  comp_high = L_add (comp_high, comp_high);
#endif
  *xl = limit ((Word16) L_shr (comp_low, (Word16) 16));
  *xh = limit ((Word16) L_shr (comp_high, (Word16) 16));
}
#undef delayx
/* ..................... End of qmf_tx() ..................... */


/*___________________________________________________________________________
                                                                            
    Function Name : qmf_rx_buf                                                  
                                                                            
     G722 QMF synthesis (decoder) filter, whitout memory shift. 
     Uses coefficients in array coef_qmf[] defined above.                                              
                                                                            
    Inputs :                                                                
     out      - out of the QMF filter (write-only)                
     rl       - lower band portion of a sample (read-only)                     
     rh       - higher band portion of a sample (read-only)                    
     *delayx  - pointer to delay line allocated outside               
                                                                            
    Return Value :                                                          
     None.                                                                  
 ___________________________________________________________________________
*/

/* Complexity optimization : note that the qmf_rx function can be replaced by 
 *                 the below defined complexity optimized qmf_rx_buf function 
 *                 See also complexity note in g722.c file.
 */


#define delayx s->qmf_rx_delayx
void qmf_rx (Word16 rl, Word16 rh, Word16 *xout1, Word16 *xout2, g722_state *s)
{
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i;
#endif
  Word32          comp_low, comp_high;

  /* compute sum and difference from lower-band (rl) and higher-band (rh) signals */
  /* update delay line */
  delayx[1] = add (rl, rh);
  delayx[0] = sub (rl, rh);
#ifdef WMOPS
    move16();
    move16();
#endif
  /* qmf_rx filtering */
#ifdef WMOPS
  move16();
  move16();
#endif
#ifdef REMAPITU_T
  v2accum32_t accum_ab = {0, 0};
  S_xy v2q15_t *pv2coef = (S_xy v2q15_t *)coef_qmf;
  S_xy v2q15_t *pv2delayx = (S_xy v2q15_t *)delayx;
#pragma clang loop unroll_count(2)
  FOR (i = 0; i < 12; i++)
  {
    accum_ab = fx_v2a32_mac_nf_v2q15(accum_ab, *pv2coef++, *pv2delayx++);
  }
  comp_low = fx_q31_cast_asl_rnd_a32(fx_get_v2a32(accum_ab, 0), 4);
  comp_high = fx_q31_cast_asl_rnd_a32(fx_get_v2a32(accum_ab, 1), 4);
#else
  Word32          accuma, accumb;
  accuma = L_mult0(delayx[0], coef_qmf[0]);
  accumb = L_mult0(coef_qmf[1], delayx[1]);
  FOR (i = 2; i < 24; i+=2)
  {
    accuma = L_mac0(accuma, delayx[i], coef_qmf[i]);
    accumb = L_mac0(accumb, delayx[i+1], coef_qmf[i+1]);
  }
  comp_low = L_shl(accuma,4);
  comp_high = L_shl(accumb,4);
#endif
  /* shift delay line */
#pragma clang loop unroll_count(2)
  FOR(i = 0; i < 22; i++)
  {
    delayx[23 - i] = delayx[21 - i];
#ifdef WMOPS
    move16();
#endif
  }


  /* compute output samples */
  *xout1 = extract_h(comp_low);
  *xout2 = extract_h(comp_high);
}

#undef delayx
/* ..................... End of qmf_rx() ..................... */


void qmf_rx_buf (Word16 rl, Word16 rh, Word16 **delayx, Word16 **out)
{
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i;
#endif
  Word32          comp_low, comp_high;
  S_xy Word16 *pcoef, *pdelayx;

  /* compute sum and difference from lower-band (rl) and higher-band (rh) signals */
  /* update delay line */
  *--(*delayx) = add (rl, rh);
  *--(*delayx) = sub (rl, rh);
#ifdef WMOPS
    move16();
    move16();
#endif

  /* qmf_rx filtering */
  pcoef = (S_xy Word16 *)coef_qmf;
  pdelayx = (S_xy Word16 *)*delayx;
#ifdef WMOPS
  move16();
  move16();
#endif
#ifdef REMAPITU_T
  v2accum32_t accum_ab = {0, 0};
  S_xy v2q15_t *pv2coef = (S_xy v2q15_t *)pcoef;
  S_xy v2q15_t *pv2delayx = (S_xy v2q15_t *)pdelayx;
#pragma clang loop unroll_count(2)
  FOR (i = 0; i < 12; i++)
  {
    accum_ab = fx_v2a32_mac_nf_v2q15(accum_ab, *pv2coef++, *pv2delayx++);

  }
  comp_low = fx_q31_cast_asl_rnd_a32(fx_get_v2a32(accum_ab, 0), 4);
  comp_high = fx_q31_cast_asl_rnd_a32(fx_get_v2a32(accum_ab, 1), 4);
#else
  Word32          accuma, accumb;
  accuma = L_mult0(*pcoef++, *pdelayx++);
  accumb = L_mult0(*pcoef++, *pdelayx++);
  FOR (i = 1; i < 12; i++)
  {
    accuma = L_mac0(accuma, *pcoef++, *pdelayx++);
    accumb = L_mac0(accumb, *pcoef++, *pdelayx++);
  }
  comp_low = L_shl(accuma,4);
  comp_high = L_shl(accumb,4);
#endif


  /* compute output samples */
  *(*out)++ = extract_h(comp_low);
  *(*out)++ = extract_h(comp_high);
#ifdef WMOPS
  move16();
  move16();
#endif
}


/* ******************** End of funcg722.c ***************************** */
