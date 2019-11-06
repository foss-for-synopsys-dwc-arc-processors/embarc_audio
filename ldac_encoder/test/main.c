/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include <stdio.h>
#include "ldaclib.h"
#include <stdlib.h>
#include "struct_ldac.h"
#include "ldac.h"
#include "wavlib.h"
#define MAX_STR_LEN 1024

#ifdef NATIVE_CYCLE_PROFILING
#include "arc_profile.h"
#include "helper_lib.h"
Application_Settings_t app_settings = {0};
#endif

void usage()
{
    printf("ldac_codec [options]\n");
    printf("  -u/-h   : print usage\n");
    printf("  -F <frame_len>     : number of samples per channel, must be in {165, 110, 82, 66, 55, 47, 41, 36, 33, 30, 27, 25, 23} 66 on default.\n");
    printf(" -i <InputFileName>     : Input file name\n");
    printf(" -o <OutputFileName>     : Output file name\n");
}

void set_config_from_wav(wav_instance_t *inst, int *sfid, int *cci, LDAC_SMPL_FMT_T *fmt)
{
    wav_header_t *header = (wav_header_t *)inst->wav_header;
    //Choose sample rate ID depending on the sampling rate
    switch (header->samplerate)
    {
    case 44100:
        *sfid = LDAC_SMPLRATEID_0;
        break;
    case 48000:
        *sfid = LDAC_SMPLRATEID_1;
        break;
    case 88200:
        *sfid = LDAC_SMPLRATEID_2;
        break;
    case 96000:
        *sfid = LDAC_SMPLRATEID_3;
        break;
    default:
        printf("Invalid sample rate in wav file!\n");
        exit(1);
        break;
    }
    //Choose Channel config ID depending on the number of channels
    switch (header->number_of_channels)
    {
    case 1:
        *cci = LDAC_CHCONFIGID_MN;
        break;
    case 2:
        *cci = LDAC_CHCONFIGID_ST;
        break;
    default:
        printf("Invalid number of channels in wav file!\n");
        exit(1);
        break;
    }
    //Choose Sample format depending on the sample size
    switch (header->samplesize)
    {
    case 16:
        *fmt = LDAC_SMPL_FMT_S16;
        break;
    case 24:
        *fmt = LDAC_SMPL_FMT_S24;
        break;
    case 32:
        if (header->fmt_code == 3)
        {
#ifdef _32BIT_FIXED_POINT
            printf("Unsupported format of samples.\n");
            exit(1);
#endif /* _32BIT_FIXED_POINT */
            *fmt = LDAC_SMPL_FMT_F32;
        }
        else
        {
            *fmt = LDAC_SMPL_FMT_S32;
        }
        break;
    default:
        printf("Invalid sample size in wav file!\n");
        exit(1);
        break;
    }
}

