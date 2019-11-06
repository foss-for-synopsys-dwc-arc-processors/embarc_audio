/*                                                        02.Feb.2010 v3.3
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	G711DEMO.C

	Description:
	~~~~~~~~~~~~
	Example-program for converting from linear samples to A/u law log
	compression and vice-versa, according to ITU-T Rec.G711.

	Usage:
	~~~~~~
	$ g711demo [-?] [-r] [-skip skip] Law Transf InpFile OutFile
						 [BlockSize [1stBlock [NoOfBlocks]]]

	where:
	Law	      is the law desired (either A or u)
	Transf      is the desired convertion on the input file:
							[lili], linear to linear: lin -> (A/u)log -> lin
							[lilo], linear to (A/u)-log
							[loli], (A/u) log to linear
	InpFile     is the name of the file to be processed;
	OutFile     is the name with the compressed/expanded data;
	BlockSize   is the block size, in number of samples (16 -bit words)
							(default is 256);
	1stBlock    is the number of the first block of the input file
							to be processed. (Default: 1)
	NoOfBlocks  is the number of blocks to be processed, starting on
							block "1stBlock". (default is til end-of-file)

	Options:
	-?            display usage and quit.
	-r            disables even-bit swap by A-law encoding and decoding.
								By default, even bits of the A-law samples ARE inverted
								by alaw_compress() at return time, as well as by
								alaw_expand() at its beginning. With the option
								"-r", the log samples have NOT their even bits
								inverted before being saved to disk NOR after
								read from a file.
	-skip         is the number of samples to skip before the beginning
								of the 1st block.

	Example:
	$ G711 u lili voice.ref voice.rel 256 3 45

	The command above takes the samples in file "voice.ref" and converts
	them to A-law and back to linear, saving them into file "voice.rel",
	starting at block 3 for 45 blocks, each block being 256 samples wide.

	Variables:
	~~~~~~~~~~
	law              law to use (either A or u)
	conv              desired processing
	inpfil              input file name;
	outfil              output file name;
	N              block size;
	N1              first block to filter;
	N2              no. of blocks to filter;

	Compilation of Demo Program:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	The test program allows to process binary/ILS files of any size by
	loading segments of data from/to file. The size of the working
	segment must be entered.

	VAX/VMS:
	$ CC G711
	$ link G711
	$ G711 :== $G711_Disk:[G711_Dir]G711
	$ G711 u lili ifile ofile 233 3 47

	This command line invokes the u-law utility, converts from linear
	over ulaw to linear. It processes the input file "ifile" by
	loading segments of 233 samples and writes the result to "ofile",
	from the 3rd segment, for 47 segments.
	
	Turbo-C, Turbo-C++:
	> tcc G711
	> G711 a lilo ifile ofile 39 1

	This command line invokes the a-law utility, converts from linear
	to a-law.  It processes the input file "ifile" by loading
	segments of 39 samples and writes the result (alaw-samples) to
	"ofile", starting from the beginning of the file. It will ask
	the number of segments.

	HighC (MetaWare, version R2.32):
	> hc386 G711.c
	> Run386 G711 a loli ifile ofile 3333

	This command line invokes the a-law utility, converts from a-law
	to linear It processes the input file "ifile" (alaw-samples) by
	loading segments of 3333 samples and writes the result (linear
	samples) to "ofile". It will ask the starting segment and the
	number of segments.

	SunC (SunOS - BSD Unix)
	# cc -o g711demo g711demo.c
	# g711demo a lilo ifile ofile 256 1 132

	Exit values:
	~~~~~~~~~~~~
	 0   success (all but VMS);
	 1   success (only in VMS);
	 2   error opening input file;
	 3   error creating output file;
	 4   error moving pointer to desired start of conversion;
	 5   error reading input file;
	 6   error writing to file;
	 7   invalid law
	 8   invalid conversion
	10   error allocating memory

	Prototypes:
	~~~~~~~~~~~
	Needs ugstdemo.h and includes g711.h.

	Authors:
	~~~~~~~~
	Simao Ferraz de Campos Neto         Rudolf Hofmann
	CPqD/Telebras                       PHILIPS KOMMUNIKATIONS INDUSTRIE AG
	DDS/Pr.11                           Kommunikationssysteme
	Rd. Mogi Mirim-Campinas Km.118      Thurn-und-Taxis-Strasse 14
	13.080-061 - Campinas - SP (Brazil) D-8500 Nuernberg 10 (Germany)

	Phone : +55-192-39-6637             Phone : +49-911-526-2603
	FAX   : +55-192-39-2179             FAX   : +49-911-526-3385
	EMail : tdsimao@cpqd.ansp.br        EMail : hf@pkinbg.uucp

	History:
	~~~~~~~~
	07.Feb.1991 v1.0 Release of 1st demo program for G711 module (CPqD).
	10.Dec.1991 v2.0 Demo program incorporated in G711 module and
									 rebuild by PKI.
	08.Feb.1992 v2.1 Demo separated from module file; ILS header
									 manipulation removed; low level I/O; changes to
									 assure compatibility with Unix (SunOS).
	14.Apr.1993 v3.0 Inclusion of display_usage() and of option "-r"
									 <tdsimao@venus.cpqd.ansp.br>
	22.Feb.1996 v3.1 Removed compilation warnings, included headers as
									 suggested by Kirchherr (FI/DBP Telekom) to run under
									 OpenVMS/AXP <simao@ctd.comsat.com>
	11.Apr.2000 v3.2 Corrected bug that made incorrect calculation on
									 total number of blocks to process when the block
									 size is not a multiple of the file
									 size. <simao.campos@labs.comsat.com>
	02.Feb.2010 v3.3 Modified maximum string length (y.hiwasaki)
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ugstdemo.h"		/* UGST defines for demo programs */

