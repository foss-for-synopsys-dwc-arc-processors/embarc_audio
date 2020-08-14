/*
* Copyright 2019-2020, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include "cvsd.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __ARC
#include <stdfix.h>
#endif

#include "filter.h"

#ifdef NATIVE_CYCLE_PROFILING
#include "arc_profile.h"
#include "helper_lib.h"
#else
#include "string.h"
#include "stdio.h"
#endif


#define MAX_NUMBER_FRAMES 0x7FFFFFFF
#define CVSD_FRAME_SIZE 256
#define MAX_STR_LEN 1024

#ifdef NATIVE_CYCLE_PROFILING
static inline void init_native_cycle_profiling(int enc, int dec, int frame_len, int bit_rate, int sample_rate, Application_Settings_t *app_settings) {
    profile_preprocess(app_settings);

    app_settings->stream_config.stream_name = app_settings->input_file_name;
    app_settings->stream_config.stream_type = STREAM_TYPE_PCM_NONINTERLEAVED;
    app_settings->stream_config.sample_rate = sample_rate;
    app_settings->stream_config.sample_size = 2;
    app_settings->stream_config.bit_rate = bit_rate;
    app_settings->stream_config.num_ch = 1;
    if (enc & dec) {
        app_settings->stream_config.component_class = ARC_API_CLASS_TRANSCODER;
        app_settings->stream_config.codec_instance_size = sizeof(cvsd_t) * 2; //instance_size;
    } else if (enc) {
        app_settings->stream_config.component_class = ARC_API_CLASS_ENCODER;
        app_settings->stream_config.codec_instance_size = sizeof(cvsd_t); //instance_size;
    } else {
        app_settings->stream_config.component_class = ARC_API_CLASS_DECODER;
        app_settings->stream_config.codec_instance_size = sizeof(cvsd_t); //instance_size;
    }

    app_settings->rt_stats.used_output_buffer = frame_len * sizeof(int16_t);
    app_settings->rt_stats.microseconds_per_frame = 1000000 * frame_len / app_settings->stream_config.sample_rate;
}

static inline void set_configurations_profile_frame_post_process(int bit_rate, int frame_len, 
        int sample_rate, int inp_block_size, int out_block_size, Application_Settings_t *app_settings) {
    app_settings->stream_config.bit_rate = bit_rate;
    app_settings->rt_stats.used_output_buffer = frame_len * out_block_size;
    app_settings->rt_stats.microseconds_per_frame = (uint64_t)1000000 * frame_len / sample_rate;
    app_settings->rt_stats.used_input_buffer = frame_len * inp_block_size; /* input buffer size */
}
#endif

/* print usage/help message */
void usage()
{
    printf("cvsd_example [options]\n");
    printf("  -u/-h   : print usage\n");
    printf("  -F <frame_len>     : number of samples, default: 256\n");
    printf("  -enc     : Encoder only\n");
    printf("  -dec     : Decoder only\n");
    printf("  -encdec     : Encoder and decoder\n");
    printf(" -i <InputFileName>     : Input file name\n");
    printf(" -o <OutputFileName>     : Output file name\n");
}

static inline void* calloc_buffer (void * src, int size, int n){
    if ((src = calloc(n, sizeof(size))) == NULL)
    {
        printf("Error in memory allocation!\n");
        exit(1);
    }
    return src;
}

