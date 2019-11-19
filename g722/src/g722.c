/*
* Copyright 2019-present, Synopsys, Inc.
* All rights reserved.
*
* Synopsys modifications to this source code is licensed under
* the ITU-T SOFTWARE TOOLS' GENERAL PUBLIC LICENSE found in the
* LICENSE.md file in the root directory of this source tree.
*/
/* ITU G.722 3rd Edition (2012-09) */

/* ITU-T G722 PLC Appendix IV - Reference C code for fixed-point implementation */
/* Version:       1.2                                                           */
/* Revision Date: 29 October 2009 (Approved: 6 Noc 2009)                        */

/*
   ITU-T G.722 PLC Appendix IV   ANSI-C Source Code
   Copyright (c) 2006-2007
   France Telecom
*/

#include "g722.h"
#include "stl.h"

#include "g722_plc.h"
#include "funcg722.h"
#include "memory_control.h"
#include <stdlib.h>

#define INV_L_8_FRAMES_P1_8KHZ_int   51 /* Q15 */
#define INV_L_8_FRAMES_P1_8KHZ_frc 3936 /* Q15 of fraction */
#define INV_L_4_FRAMES_P1_8KHZ_int  102 /* Q15 */
#define INV_L_4_FRAMES_P1_8KHZ_frc 2654 /* Q15 of fraction */

#ifdef NATIVE_CYCLE_PROFILING
#include "helper_lib.h"
#include <string.h>
#endif

void g722_reset_encoder(encoder)
g722_state *encoder;
{

  Word16          xl, il;
  Word16          xh, ih, j;

  xl = xh = 0;
#ifdef WMOPS
    move16();
    move16();
#endif
  FOR (j = 0; j < 24; j++)
  {
    encoder->qmf_tx_delayx[j] = 0;
#ifdef WMOPS
    move16();
#endif
  }
  il = lsbcod (xl, 1, encoder);
  ih = hsbcod (xh, 1, encoder);
}
/* .................... end of g722_reset_encoder() ....................... */


Word32 g722_encode(incode,code,read1,encoder)
  short *incode;
  short *code;
  Word32 read1;
  g722_state     *encoder;
{
  /* Encoder variables */
  Word16          xl, il;
  Word16          xh, ih;
  Word16          xin0, xin1;

  /* Auxiliary variables */
  Word32             i;
  Word16          *p_incode;
#ifdef REMAPITU_T
  fx_init_dsp_mode();
#endif
  /* Divide sample counter by 2 to account for QMF operation */
  read1 = L_shr(read1, 1);

  /* Main loop - never reset */
	p_incode = incode;
#ifdef WMOPS
    move16();
#endif
  FOR (i = 0; i < read1; i++)
  {
    xin1 = *incode++;
    xin0 = *incode++;
#ifdef WMOPS
    move16();
    move16();
#endif

    /* Calculation of the synthesis QMF samples */
    qmf_tx (xin0, xin1, &xl, &xh, encoder);

    /* Call the upper and lower band ADPCM encoders */
    il = lsbcod (xl, 0, encoder);
    ih = hsbcod (xh, 0, encoder);

    /* Mount the output G722 codeword: bits 0 to 5 are the lower-band
     * portion of the encoding, and bits 6 and 7 are the upper-band
     * portion of the encoding */
    code[i] = s_and(add(shl(ih, 6), il), 0xFF);
#ifdef WMOPS
    move16();
#endif
  }

  /* Return number of samples read */
  return(read1);
}
/* .................... end of g722_encode() .......................... */

void g722_reset_decoder(g722_state *decoder)
{
  Word16          il, ih;
  Word16          rl, rh;
  Word16          j;

  il = ih = 0;
#ifdef WMOPS
    move16();
    move16();
#endif
  FOR (j = 0; j < 24; j++)
  {
    decoder->qmf_rx_delayx[j] = 0;
#ifdef WMOPS
    move16();
#endif
  }
  rl = lsbdec (il, (Word16)0, 1, decoder);
  rh = hsbdec (ih, 1, decoder);
}
/* .................... end of g722_reset_decoder() ....................... */