/* Include general headers */
#include <stdio.h>		/* UNIX Standard I/O Definitions */
#include <ctype.h>		/* Character Type Classification */
#include <string.h>		/* String handling functions */
#include <stdlib.h>		/* General utility definitions */
#include <math.h>



/* G711 module functions */
#include "g711.h"

/* ..... Native profiling module defenitions ..... */
#ifdef NATIVE_CYCLE_PROFILING
#include "arc_profile.h"
#include "helper_lib.h"
#endif
/*
	--------------------------------------------------------------------------
	 void display_usage()

	 Displays usage of the program. Upon completion, aborts the program.

	 Created by <tdsimao@cpqd.ansp.br>, 14.Apr.93

	--------------------------------------------------------------------------
*/
#define FP(x) fprintf(stderr, x)
void            display_usage()
{
	FP("\n  G711DEMO.C   --- Version v3.3 of 02.Feb.2010 \n");
	FP("\n");
	FP("  Description:\n");
	FP("  ~~~~~~~~~~~~\n");
	FP("  Example-program for converting from linear samples to A/u law log\n");
	FP("  compression and vice-versa, according to ITU-T Rec.G711.\n");
	FP("\n");
	FP("  Usage:\n");
	FP("  ~~~~~~\n");
	FP("  $ G711 [-r] Law Transf InpFile OutFile BlkSize 1stBlock NoOfBlocks\n");
	FP("\n");
	FP("  where:\n");
	FP("  Law	   is the law desired (either A or u)\n");
	FP("  Transf	   is the desired convertion on the input file:\n");
	FP("	             [lili], linear to linear: lin -> (A/u)log -> lin\n");
	FP("               [lilo], linear to (A/u)-log\n");
	FP("               [loli], (A/u) log to linear\n");
	FP("  InpFile	   is the name of the file to be processed;\n");
	FP("  OutFile	   is the name with the compressed/expanded data;\n");
	FP("  BlkSize    is number of samples per block [256];\n");
	FP("  1stBlock   is the number of the first block of the input file\n");
	FP("		   to be processed [1].\n");
	FP("  NoOfBlocks is the number of blocks to be processed, starting on\n");
	FP("		   block `1stBlock'. Default is til end-of-file.\n");
	FP("\n");
	FP("  Options:\n");
	FP("  -?         displays this message.\n");
	FP("  -r         disables even-bit swap by A-law encoding and decoding.\n");
	FP("  -skip      is the number of samples to skip.\n");
	FP("\n");

	/* Quit program */
	exit(1);
}
#undef FP
/* ..................... End of display_usage() .......................... */


/*
	 **************************************************************************
	 ***                                                                    ***
	 ***        Demo-Program for testing the correct implementation         ***
	 ***               and to show how to use the programs                  ***
	 ***                                                                    ***
	 **************************************************************************
*/
#if defined(NATIVE_CYCLE_PROFILING)
Application_Settings_t app_settings = {0};
int profile_frame_postprocess_(int bit_rate, int numb_samples,int sample_rate, int frame) {
		app_settings.stream_config.bit_rate = bit_rate;
		app_settings.rt_stats.used_output_buffer = numb_samples * sizeof(short);
		app_settings.rt_stats.microseconds_per_frame = (uint64_t)1000000 * numb_samples / sample_rate;
		app_settings.rt_stats.used_input_buffer = numb_samples * sizeof(short);//INBUFFSZ =  LPCSTARTINDEX + A_LEN /* input buffer size */
		if(!profile_frame_postprocess(&app_settings))
				return 1;
		fprintf(stderr, "\r[%4d] %5.2f MHz", frame,
						(float) app_settings.profiling_data.cycles_per_frame
								/ app_settings.profiling_data.mksec_per_frame);
												printf("\n");
		return 0;
}
#endif //NATIVE_CYCLE_PROFILING