int main(int argc, char *argv[])
{
    cvsd_t cvsd_e, cvsd_d; //cvsd struct
    long frame_len = CVSD_FRAME_SIZE, cur_frame = 0, n_samples = 0;
    /* File variables */
    char File_in[MAX_STR_LEN], File_out[MAX_STR_LEN];
    FILE *F_inp, *F_out;
    uint8_t *bit_buf = NULL;
    int16_t *inp_buf = NULL, *out_buf = NULL, *interpolate_buf = NULL, *decimate_buf = NULL;

#ifdef NATIVE_CYCLE_PROFILING
    int bit_rate = 64000;
    int sample_rate = 64000;
#endif
    int pow_M_L_factor = 3;

#ifdef NATIVE_CYCLE_PROFILING
    Application_Settings_t app_settings = {0};
    mem_set_context(&app_settings);
    profile_init(&app_settings, &argc, argv);
#endif

    char run_encoder = 0, run_decoder = 0;

    while (argc > 1 && argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-") == 0)
        {
            /* Oops, stop processing options! */
            break;
        }
        else if (strcmp(argv[1], "-F") == 0)
        {
            frame_len = atol(argv[2]);
            argv += 2;
            argc -= 2;
        }
        else if (strcmp(argv[1], "-i") == 0)
        {
            if (strlen(argv[2]) < MAX_STR_LEN)
            {
                strcpy(File_in, argv[2]);
                argv += 2;
                argc -= 2;
            }
            else
            {
                printf("Name of input file to large!\n");
                exit(1);
            }
        }
        else if (strcmp(argv[1], "-o") == 0)
        {
            if (strlen(argv[2]) < MAX_STR_LEN)
            {
                strcpy(File_out, argv[2]);
                argv += 2;
                argc -= 2;
            }
            else
            {
                printf("Name of input file to large!\n");
                exit(1);
            }
        }
        else if ((strcmp(argv[1], "-u") == 0) || (strcmp(argv[1], "-h") == 0))
        {
            usage();

            argv++;
            argc--;
        }
        else if (strcmp(argv[1], "-enc") == 0)
        {
            /* Run only the encoder */
            run_encoder = 1;
            run_decoder = 0;

            argv++;
            argc--;
        }
        else if (strcmp(argv[1], "-dec") == 0)
        {
            /* Run only the decoder */
            run_encoder = 0;
            run_decoder = 1;

            argv++;
            argc--;
        }
        else if (strcmp(argv[1], "-encdec") == 0)
        {
            /* Run encoder and decoder */
            run_encoder = 1;
            run_decoder = 1;

            argv++;
            argc--;
        }
    }

    printf("_Frame of length: .................. %ld\n", frame_len);
    printf("_Input File: .................. %s \n", File_in);
    printf("_Output File: ................. %s \n", File_out);

    int out_len_interpolate = frame_len << pow_M_L_factor;

    /*-----Checking I/O files-----*/
#ifdef NATIVE_CYCLE_PROFILING
    memmove(app_settings.input_file_name, File_in, strlen(File_in) + 1);
    app_settings.stream_config.stream_name = app_settings.input_file_name;
#endif
    if ((F_inp = fopen(File_in, "rb")) == NULL)
    {
        perror(File_in);
        exit(2);
    }
    if ((F_out = fopen(File_out, "wb")) == NULL)
    {
        perror(File_out);
        exit(3);
    }
#ifdef NATIVE_CYCLE_PROFILING
    memmove(app_settings.output_file_name, File_out, strlen(File_out) + 1);
#endif

#ifdef NATIVE_CYCLE_PROFILING
    init_native_cycle_profiling(run_encoder, run_decoder, frame_len, bit_rate, sample_rate, &app_settings);
#endif

    /*------MEMORY ALLOCATION------*/
    int buf_size_FIR_assumpt = frame_len + FIR_FILTER_LENGTH;
    int out_len_interp_FIR_assumpt = out_len_interpolate + FIR_FILTER_LENGTH;
    bit_buf = (uint8_t *)calloc_buffer(bit_buf, sizeof(uint8_t), frame_len);
    if (run_encoder & run_decoder) {
        inp_buf = (int16_t *)calloc_buffer(inp_buf, sizeof(int16_t), buf_size_FIR_assumpt);
        out_buf = (int16_t *)calloc_buffer(out_buf, sizeof(int16_t), out_len_interp_FIR_assumpt);
        interpolate_buf = (int16_t *)calloc_buffer(interpolate_buf, sizeof(int16_t), out_len_interpolate);
        decimate_buf = (int16_t *)calloc_buffer(decimate_buf, sizeof(int16_t), frame_len);
    } else if (run_encoder) {
        inp_buf = (int16_t *)calloc_buffer(inp_buf, sizeof(int16_t), buf_size_FIR_assumpt);
        interpolate_buf = (int16_t *)calloc_buffer(interpolate_buf, sizeof(int16_t), out_len_interpolate);
    } else {
        out_buf = (int16_t *)calloc_buffer(out_buf, sizeof(int16_t), out_len_interp_FIR_assumpt);
        decimate_buf = (int16_t *)calloc_buffer(decimate_buf, sizeof(int16_t), frame_len);
    }

    /*------CODEC INITIALIZATION------*/
    if (cvsdInit(&cvsd_e)) {
        printf("incorrect initialization of CVSD!\n");
        exit(1);
    }

    if (cvsdInit(&cvsd_d)) {
        printf("incorrect initialization of CVSD!\n");
        exit(1);
    }

    /*-----PROCESSING ACCORDING TO CVSD-----*/
    int16_t *inp_buf_shift = (int16_t *)(inp_buf + FIR_FILTER_LENGTH);
    int16_t *out_buf_shift = (int16_t *)(out_buf + FIR_FILTER_LENGTH);
    for (cur_frame = 0; cur_frame < MAX_NUMBER_FRAMES; cur_frame++) {
        /* Read a block of samples */
        if (run_encoder && run_decoder) {
            memmove(inp_buf, (int16_t *)(inp_buf + n_samples), FIR_FILTER_LENGTH * sizeof(inp_buf[0]));
            /* Read a block of uncoded samples */
            if ((n_samples = fread(inp_buf_shift, sizeof(int16_t), frame_len, F_inp)) <= 0)
                break;
        } else if(run_decoder) {
            memmove(out_buf, (int16_t *)(out_buf + out_len_interpolate), FIR_FILTER_LENGTH * sizeof(out_buf[0]));
            /* Read a block of uncoded samples */
            if ((n_samples = fread(bit_buf, sizeof(uint8_t), frame_len, F_inp)) <= 0)
                break;
        } else {
            memmove(inp_buf, (int16_t *)(inp_buf + n_samples), FIR_FILTER_LENGTH * sizeof(inp_buf[0]));
            /* Read a block of uncoded samples */
            if ((n_samples = fread(inp_buf_shift, sizeof(int16_t), frame_len, F_inp)) <= 0)
                break;
        }

        /*
        * 8x-upsampler and 8x-downsampler are parts of test application and provided as examples only.
        * FIR filter parameters coefficients are designed per Bluetooth Core 5.1 spectral mask requirements
        * and provided as example also. Test application processes 8 kHz sampled signal at input and provides
        * 8 kHz samples signal as output.
        */

        /* CODEC OPERATION */
        if (run_encoder && run_decoder)
        {
            /* Run both and save decoded samples */
            interpolation_x8(inp_buf, buf_size_FIR_assumpt, interpolate_buf, out_len_interpolate);
#ifdef NATIVE_CYCLE_PROFILING
                if (!profile_frame_preprocess(&app_settings))
                    break;
#endif //NATIVE_CYCLE_PROFILING
            cvsdEncode(&cvsd_e, (const int16_t *)interpolate_buf, out_len_interpolate, (uint32_t *)bit_buf);
            memmove(out_buf, (out_buf + out_len_interpolate), FIR_FILTER_LENGTH * sizeof(out_buf[0]));
            cvsdDecode(&cvsd_d, (const uint8_t *)bit_buf, n_samples, (int16_t *)out_buf_shift);
#if defined(NATIVE_CYCLE_PROFILING)
            set_configurations_profile_frame_post_process(bit_rate, out_len_interpolate, sample_rate,
                        sizeof(int16_t), sizeof(int16_t), &app_settings);
            if (!profile_frame_postprocess(&app_settings))
                break;
            fprintf(stderr, "\r[%4ld] %5.2f MHz\n", cur_frame,
                    (float)app_settings.profiling_data.cycles_per_frame / app_settings.profiling_data.mksec_per_frame);
#endif //NATIVE_CYCLE_PROFILING
            decimation_x8(out_buf, out_len_interp_FIR_assumpt, decimate_buf, n_samples);

            /* Save samples to file */
            if (!(n_samples = fwrite(decimate_buf, sizeof(int16_t), n_samples, F_out)))
                break;
        } else if (run_decoder) {
#ifdef NATIVE_CYCLE_PROFILING
                if (!profile_frame_preprocess(&app_settings))
                    break;
#endif //NATIVE_CYCLE_PROFILING
            cvsdDecode(&cvsd_d, (const uint8_t *)bit_buf, n_samples, (int16_t *)out_buf_shift);
#if defined(NATIVE_CYCLE_PROFILING)
            set_configurations_profile_frame_post_process(bit_rate, out_len_interpolate, sample_rate,
                        sizeof(uint8_t), sizeof(int16_t), &app_settings);
            if (!profile_frame_postprocess(&app_settings))
                break;
            fprintf(stderr, "\r[%4ld] %5.2f MHz\n", cur_frame,
                    (float)app_settings.profiling_data.cycles_per_frame / app_settings.profiling_data.mksec_per_frame);
#endif //NATIVE_CYCLE_PROFILING
            decimation_x8(out_buf, out_len_interp_FIR_assumpt, decimate_buf, n_samples);

            /* Save samples to file */
            if (!(n_samples = fwrite(decimate_buf, sizeof(int16_t), n_samples, F_out)))
                break;
        }
        else if (run_encoder) {
            interpolation_x8(inp_buf, buf_size_FIR_assumpt, interpolate_buf, out_len_interpolate);
#ifdef NATIVE_CYCLE_PROFILING
                if (!profile_frame_preprocess(&app_settings))
                    break;
#endif //NATIVE_CYCLE_PROFILING
            cvsdEncode(&cvsd_e, (const int16_t *)interpolate_buf, out_len_interpolate, (uint32_t *)bit_buf);
#if defined(NATIVE_CYCLE_PROFILING)
            set_configurations_profile_frame_post_process(bit_rate, out_len_interpolate, sample_rate,
                        sizeof(int16_t),sizeof(uint8_t), &app_settings);
            if (!profile_frame_postprocess(&app_settings))
                break;
            fprintf(stderr, "\r[%4ld] %5.2f MHz\n", cur_frame,
                    (float)app_settings.profiling_data.cycles_per_frame / app_settings.profiling_data.mksec_per_frame);
#endif //NATIVE_CYCLE_PROFILING

            /* Save samples to file */
            if (!(n_samples = fwrite(bit_buf, sizeof(uint8_t), n_samples, F_out)))
                break;
        }
    } //for (cur_frame = 0; cur_frame < MAX_NUMBER_FRAMES; cur_frame++) 

#ifdef NATIVE_CYCLE_PROFILING
    profile_postprocess(&app_settings);
#endif
    free(bit_buf);
    if (run_encoder & run_decoder) {
        free(inp_buf);
        free(out_buf);
        free(interpolate_buf);
        free(decimate_buf);
    } else if (run_encoder) {
        free(inp_buf);
        free(interpolate_buf);
    } else {
        free(out_buf);
        free(decimate_buf);
    }
    fclose(F_out);
    fclose(F_inp);
    printf("results wrtten to %s\n", File_out);
    printf("done.\n");

    return 0;
}
