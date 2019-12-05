/*
  ============================================================================
   File: G726.H                                        28-Feb-1991 (18:00:00)
  ============================================================================

                            UGST/ITU-T G726 MODULE

                          GLOBAL FUNCTION PROTOTYPES

   History:
   28.Feb.92	v1.0	First version <simao@cpqd.br>
   06.May.94    v2.0    Smart prototypes that work with many compilers <simao> 
  ============================================================================
*/
#ifndef G726_defined
#define G726_defined 200

/* Smart function prototypes: for [ag]cc, VaxC, and [tb]cc */
#if !defined(ARGS)
#if (defined(__STDC__) || defined(VMS) || defined(__DECC)  || defined(MSDOS) || defined(__MSDOS__)) || defined (__CYGWIN__) || defined (_MSC_VER)
#define ARGS(s) s
#else
#define ARGS(s) ()
#endif
#endif


/* State for G726 encoder and decoder */
typedef struct
{
  short           sr0, sr1;	/* Reconstructed signal with delays 0 and 1 */
  short           a1r, a2r;	/* Triggered 2nd order predictor coeffs. */
  short           b1r;		/* Triggered 6nd order predictor coeffs */
  short           b2r;
  short           b3r;
  short           b4r;
  short           b5r;
  short           b6r;
  short           dq5;		/* Quantized difference signal with delays 5
				 * to 0 */
  short           dq4;
  short           dq3;
  short           dq2;
  short           dq1;
  short           dq0;
  short           dmsp;		/* Short term average of the F(I) sequence */
  short           dmlp;		/* Long term average of the F(I) sequence */
  short           apr;		/* Triggered unlimited speed control
				 * parameter */
  short           yup;		/* Fast quantizer scale factor */
  short           tdr;		/* Triggered tone detector */
  short           pk0, pk1;	/* sign of dq+sez with delays 0 and 1 */
  long            ylp;		/* Slow quantizer scale factor */
}               G726_state;

#ifdef VAXC
#  define SHORT short
#else
#  define SHORT int
#endif

/* Function prototypes */
void G726_encode ARGS((short *inp_buf, short *out_buf, long smpno, char *law, 
	SHORT rate, SHORT r, G726_state *state));
void G726_decode ARGS((short *inp_buf, short *out_buf, long smpno, char *law, 
	SHORT rate, SHORT r, G726_state *state));

/* Definitions for better user interface (?!) */
#ifndef IS_LOG 
#  define IS_LOG   0
#endif

#ifndef IS_LIN
#  define IS_LIN   1
#endif

#ifndef IS_ADPCM
#  define IS_ADPCM 2
#endif

#endif
/* .......................... End of G726.H ........................... */
