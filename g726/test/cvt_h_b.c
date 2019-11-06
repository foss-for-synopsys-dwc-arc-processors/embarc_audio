/*
==============================================================================

        CVT_H_B.C
        ~~~~~~~~~

        Program to convert ITU-T test sequences in hexa formats to
        word oriented binary files. Each line is supposed to have 32 valid
        samples (16 bit, or a word), or 64 hexa characters, either upper or
        lower case. In the last line is expected one word (2 hexa chars)
        in hexadecimal representation as the checksum of all the samples of
        the file modulus 255 (sum mod 255).


        USAGE:
        ~~~~~~
        $ cvt_h_b [-r] [-q] hex-in bin-out 
        where:
        hex-in          is the (input) hexadecimal file's name
        bin-out         is the (output) binary file's name
        
        Options
        ~~~~~~~
        -q              Don't print progress indicator on the screen.
        -r              is an optional parameter; if specified as r or R,
                        even-bit reversal of all samples is accomplished 
                        before saving the file. Default is NOT invert.

        Compile:
        ~~~~~~~~
        SunC: cc -o cvt_h_b cvt_h_b.c # or #
              acc -o cvt_h_b cvt_h_b.c
        GNUC: gcc -o cvt_h_b cvt_h_b.c
        VMS:  cc cvt_h_b.c
              link cvt_h_b
 
        History:
        ~~~~~~~~
        92.01.22   1.0   1st release by tdsimao@venus.cpqd.ansp.br
        93.11.23   1.01  ported to SunOS (Unix)
        10.02.03   1.02  Avoid buffer overruns (y.hiwasaki)

       =============================================================
       COPYRIGHT NOTE: This source code, and all of its derivations,
       is subject to the ITU-T General Public License". Please have
       it  read  in    the  distribution  disk,   or  in  the  ITU-T
       Recommendation G.191 on "SOFTWARE TOOLS FOR SPEECH AND  AUDIO
       CODING STANDARDS".
       =============================================================
==============================================================================
*/

/* OS definition */
#ifdef __MSDOS__  /* definition for automatic compil. under TurboC v2.0 */
# define MSDOS	  /* this is already defined for Microsoft C 5.1 */
#endif


/* includes in general */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* Specific includes */
#if defined(MSDOS)	/* for MSDOS */
#  include <fcntl.h>
#  include <io.h>
#  include <sys\stat.h>
#elif defined(VMS)  	/* for VMS */
#  include <perror.h>
#  include <file.h>
#  include <stat.h>
#else			/* for OTHER OS, in special UNIX */
#  include <sys/stat.h>
#endif


/* Generic defines */
#define MAX_STRLEN 1024
#define askc(m,v) {printf(m);scanf("%c%*c",&v);}
#define aski(m,v) {printf(m);scanf("%d",&v);}
#define asks(m,v) {printf(m);fgets(v,sizeof(v),stdin);*(v+strlen(v)-1)='\0';}
#define KILL(str,code) {perror(str); exit(code);}
#define GET_PAR_I(a,s,p,msg,i) {if(a>p) i=atoi(s[p]); else aski(msg,i);}
#define GET_PAR_C(a,s,p,msg,c) {if(a>p) c=toupper((int)s[p][0]);else askc(msg,c);}
#define GET_PAR_S(a,s,p,msg,S) {if((a>p)&&(strlen(s[p])<sizeof(S))) strcpy(S,s[p]); else asks(msg,S);}

#if defined(VMS)
#define WB  "wb","mrs=512","rfm=fix","ctx=stm"
#define RB  "rb","mrs=512","rfm=fix","ctx=stm"
#elif defined(MSDOS)
#define WB  "wb"
#define RB  "rb"
#else
#define WB  "w"
#define RB  "r"
#endif

#define YES 1
#define NO  0

/* 
 * --------------------------------------------------------------------------
 * ... Calculate check-sum of a array of shorts ... 
 *     Simao 07.Mar.94
 * --------------------------------------------------------------------------
 */
short short_check_sum(s_data, n)
short *s_data;
long n;
{
  float csum = 0;
  long i;
  
  /* Add samples */
  while(n) 
    csum += s_data[--n];
    
  /* return mod as a short */
  csum = fmod(csum,255.0);
  return ((short)csum);
}


/* 
 * --------------------------------------------------------------------------
 * ... Display usage of program ... 
 *     Simao 10.Jan.93
 * --------------------------------------------------------------------------
 */
#define FP(x) printf(x)
void display_usage()
{
  FP("Version 1.01 of 23/Nov/1993 \n");
 
  FP(" CVT_H_B.C:\n");
  FP(" Program to convert ITU-T test sequences in hexa formats to\n");
  FP(" word oriented binary files. Each line is supposed to have 32 valid\n");
  FP(" samples (16 bit, or a word), or 64 hexa characters, either upper or\n");
  FP(" lower case. In the last line is expected one word (2 hexa chars)\n");
  FP(" in hexadecimal representation as the checksum of all the samples of\n");
  FP(" the file modulus 255 (sum mod 255).\n");
  FP("\n");
  FP(" Usage:\n");
  FP(" $ cvt_h_b [-r] hex-in bin-out\n");
  FP(" where:\n");
  FP("  hex-in          is the (input) hexadecimal file's name\n");
  FP("  bin-out         is the (output) binary file's name\n\n");

  FP(" Options\n");
  FP("  -q              Don't print progress indicator on the screen\n");
  FP("  -r              is an optional parameter; if specified as r or R,\n");
  FP("                  even-bit reversal of all samples is accomplished \n");
  FP("                  before saving the file. Default is NOT invert.\n");
  FP("\n");

  /* Quit program */
  exit(-128);
}
#undef FP
/* ....................... end of display_usage() ...........................*/