/* Split LR interleaved PCM into buffer for LDAC encoder */
static void prepare_pcm_encode(void *pbuff, char **ap_pcm, int nsmpl, int nch, LDAC_SMPL_FMT_T fmt)
{
    int i;
    if (nch == 2)
    {
        if (fmt == LDAC_SMPL_FMT_S16)
        {
            short *p_pcm_16 = (short *)pbuff;
            short *p_lch_16 = (short *)ap_pcm[0];
            short *p_rch_16 = (short *)ap_pcm[1];
            for (i = 0; i < nsmpl; i++)
            {
                *p_lch_16++ = p_pcm_16[0];
                *p_rch_16++ = p_pcm_16[1];
                p_pcm_16 += 2;
            }
        }
        else if (fmt == LDAC_SMPL_FMT_S24)
        {
            char *p_pcm_8 = (char *)pbuff;
            char *p_lch_8 = (char *)ap_pcm[0];
            char *p_rch_8 = (char *)ap_pcm[1];
            for (i = 0; i < nsmpl; i++)
            {
                *p_lch_8++ = p_pcm_8[0];
                *p_lch_8++ = p_pcm_8[1];
                *p_lch_8++ = p_pcm_8[2];
                p_pcm_8 += 3;
                *p_rch_8++ = p_pcm_8[0];
                *p_rch_8++ = p_pcm_8[1];
                *p_rch_8++ = p_pcm_8[2];
                p_pcm_8 += 3;
            }
        }
        else if (fmt == LDAC_SMPL_FMT_S32)
        {
            char *p_pcm_8 = (char *)pbuff;
            char *p_lch_8 = (char *)ap_pcm[0];
            char *p_rch_8 = (char *)ap_pcm[1];
            for (i = 0; i < nsmpl; i++)
            {
                *p_lch_8++ = p_pcm_8[0];
                *p_lch_8++ = p_pcm_8[1];
                *p_lch_8++ = p_pcm_8[2];
                *p_lch_8++ = p_pcm_8[3];
                p_pcm_8 += 4;
                *p_rch_8++ = p_pcm_8[0];
                *p_rch_8++ = p_pcm_8[1];
                *p_rch_8++ = p_pcm_8[2];
                *p_rch_8++ = p_pcm_8[3];
                p_pcm_8 += 4;
            }
        }
        else if (fmt == LDAC_SMPL_FMT_F32)
        {
            float *p_pcm = (float *)pbuff;
            float *p_lch = (float *)ap_pcm[0];
            float *p_rch = (float *)ap_pcm[1];
            for (i = 0; i < nsmpl; i++)
            {
                *p_lch++ = p_pcm[0];
                p_pcm++;
                *p_rch++ = p_pcm[0];
                p_pcm++;
            }
        }
    }
    else if (nch == 1)
    {
        switch (fmt)
        {
        case LDAC_SMPL_FMT_S16:
            copy_data_ldac(pbuff, ap_pcm[0], 2 * nsmpl);
            break;
        case LDAC_SMPL_FMT_S24:
            copy_data_ldac(pbuff, ap_pcm[0], 3 * nsmpl);
            break;
        case LDAC_SMPL_FMT_S32:
        case LDAC_SMPL_FMT_F32:
            copy_data_ldac(pbuff, ap_pcm[0], 4 * nsmpl);
            break;
        default:
            break;
        }
    }
}

void wrap_encoding(HANDLE_LDAC hLDAC, char **pp_pcm, LDAC_SMPL_FMT_T fmt, unsigned char *p_ldac_transport_frame, int *frmlen_wrote, unsigned char *a_frm_header)
{
    LDAC_RESULT result;
    result = ldaclib_encode(hLDAC, pp_pcm, fmt, p_ldac_transport_frame + LDAC_FRMHDRBYTES, frmlen_wrote);
    if (LDAC_FAILED(result))
    {
        printf("Failed to encode frame.\n");
        exit(1);
    }

    if (*frmlen_wrote == 0)
    {
        result = ldaclib_flush_encode(hLDAC, fmt, p_ldac_transport_frame + LDAC_FRMHDRBYTES, frmlen_wrote);

        if (LDAC_FAILED(result))
        {
            printf("Failed to flush encoded.\n");
            exit(1);
        }
    }

    if (*frmlen_wrote > 0)
    {
        for (int i = 0; i < LDAC_FRMHDRBYTES + 2; i++)
        {
            a_frm_header[i] = 0;
        }
        int sfid, cci, frmlen, frm_status;
        result = ldaclib_get_config_info(hLDAC, &sfid, &cci, &frmlen, &frm_status);
        result = ldaclib_set_frame_header(hLDAC, a_frm_header, sfid, cci, frmlen, frm_status);
        copy_data_ldac(a_frm_header, p_ldac_transport_frame, LDAC_FRMHDRBYTES);
        *frmlen_wrote += LDAC_FRMHDRBYTES;
    }
}