Word32 G722PLC_decode(short *code, short *outcode, short mode, Word32 read1,
                      g722_state *decoder,void *plc_state)
{
  /* Decoder variables */
  Word16          il, ih;
  Word16          rl, rh;
  Word16          xout1, xout2;
    
  /* Auxiliary variables */
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i;
#endif
  Word16 ind_l, ind_h;
  Word16 *ptr_l, *ptr_h;
  G722PLC_STATE *state = (G722PLC_STATE *) plc_state;
  Word16 * filtmem, *filtptr;
  Word16 weight;
/* Complexity optimization : note that the qmf_rx function called by G722PLC_decode
 *                 can be replaced by the complexity optimized qmf_rx_buf function 
 *                 in the same way as it is done in G722PLC_qmf_updstat.
 */

  /* shift speech buffers */
	ind_l = sub(state->l_mem_speech, state->l_frame);
	G722PLC_copy(&state->mem_speech[state->l_frame],state->mem_speech, ind_l); /*shift 10 ms*/
	ind_h = sub(160, state->l_frame); 
	G722PLC_copy(&state->mem_speech_hb[state->l_frame],state->mem_speech_hb, ind_h); /*shift 10 ms*/

  ptr_l = &(state->mem_speech[ind_l]);
  ptr_h = &(state->mem_speech_hb[ind_h]);
    
  /* increment counter */
#ifdef REMAPITU_T
  if(state->count_hpf > 32000)
#else
  if(sub(state->count_hpf, 32000) > 0)
#endif
  {
    state->count_hpf = 32000;
#ifdef WMOPS
    move16();
#endif
  }
  state->count_hpf = add(state->count_hpf, (Word16)read1); /* upper count after loop, 32000 = 4sec*/
#ifdef WMOPS
		move16();
#endif
  /* Decode - reset is never applied here */
  i = 0;
#ifdef WMOPS
		move16();
#endif

  IF(state->count_crossfade == 0) /* first good frame, crossfade is needed*/
  {
    filtmem = (Word16 *)CALLOC(filtmem, (add(CROSSFADELEN16, 22) * sizeof(Word16)));
    G722PLC_copy(&decoder->qmf_rx_delayx[2],&filtmem[CROSSFADELEN16], 22); /*load memory*/
    filtptr = &filtmem[CROSSFADELEN16];
    
    FOR (i = 0; i < 20; i++)
    {
      /* Separate the input G722 codeword: bits 0 to 5 are the lower-band
       * portion of the encoding, and bits 6 and 7 are the upper-band
       * portion of the encoding */
      il = s_and(code[i], 0x3F);	/* 6 bits of low SB */
      ih = s_and(lshr(code[i], 6), 0x03);/* 2 bits of high SB */
      
      /* Call the upper and lower band ADPCM decoders */
      rl = lsbdec (il, mode, 0, decoder);
      rh = hsbdec (ih, 0, decoder);
      
      /* cross-fade samples with PLC synthesis (in lower band only) */
      rl = mult(state->crossfade_buf[i], 32767);
      
      /* remove-DC filter */
      rh = G722PLC_rem_dc(&state->mem_hpf_in, &state->mem_hpf_out, rh);
      
      /* copy lower and higher band sample */
      *ptr_l++ = rl;
      *ptr_h++ = rh;
#ifdef WMOPS
      move16();
      move16();
#endif
      
      /* Calculation of output samples from QMF filter */
      qmf_rx_buf (rl, rh, &filtptr, &outcode);
      
    }
    weight = 546;
#ifdef WMOPS
      move16();
#endif
    FOR (; i < CROSSFADELEN; i++)
    {
      /* Separate the input G722 codeword: bits 0 to 5 are the lower-band
       * portion of the encoding, and bits 6 and 7 are the upper-band
       * portion of the encoding */
      il = s_and(code[i], 0x3F);	/* 6 bits of low SB */
      ih = s_and(lshr(code[i], 6), 0x03);/* 2 bits of high SB */
      
      /* Call the upper and lower band ADPCM decoders */
      rl = lsbdec (il, mode, 0, decoder);
      rh = hsbdec (ih, 0, decoder);
      
      /* cross-fade samples with PLC synthesis (in lower band only) */
      rl = add(mult(rl, weight), mult(state->crossfade_buf[i], sub(32767, weight)));
      weight = add(weight, 546);
      
      /* remove-DC filter */
      rh = G722PLC_rem_dc(&state->mem_hpf_in, &state->mem_hpf_out, rh);
      
      /* copy lower and higher band sample */
      *ptr_l++ = rl;
      *ptr_h++ = rh;
#ifdef WMOPS
      move16();
      move16();
#endif
      
      /* Calculation of output samples from QMF filter */
      qmf_rx_buf (rl, rh, &filtptr, &outcode);
      
    }
    state->count_crossfade = add(state->count_crossfade, CROSSFADELEN); 
#ifdef WMOPS
    move16();
#endif   
    G722PLC_copy(filtmem,&decoder->qmf_rx_delayx[2], 22); /*save memory*/
    FREE(filtmem);

  }
  
  FOR (; i < read1; i++)
  {
    /* Separate the input G722 codeword: bits 0 to 5 are the lower-band
     * portion of the encoding, and bits 6 and 7 are the upper-band
     * portion of the encoding */
    il = s_and(code[i], 0x3F);	/* 6 bits of low SB */
    ih = s_and(lshr(code[i], 6), 0x03);/* 2 bits of high SB */
    
    /* Call the upper and lower band ADPCM decoders */
    rl = lsbdec (il, mode, 0, decoder);
    rh = hsbdec (ih, 0, decoder);
    
    /* remove-DC filter */
    IF(sub(state->count_hpf, 32000) <= 0) /* 4 s good frame HP filtering (offset remove) */
    {
      rh = G722PLC_rem_dc(&state->mem_hpf_in, &state->mem_hpf_out, rh);
    }
    
    /* copy lower and higher band sample */
    *ptr_l++ = rl;
    *ptr_h++ = rh;
#ifdef WMOPS
    move16();
    move16();
#endif
    
    /* Calculation of output samples from the reference QMF filter */
    qmf_rx (rl, rh, &xout1, &xout2, decoder);

    /* Save samples in output vector */
    *outcode++ = xout1;
    *outcode++ = xout2;
#ifdef WMOPS
    move16();
    move16();
#endif      
    
  }

  /* Return number of samples read */
  return(shl((Word16)read1,1));
}
/* .................... end of g722_decode() .......................... */