/*============================== */
main(argc,argv)
int argc;
char *argv[];
/*============================== */
{
  char line[200], *lp;
  char inpfil[MAX_STRLEN], outfil[MAX_STRLEN];
  int inp, out, itmp, count=0;
  float check_sum=0;
  short scheck_sum=0;
  short chk;
  short *bin,*tmp;
  long leng,i,smpno=0,lineno=0;
  int lastline_len = 0;

  FILE *Fi,*Fo;
  struct stat st;
  int invert_even_bits = NO, quiet = NO;

  static char     funny[8] =  {'\\','-','/','|','\\','-','/','|'};


  /* GETTING OPTIONS */

  if (argc < 2)
    display_usage();
  else
  {
    while (argc > 1 && argv[1][0] == '-')
      if (strcmp(argv[1],"-r")==0)
      {
	/* Reverse even bits */
	invert_even_bits = YES;
	
	/* Move argv over the option to the next argument */
	argv++;

	/* Update argc */
	argc--;
      }
      else if (strcmp(argv[1],"-q")==0)
      {
	/* Don't print funny chars */
	quiet = YES;
	
	/* Move argv over the option to the next argument */
	argv++;
	argc--;
      }
      else
      {
	fprintf(stderr, "ERROR! Invalid option \"%s\" in command line\n\n",
		argv[1]);
	display_usage();
      }
  }

  

  /* WELCOME! */

  fprintf (stderr, "\n\t\tConversion from ITU-T HEX of test sequence");
  fprintf (stderr, "\n\t\tfiles to binary files, 16 bits per sample\n");


  /* GETTING PARAMETERS */

  GET_PAR_S(argc, argv, 1, "_Input (ITU-T HEX) file: ....... ", inpfil);
  GET_PAR_S(argc, argv, 2, "_Output (`short') file: ........ ", outfil);


  /* OPEN FILES */

  if ((Fi=fopen(inpfil,"r"))==NULL) KILL(inpfil,2);
  if ((Fo=fopen(outfil,WB))==NULL) KILL(outfil,3);
  out=fileno(Fo);

  /* FIND FILE SIZE AND ALLOC MEMORY FOR BINARY BUFFER */

  stat(inpfil,&st);
  if ((bin = (short *)malloc(st.st_size))==(short *)NULL)
  {
    fprintf(stderr,"\n Can't allocate memory for output buffer");
    exit(4);
  }
  tmp=bin;


  /* INFOS */

  fprintf (stderr, "\n  Converting %s to %s\r", inpfil, outfil);


  /* CONVERSION PART */

  /* FIND NUMBER OF LINES IN FILE */
  while(fgets(line,(int)200,Fi)!=NULL)
    lineno++;
  rewind(Fi);
  lastline_len = strlen(line);
  if (line[lastline_len-1]='\n') 
    lastline_len--; 
  
  /* READ ALL CHARS FOR THE FILE */
  while(fgets(line,(int)200,Fi)!=NULL)
  {
    /* Print `working' flag */
    if (!quiet) 
     fprintf(stderr, "%c\r", funny[count % 8]);

    /* Update line counter */
    count++;
    
    /* Remove CR from line end */
    leng = strlen(line);
    if (line[leng-1] == '\n')
      line[--leng] = '\0';

    /* Convert line */
    switch(leng)
    {
      case 64:
	for (lp=line, i=0;i<leng;i+=2, lp+=2)
	{
	  sscanf(lp,"%2hx",tmp);
	  tmp++; smpno++;
	}
	break;

      case 2: 
        /* Check whether the last line */
        if (count != lineno)
        {
          /* If not, it's a regular sample */
          sscanf(lp,"%2hx",tmp);
	  tmp++; smpno++;
        }
        else
        {
          /* value is check-sum */
	  sscanf(line,"%2hx",&chk);
	}
	break;

      default:   /* for old (red-book) test sequences: initialization */
	for (i=0;i<leng-2;i+=2)
	{
	  sscanf(&line[i],"%2hx",tmp);
	  tmp++; smpno++;
	}

        /* Check sum for old files ... */
	sscanf(&line[i],"%2hx",&chk);
	break;

    }
  }

  /* CALCULATE CHECK-SUMS */
  scheck_sum = short_check_sum(bin, smpno);
  fprintf(stderr, "Check-sum is: %d (calculated) %d (read)\n",
                  scheck_sum, chk);
  if (chk != scheck_sum)
  {
    fprintf(stderr, "\n Checksum Failed! (%d != %d)\n", chk, scheck_sum);
    exit(5);
  }


  /* INVERT EVEN BITS, IF IT IS THE CASE */

  if (invert_even_bits)
  {
    fprintf(stderr, "\r  Inverting even bits, as requested\t\t");
    for (i=0; i<(tmp-bin)/2; i++) bin[i] ^= 0x55;
  }


  /* SAVE TO FILE */

  for (i=0;i<smpno;i+=256)
  {
    if (write(out,&bin[i],512)!=512) KILL(outfil,6);
  }


  /* EXITING */

  fprintf(stderr, "\rDone: %d samples converted!\n",smpno);
  fclose(Fi); fclose(Fo);
#ifndef VMS
  exit(0);
#endif
}