int main(int argc, char *argv[])
{
    wav_instance_t instance;
    uint32_t read_size;
    //set config info
    LDAC_RESULT result;
    HANDLE_LDAC hLDAC = ldaclib_get_handle();
    int sfid, cci, frmlen, frm_status = 0;
    LDAC_SMPL_FMT_T fmt;
    const int a_cci_nch[] = {1, 2, 2};
    int frmlen_1ch = 66; //{165, 110, 82, 66, 55, 47, 41, 36, 33, 30, 27, 25, 23}
    char File_in[MAX_STR_LEN], File_out[MAX_STR_LEN];

#if defined(NATIVE_CYCLE_PROFILING)
    mem_set_context(&app_settings);
    profile_init(&app_settings, &argc, argv);
#endif

    while (argc > 1 && argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-") == 0)
        {
            /* Oops, stop processing options! */
            break;
        }
        else if (strcmp(argv[1], "-F") == 0)
        {
            frmlen_1ch = atoi(argv[2]);
            argv += 2;
            argc -= 2;
        }
        else if (strcmp(argv[1], "-i") == 0)
        {
            if (strlen(argv[2]) < MAX_STR_LEN)
            {
                strcpy(File_in, argv[2]);
#ifdef NATIVE_CYCLE_PROFILING
                memcpy(app_settings.input_file_name, File_in, strlen(File_in) + 1);
#endif
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
#ifdef NATIVE_CYCLE_PROFILING
                memcpy(app_settings.output_file_name, File_out, strlen(File_out) + 1);
#endif
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
        else
        {
            //skip invalid flags
            argv += 2;
            argc -= 2;
        }
    }

    if (rd_wav_open(&instance, File_in, "rb", 0) != WAV_ERR_NONE)
    {
        printf("Couldn't open wav file.\n");
        exit(1);
    }
    uint32_t file_size = get_file_size_from_instance(&instance);
    set_config_from_wav(&instance, &sfid, &cci, &fmt);
    FILE *out = fopen(File_out, "wb");
    if (out == NULL)
    {
        printf("Couldn't open output file.\n");
        exit(1);
    }
    frmlen = a_cci_nch[cci] * frmlen_1ch;
    frmlen -= LDAC_FRMHDRBYTES;
    int nbasebands, grad_mode, grad_qu_l, grad_qu_h, grad_ofst_l, grad_ofst_h, abc_flag;

#ifdef NATIVE_CYCLE_PROFILING
    profile_preprocess(&app_settings);
    app_settings.stream_config.component_class = ARC_API_CLASS_ENCODER;

    switch (sfid)
    {
    case LDAC_SMPLRATEID_0:
        app_settings.stream_config.sample_rate = 44100;
        break;
    case LDAC_SMPLRATEID_1:
        app_settings.stream_config.sample_rate = 48000;
        break;
    case LDAC_SMPLRATEID_2:
        app_settings.stream_config.sample_rate = 88200;
        break;
    case LDAC_SMPLRATEID_3:
        app_settings.stream_config.sample_rate = 96000;
        break;
    default:
        break;
    }

    app_settings.stream_config.stream_name = app_settings.input_file_name;
    app_settings.stream_config.stream_type = STREAM_TYPE_PCM_NONINTERLEAVED;
    app_settings.stream_config.num_ch = a_cci_nch[cci];
    app_settings.stream_config.sample_size = (fmt < 5) ? fmt : 4;
    app_settings.stream_config.bit_rate = app_settings.stream_config.sample_rate * app_settings.stream_config.sample_size * 8 * app_settings.stream_config.num_ch;
    app_settings.stream_config.codec_instance_size = sizeof(HANDLE_LDAC_STRUCT);
    app_settings.rt_stats.used_output_buffer = frmlen;
#endif

    result = ldaclib_set_config_info(hLDAC, sfid, cci, frmlen, frm_status);
    if (LDAC_FAILED(result))
    {
        printf("Failed to set configuration information.\n");
        exit(1);
    }
    /* Set Encoding Information */

    ldaclib_get_encode_setting(frmlen_1ch, sfid, &nbasebands, &grad_mode, &grad_qu_l, &grad_qu_h, &grad_ofst_l, &grad_ofst_h, &abc_flag);
    result = ldaclib_set_encode_info(hLDAC, nbasebands, grad_mode, grad_qu_l, grad_qu_h, grad_ofst_l, grad_ofst_h, abc_flag);

    if (LDAC_FAILED(result))
    {
        printf("Failed to set encode infromation\n");
        exit(1);
    }

    /* Initialize ldaclib for Encoding */
    result = ldaclib_init_encode(hLDAC);
    if (LDAC_FAILED(result))
    {
        printf("Failed to initialize encoder\n");
        exit(1);
    }

    int input_frame_size_1ch = npow2_ldac(hLDAC->nlnn);

    ldaclib_clear_error_code(hLDAC);
    ldaclib_clear_internal_error_code(hLDAC);
    char **pp_pcm; //input data
    pp_pcm = (char **)calloc(a_cci_nch[cci], sizeof(char *));
    for (int i = 0; i < a_cci_nch[cci]; i++)
    {
        pp_pcm[i] = (char *)calloc(fmt * input_frame_size_1ch, sizeof(char));
    }

    int frmlen_wrote = 0;                                                                                   //how much bytes wrote
    unsigned char *p_ldac_transport_frame;                                                                  //outputdata
    p_ldac_transport_frame = (unsigned char *)calloc(frmlen + LDAC_FRMHDRBYTES + 2, sizeof(unsigned char)); //&&&
    unsigned char a_frm_header[LDAC_FRMHDRBYTES + 2];

    int frame_size = fmt * input_frame_size_1ch * a_cci_nch[cci] * sizeof(char);
    char *transit = (char *)calloc(frame_size, sizeof(char));

    for (int i = 0; i < file_size / frame_size; i++)
    {
        frmlen_wrote = 0;
        rd_wav_read(&instance, (uint8_t *)transit, frame_size, &read_size);
        prepare_pcm_encode((void *)transit, pp_pcm, input_frame_size_1ch, a_cci_nch[cci], fmt);
#ifdef NATIVE_CYCLE_PROFILING
        if (!profile_frame_preprocess(&app_settings))
            break;
#endif
        wrap_encoding(hLDAC, pp_pcm, fmt, p_ldac_transport_frame, &frmlen_wrote, a_frm_header);
#if defined(NATIVE_CYCLE_PROFILING)
        app_settings.rt_stats.used_input_buffer = app_settings.stream_config.sample_size * input_frame_size_1ch * app_settings.stream_config.num_ch;
        app_settings.rt_stats.microseconds_per_frame = 1000000 * input_frame_size_1ch / app_settings.stream_config.sample_rate;
        app_settings.rt_stats.used_output_buffer = frmlen + LDAC_FRMHDRBYTES;
        if (!profile_frame_postprocess(&app_settings))
            break;
        fprintf(stderr, "\r[%4hd] %5.2f MHz\n", (short)(i),
                (float)app_settings.profiling_data.cycles_per_frame / app_settings.profiling_data.mksec_per_frame);
#endif
        fwrite((void *)p_ldac_transport_frame, frmlen_1ch * a_cci_nch[cci] * sizeof(char), 1, out);
    }
    ldaclib_free_encode(hLDAC);
    //ldaclib_free_handle(hLDAC);
    for (int i = 0; i < a_cci_nch[cci]; i++)
    {
        free(pp_pcm[i]);
    }
    free(pp_pcm);
    free(p_ldac_transport_frame);
    free(transit);
    fclose(out);
    rd_wav_close(&instance);

#ifdef NATIVE_CYCLE_PROFILING
    profile_postprocess(&app_settings);
#endif
    return 0;
}