int main(argc, argv)
	int             argc;
	char           *argv[];
{
	long            i, N, N1, N2, smpno, tot_smpno, cur_blk;
	short          *lin_buff;	/* linear input samples */
	short          *log_buff;	/* compressed data */
	short          *lon_buff;	/* quantized output samples */
	char            inpfil[MAX_STRLEN], outfil[MAX_STRLEN];
	FILE           *Fi, *Fo;
	int             inp, out;
	char            law[MAX_STRLEN], lilo[MAX_STRLEN];
	short           inp_type, out_type;
	char            revert_even_bits = 1;
#ifndef NATIVE_CYCLE_PROFILING
	clock_t         t1, t2;	/* aux. for CPU-time measurement */
#endif
#ifdef VMS
	char            mrs[15];	/* for correct mrs in VMS environment */
#endif
	long            start_byte, skip = 0;
#ifdef NATIVE_CYCLE_PROFILING
	mem_set_context(&app_settings);
	profile_init(&app_settings, &argc, argv);
	const long nSampleRate=8000L;
#endif
/*
 * GETTING PARAMETERS
 */
	/* Get options */
	if (argc < 2)
		display_usage();
	else
	{
		while (argc > 1 && argv[1][0] == '-')
			if (argv[1][1] == 'r')
			{
	/* Disable revertion of even bits */
	revert_even_bits = 0;

	/* Move argv over the option to the 1st mandatory argument */
	argv++;

	/* Update argc */
	argc--;
			}
			else if (strcmp(argv[1], "-skip") == 0)
			{
	/* Get skip length */
	skip = atol(argv[2]);

	/* Check bounds */
	if (skip < 0)
	{
		fprintf(stderr, "Skip has to be > 0!\n");
		exit(10);
	}

	/* Move argv over the option to the next argument */
	argv += 2;

	/* Update argc */
	argc -= 2;
			}
			else if (argv[1][1] == '?')
			{
	/* Display intructions */
	display_usage();
			}
			else
			{
	fprintf(stderr, "ERROR! Invalid option \"%s\" in command line\n\n",
		argv[1]);
	display_usage();
			}
	}


	/* Get parameters */
	GET_PAR_S(1, "_Law (A, u): ................. ", law);
	GET_PAR_S(2, "_Transf (lili,lilo,loli): .... ", lilo);
	GET_PAR_S(3, "_File to be converted: ....... ", inpfil);
	GET_PAR_S(4, "_Output File: ................ ", outfil);
	FIND_PAR_L(5, "_Block Length: ............... ", N, 256);
	FIND_PAR_L(6, "_Start Block: ................ ", N1, 1);
	FIND_PAR_L(7, "_No. of Blocks: .............. ", N2, 0);


	/* ......... SOME INITIALIZATIONS ......... */
	--N1;

	/* Classification of the conversion desired */
	inp_type = toupper(lilo[1]) == 'O' ? IS_LOG : IS_LIN;
	out_type = toupper(lilo[3]) == 'O' ? IS_LOG : IS_LIN;
	if ((out_type == IS_LOG) && (inp_type == IS_LOG))
		HARAKIRI("log. to log. makes no sense! Aborted...\n", 8);

	/* Classification of law */
	law[0] = toupper(law[0]);
	if ((law[0] != (char) 'A') && (law[0] != (char) 'U'))
		HARAKIRI(" Invalid law!\n", 7);

	/* .......... ALLOCATION OF BUFFERS .......... */
	if ((lin_buff = (short *) calloc(N, sizeof(short))) == NULL)
		HARAKIRI("Can't allocate memory for input buffer\n", 10);

	if ((log_buff = (short *) calloc(N, sizeof(short))) == NULL)
		HARAKIRI("Can't allocate memory for output buffer\n", 10);

	if ((lon_buff = (short *) calloc(N, sizeof(short))) == NULL)
		HARAKIRI("Can't allocate memory for temporary buffer\n", 10);
	/* .......... FILE OPERATIONS .......... */

#ifdef VMS
	sprintf(mrs, "mrs=%d", 2 * N);
#endif

	/* Open input file */
#ifdef NATIVE_CYCLE_PROFILING
	memcpy(app_settings.input_file_name, inpfil, strlen(inpfil)+1);
	app_settings.stream_config.stream_name = app_settings.input_file_name;
#endif
	if ((Fi = fopen(inpfil, RB)) == NULL)
		KILL(inpfil, 2);
	inp = fileno(Fi);

	/* Open (create) output file */
#ifdef NATIVE_CYCLE_PROFILING
	memcpy(app_settings.output_file_name, outfil, strlen(outfil)+1);
#endif
	if ((Fo = fopen(outfil, WB)) == NULL)
		KILL(outfil, 3);
	out = fileno(Fo);

	/* Define starting byte in file */
	start_byte = (N1 * N + skip) * sizeof(short);

	/* ... and move file's pointer to 1st desired block */
	if (fseek(Fi, (N1 * N + skip) * sizeof(short), 0) < 0l)
		KILL(inpfil, 4);

	/* Check whether is to process til end-of-file */
	if (N2 == 0)
	{
		long st_size = (long)_filelength(_fileno(Fi));
		/* ... hey, need to skip the delayed samples! ... */
		N2 = ceil((st_size - start_byte) / (double)(N * sizeof(short)));
	}

#ifdef NATIVE_CYCLE_PROFILING
			profile_preprocess(&app_settings);

			app_settings.stream_config.stream_name = app_settings.input_file_name;
			app_settings.stream_config.stream_type = STREAM_TYPE_PCM_NONINTERLEAVED;
			if (inp_type == IS_LIN && out_type == IS_LOG)
				app_settings.stream_config.component_class = ARC_API_CLASS_ENCODER;
			else if (inp_type == IS_LIN && out_type == IS_LIN)
				app_settings.stream_config.component_class = ARC_API_CLASS_TRANSCODER;
			else if (inp_type == IS_LOG)
				app_settings.stream_config.component_class = ARC_API_CLASS_DECODER;
			app_settings.stream_config.sample_rate = nSampleRate;
			app_settings.stream_config.sample_size = 2;
			app_settings.stream_config.bit_rate = 96*1024;
			app_settings.stream_config.num_ch = 1;
			app_settings.stream_config.codec_instance_size = 7 * sizeof(long) + 4 * sizeof(short) + 3 * sizeof(char) + 2 * sizeof(int) + sizeof(FILE);
#endif
/*
 * ......... COMPRESSION/EXPANSION .........
 */
#ifndef NATIVE_CYCLE_PROFILING
	t1 = clock();			/* measure CPU-time */
#endif

	tot_smpno = 0;

	switch (law[0])
	{
		/* ......... Process A-law rule ......... */
	case 'A':

		/* Input: LINEAR | Output: LOG */
		if (inp_type == IS_LIN && out_type == IS_LOG)
			for (tot_smpno = cur_blk = 0; cur_blk < N2; cur_blk++, tot_smpno += smpno)
			{
	if ((smpno = fread(lin_buff, sizeof(short), N, Fi)) < 0)
		KILL(inpfil, 5);
#ifdef NATIVE_CYCLE_PROFILING
			if(!profile_frame_preprocess(&app_settings))
				break;
#endif //NATIVE_CYCLE_PROFILING
	alaw_compress(smpno, lin_buff, log_buff);
#if defined(NATIVE_CYCLE_PROFILING)
	if (profile_frame_postprocess_(96*1024, smpno, nSampleRate, cur_blk))
		break;
#endif //NATIVE_CYCLE_PROFILING
	if (!revert_even_bits)
		for (i = 0; i < smpno; i++)
			log_buff[i] ^= 0x0055;

	if ((smpno = fwrite(log_buff, sizeof(short), smpno, Fo)) < 0)
		KILL(outfil, 6);
			}

		/* Input: LINEAR | Output: LINEAR */
		else if (inp_type == IS_LIN && out_type == IS_LIN)
			for (tot_smpno = cur_blk = 0; cur_blk < N2; cur_blk++, tot_smpno += smpno)
			{
	if ((smpno = fread(lin_buff, sizeof(short), N, Fi)) < 0)
		KILL(inpfil, 5);
#ifdef NATIVE_CYCLE_PROFILING
			if(!profile_frame_preprocess(&app_settings))
				break;
#endif //NATIVE_CYCLE_PROFILING
	alaw_compress(smpno, lin_buff, log_buff);
	alaw_expand(smpno, log_buff, lon_buff);
#if defined(NATIVE_CYCLE_PROFILING)
	if (profile_frame_postprocess_(96*1024, smpno, nSampleRate, cur_blk))
		break;
#endif //NATIVE_CYCLE_PROFILING
	if ((smpno = fwrite(lon_buff, sizeof(short), smpno, Fo)) < 0)
		KILL(outfil, 6);
			}

		/* Input: LOG | Output: LINEAR */
		else if (inp_type == IS_LOG)
			for (tot_smpno = cur_blk = 0; cur_blk < N2; cur_blk++, tot_smpno += smpno)
			{
	if ((smpno = fread(log_buff, sizeof(short), N, Fi)) < 0)
		KILL(inpfil, 5);
	if (!revert_even_bits)
		for (i = 0; i < smpno; i++)
			log_buff[i] ^= 0x0055;
#ifdef NATIVE_CYCLE_PROFILING
			if(!profile_frame_preprocess(&app_settings))
				break;
#endif //NATIVE_CYCLE_PROFILING
	alaw_expand(smpno, log_buff, lon_buff);
#if defined(NATIVE_CYCLE_PROFILING)
	if (profile_frame_postprocess_(96*1024, smpno, nSampleRate, cur_blk))
		break;
#endif //NATIVE_CYCLE_PROFILING
	if ((smpno = fwrite(lon_buff, sizeof(short), smpno, Fo)) < 0)
		KILL(outfil, 6);
			}
		break;

		/* ......... Process u-law rule ......... */

	case 'U':
		/* Input: LINEAR | Output: LOG */
		if (inp_type == IS_LIN && out_type == IS_LOG)
			for (tot_smpno = cur_blk = 0; cur_blk < N2; cur_blk++, tot_smpno += smpno)
			{
	smpno = fread(lin_buff, sizeof(short), N, Fi);
#ifdef NATIVE_CYCLE_PROFILING
			if(!profile_frame_preprocess(&app_settings))
				break;
#endif //NATIVE_CYCLE_PROFILING
	ulaw_compress(smpno, lin_buff, log_buff);
#if defined(NATIVE_CYCLE_PROFILING)
	if (profile_frame_postprocess_(96*1024, smpno, nSampleRate, cur_blk))
		break;
#endif //NATIVE_CYCLE_PROFILING
	smpno = fwrite(log_buff, sizeof(short), smpno, Fo);
			}

		/* Input: LINEAR | Output: LINEAR */
		else if (inp_type == IS_LIN && out_type == IS_LIN)
			for (tot_smpno = cur_blk = 0; cur_blk < N2; cur_blk++, tot_smpno += smpno)
			{
	smpno = fread(lin_buff, sizeof(short), N, Fi);
#ifdef NATIVE_CYCLE_PROFILING
			if(!profile_frame_preprocess(&app_settings))
				break;
#endif //NATIVE_CYCLE_PROFILING
	ulaw_compress(smpno, lin_buff, log_buff);
	ulaw_expand(smpno, log_buff, lon_buff);
#if defined(NATIVE_CYCLE_PROFILING)
	if (profile_frame_postprocess_(96*1024, smpno, nSampleRate, cur_blk))
		break;
#endif //NATIVE_CYCLE_PROFILING
	smpno = fwrite(lon_buff, sizeof(short), smpno, Fo);
			}

		/* Input: LOG | Output: LINEAR */
		else if (inp_type == IS_LOG)
			for (tot_smpno = cur_blk = 0; cur_blk < N2; cur_blk++, tot_smpno += smpno)
			{
	smpno = fread(log_buff, sizeof(short), N, Fi);
#ifdef NATIVE_CYCLE_PROFILING
			if(!profile_frame_preprocess(&app_settings))
				break;
#endif //NATIVE_CYCLE_PROFILING
	ulaw_expand(smpno, log_buff, lon_buff);
#if defined(NATIVE_CYCLE_PROFILING)
	if (profile_frame_postprocess_(96*1024, smpno, nSampleRate, cur_blk))
		break;
#endif //NATIVE_CYCLE_PROFILING
	smpno = fwrite(lon_buff, sizeof(short), smpno, Fo);
			}
		break;
	}


	/* ......... FINALIZATIONS ......... */
#ifdef NATIVE_CYCLE_PROFILING
	profile_postprocess(&app_settings);
#else
	t2 = clock();
	printf("Speed: %f sec CPU-time for %ld processed samples\n",
	 (t2 - t1) / (double) CLOCKS_PER_SEC,
	 tot_smpno);
#endif
	fclose(Fi);
	fclose(Fo);
#ifndef VMS
	return (0);
#endif
}