#define DLT  decoder->dlt
#define PLT  decoder->plt
#define RLT  decoder->rlt
#define SL   decoder->sl
#define SZL  decoder->szl
#define DETL decoder->detl
#define NBL  decoder->nbl
#define DETH decoder->deth
#define NBH  decoder->nbh

void G722PLC_qmf_updstat (short *outcode, short read1, g722_state *decoder, short *lb_signal, short *hb_signal, void *plc_state)
{
  Word16  rh;
#ifdef REMAPITU_T
  Word32          i;
#else
  Word16          i;
#endif
  G722PLC_STATE * state = (G722PLC_STATE *) plc_state;
  Word16 *endLastOut; 
  Word16 *firstFuture;
  Word16 * filtmem, *filtptr;
  Word16 read2;

  read2 = shl(read1,1);
  filtmem = (Word16 *)CALLOC(filtmem, (add(read2, 22) * sizeof(Word16)));
	G722PLC_copy(&decoder->qmf_rx_delayx[2],&filtmem[read2], 22); /*load memory*/
  filtptr = &filtmem[read2];
  FOR (i = 0; i < read1; i++)
  {
    /* filter higher-band */
    rh = G722PLC_rem_dc(&state->mem_hpf_in, &state->mem_hpf_out, *hb_signal);

    /* calculate output samples of QMF filter */
    qmf_rx_buf (*lb_signal, rh, &filtptr, &outcode);
			
    lb_signal++;
    hb_signal++;
  }
	G722PLC_copy(filtmem,&decoder->qmf_rx_delayx[2], 22); /*save memory*/
	FREE(filtmem);

  /* reset G.722 decoder */ 
  endLastOut = &(state->mem_speech[state->l_mem_speech - 1]);
  firstFuture = state->crossfade_buf;

  DLT[0] = DLT[1] = DLT[2] = DLT[3] = DLT[4] = DLT[5] = DLT[6] = 0;
#ifdef WMOPS
  move16();
  move16();
  move16();
  move16();
  move16();
  move16();
  move16();
#endif

  PLT[1] = shr(endLastOut[0],1);
  PLT[2] = shr(endLastOut[-1],1);

  RLT[1] = endLastOut[0];

  SL = firstFuture[0];
  SZL = shr(firstFuture[0],1);
#ifdef WMOPS
  move16();
  move16();
  move16();
  move16();
  move16();
  move16();
#endif

  /* change scale factors (to avoid overshoot) */
  NBH  = shr(NBH, 1);
  DETH = scaleh(NBH); 
#ifdef WMOPS
    move16();
    move16();
#endif

  /* reset G.722 decoder after muting */
#ifdef REMAITU_T
	IF(state->count_att_hb > 160)
#else
	IF(sub(state->count_att_hb, 160) > 0)
#endif
	{
    DETL = 32;
    NBL = 0;
    DETH = 8;
    NBH = 0;
#ifdef WMOPS
    move16();
    move16();
    move16();
    move16();
#endif
	}

}

#undef DLT 
#undef PLT 
#undef RLT 
#undef SL  
#undef SZL 
#undef DETL
#undef NBL
#undef DETH
#undef DH 
#undef NBH

/* .................... end of G722PLC_qmf_updstat() .......................... */

