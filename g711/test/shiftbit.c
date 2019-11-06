/*                                                        02.Feb.2010 v1.1
   =======================================================================

   SHIFTBIT.C
   ~~~~~~~~~~

   Description
   ~~~~~~~~~~~

   Shift samples from input file (in Qx format) such that the output file
   will be in the Qy format, with sign extension. In fact, what this does
   is multiply by the factor 2**(y-x) (if left-shift) or divide by the
   factor 2**(x-y) (if right-shift) the input samples.

   Includes:
   ~~~~~~~~~
      specific: ugstdemo.h
      system:   stdio.h,stat.h

   Author:
   ~~~~~~~

   Simao Ferraz de Campos Neto
   DDS/Pr11                      Tel: +55-192-39-1396                
   CPqD/Telebras                 Fax: +55-192-53-4754                
   13085 Campinas SP Brazil      E-mail: <tdsimao@venus.cpqd.ansp.br>
                                       
   History:
   ~~~~~~~~
   08.Feb.91 v1.0 Created!
   02.Feb.10 v1.1 Modified maximum string length (y.hiwasaki)

   =======================================================================
*/

#include "ugstdemo.h"
#include <stdio.h>

/* 
 * Includes dependent of the Operating System 
 */
#if defined(VMS)
# include <stat.h>
#elif defined (MSDOS)
# include <sys\stat.h>
#else
# include <sys/stat.h>
#endif

#define RIGHT 1
#define LEFT -1

main(argc,argv)
int argc;
char *argv[];
{
  char inp[MAX_STRLEN],out[MAX_STRLEN];
  short *buf;
  int qinp,qout,shift,dir;
  FILE *Fi, *Fo;
  int fi,fo;
#ifdef VMS
  char mrs[15]="mrs=512";
#endif
  struct stat info;
  long l,k;

  GET_PAR_S(1, "Input file: ......... ", inp);
  GET_PAR_I(2, "Input file is Q.",qinp);
  GET_PAR_S(3, "Output file: ........ ", out);
  GET_PAR_I(4, "Input file is Q.",qout);
  
  if (qout == qinp) 
    HARAKIRI("Makes no sense a shift of 0 ... \n",7)
  else if (qout > qinp) dir=RIGHT;
  else dir=LEFT; 

  l= qout-qinp; 
  if (l<0) l= -l;

  for (shift = 1, k=0; k < l; k++) shift *=2;

  if ((Fi = fopen(inp,RB)) == NULL) KILL (inp,2); fi=fileno(Fi);
  if ((Fo = fopen(out,WB)) == NULL) KILL (inp,3); fo=fileno(Fo);

  stat(inp,&info);
  if ((buf=(short *)malloc(info.st_size))==NULL) 
    HARAKIRI ("Can't alloc inp\n",4);
  l=info.st_size/2;

  fprintf(stderr, "%s: Reading, ",inp);
  for (k=0; k<l; k+=256)
    if (read(fi,&buf[k],512)<0) KILL (inp,5);

  fprintf(stderr, "shifting, ");
  if (dir==RIGHT) 
    for (k=0; k<l; k++) buf[k] = (buf[k]*shift);
  else 
    for (k=0; k<l; k++) buf[k] = (buf[k]/shift);

  fprintf(stderr, "and writing ... ");
  for (k=0; k<l; k+=256)
    if (write(fo,&buf[k],512)<=0) KILL (inp,6);

  fprintf(stderr, "Done!\n");
  fclose(Fi); fclose(Fo);
}
