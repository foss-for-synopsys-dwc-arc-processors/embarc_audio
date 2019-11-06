/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include <stdlib.h>
#include <assert.h>
#include "wavlib.h"

// macros for operation with flags
#define SET_FLAG(value, flag) value |= 1 << flag
#define CLEAR_FLAG(value, flag) value &= ~(1 << flag)
#define TEST_FLAG(value, flag) (value & (1 << flag))

#define WAV_HEADER_MAX_SIZE 128
#define WAV_CHUNK_HEADER_SIZE 8

// internal flags used during operation
typedef enum
{

    WAV_WRITING_MODE = WAV_LAST_FLAG, // it defines operation mode, if set - writing, otherwise - reading
    WAV_PARAMS_NOT_GIVEN,              // if set - audio parameters were not given
    WAV_PARAMS_CHANGED,                  // if set - indicate that parameters changed and new wav file must be opened
    WAV_CLOSE_WITHOUT_FREEING,          // if set - rd_wav_close closes file without freeing wav_header
    WAV_EOS                              // if set - EOS were reached

} wav_internal_flags_t;

// pattern of WAVE header
static const char wav_header_pat[WAV_HEADER_SIZE] =
    {
        'R', 'I', 'F', 'F', /* chunk id */
        0, 0, 0, 0,            /* <place: chunk size> */
        'W', 'A', 'V', 'E', /* WAVE ID */
        'f', 'm', 't', ' ', /* chunk with 'format' */
        16, 0, 0, 0,        /* chunk size: no extensions */
        1, 0,                /* format code: WAVE_FORMAT_PCM */
        0, 0,                /* <place: number of channels> */
        0, 0, 0, 0,            /* <place: sampling rate> */
        0, 0, 0, 0,            /* <place: data rate> */
        0, 0,                /* <place: bits per sample> */
        0, 0,                /* number of extra bytes*/
        'd', 'a', 't', 'a', /* chunk with 'data' */
        0, 0, 0, 0            /* <place: chunk size> */
};

// pattern of WAVEX header
static const char wav_header_pat_extended[WAV_HEADER_EXTENDED_SIZE] =
    {
        'R', 'I', 'F', 'F',        /* chunk id */
        0, 0, 0, 0,                /* <place: chunk size> */
        'W', 'A', 'V', 'E',        /* WAVE ID */
        'f', 'm', 't', ' ',        /* chunk with 'format' */
        40, 0, 0, 0,            /* chunk size: no extensions */
        (char)0xFE, (char)0xFF, /* format code: WAVE_FORMAT_PCM */
        0, 0,                    /* <place: number of channels> */
        0, 0, 0, 0,                /* <place: sampling rate> */
        0, 0, 0, 0,                /* <place: data rate> */
        0, 0,                    /* <place: data block size> */
        0, 0,                    /* <place: bits per sample> */
        22, 0,                    /* extra format bytes number */
        0, 0,                    /* not-used data */
        0, 0, 0, 0,                /* <place: channel mask> */
                                /* GUID "00000001-0000-0010-8000-00aa00389b71"
                                                       for PCM in WAVEX format */
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x00,
        (char)0x80, 0x00, 0x00, (char)0xAA,
        0x00, 0x38, (char)0x9B, 0x71,

        'd', 'a', 't', 'a', /* chunk with 'data' */
        0, 0, 0, 0            /* <place: chunk size> */
};

// Max number of channels could be in WaveEx stream

static uint32_t read_data_start_pos = 0;

//============================================================================================================
//=================================STATIC FUNCTIONS===========================================================
//============================================================================================================

/*  function for adding number to filename before extension
*   Input
*       filename            - pointer to char, original filename
*       filename_numbered   - pointer to char, buffer where name with number will be stored
*        number               - uint32_t, number for adding
*   Output        
*       filename_numbered   - pointer to char, buffer filled with filename with number 
*
*/
static void add_number_to_name(const char *filename, char *filename_numbered, uint32_t number)
{
    char *exts = NULL;
    uint32_t sym_to_copy = 0;

    _ASSERT(filename);
    _ASSERT(filename_numbered);
    _ASSERT(number < 1000);
    _ASSERT(strlen(filename) <= (RD_WAV_MAX_FILE_NAME_LEN - 8)); // check max filename size, 8 means 8 symbols "_ddd.wav" (ddd - max number's size)

    filename_numbered[0] = '\0';

    // step 1. search extensions
    exts = strchr(filename, '.');

    // step 2. add number "_number" to name
    if (exts)
    {
        sym_to_copy = exts - filename;
        strncat(filename_numbered, filename, sym_to_copy);
    }
    else
    {
        sym_to_copy = strlen(filename);
        //overflow can't happen here as we control filename string length above
        strcpy(filename_numbered, filename);
    }

    sprintf(filename_numbered + sym_to_copy, "_%u", number);

    // step 3. add extension
    if (exts)
        //overflow can't happen here as we control filename string length above
        strcat(filename_numbered, exts);
    else
        //overflow can't happen here as we control filename string length above
        strcat(filename_numbered, ".wav");
}

/*  function for getting file size
*   Input
*       file    - pointer to FILE, pointer to descriptor of file
*   Return Values
*       uint32_t, file size
*
*/
static uint32_t get_file_size(FILE *file)
{
    int32_t res, size, pos;

    _ASSERT(file);
    // save previous pos, negative value is controlled by asserts
    pos = ftell(file);
    _ASSERT(pos >= 0);
    // set pos to file's end
    res = fseek(file, 0, SEEK_END);
    _ASSERT(res == 0);
    // getting current pos
    size = ftell(file);
    _ASSERT(size >= 0);
    // restoring previous pos
    res = fseek(file, pos, SEEK_SET);
    _ASSERT(res == 0);

    return (uint32_t)size;
}

/*  function for seeking given chunk in buffer
*   Input
*       buffer      - pointer to char, pointer to buffer for searching
*       buffer_size - uint32_t, size of buffer
*       chunk_name  - pointer to char, name of required chunk
*
*    Return Values 
*       pointer to chunk's payload (if chunk was found), NULL (if chunk is not available)
*
*/
static char *search_chunk_payload(void *buffer, uint32_t buffer_size, char *chunk_name)
{
    char *pos;
    uint32_t search_size;

    _ASSERT(buffer);
    _ASSERT(buffer_size);
    _ASSERT(chunk_name);

    pos = (char *)buffer;
    search_size = buffer_size;

    // TODO: Probably it is better to replace this alogorithm with strstr function call
    /*pos = strstr(pos, chunk_name);
    if (pos)
    {
        return pos + WAV_CHUNK_HEADER_SIZE;
    }
    else
    {
        return NULL;
    }*/

    while (search_size)
    {
        pos = (char *)memchr(pos, chunk_name[0], search_size);

        if (pos)
        {
            if (!strncmp(chunk_name, pos, strlen(chunk_name)))
            {
                return pos + WAV_CHUNK_HEADER_SIZE;
            }
            else
            {
                pos++;
                search_size = buffer_size - (pos - (char *)buffer);
            }
        }
        else
        {
            break;
        }
    }

    return NULL;
}

#define TMP_BUFFER_SIZE 4096 // increase this value in case of data chunk is placed far away from fmt

/* function for reading and defining proper header type
*
*  Input
*       instance - pointer to wav_instance_t, pointer to main wave instance
*  Return Values
*       0 - header type detected
*       1 - incorrect header
*
*/
static int read_wav_header(wav_instance_t *instance)
{
    char *str;
    uint32_t result, read_bytes, search_size;
    unsigned long data_chunk_offset;
    uint32_t data_chunk_size_offset;
    char data_chunk_arr[4];

    _ASSERT(instance);

    result = TEST_FLAG(instance->wav_flags, WAV_WRITING_MODE);

    _ASSERT(!result);

    // step 1. reading header
    read_bytes = fread(instance->wav_header, 1, WAV_HEADER_MAX_SIZE, instance->wav_file_desc);

    if (read_bytes < WAV_HEADER_SIZE)
        return 1;

    // step 2. check header
    str = (char *)instance->wav_header;

    if (strncmp(str, "RIFF", 4) != 0)
        return 1;

    str += 8;
    if (strncmp(str, "WAVEfmt ", 8) != 0)
        return 1;

    str += 12;
    if ((str[0] != 0x01 || str[1] != 0x00) && (str[0] != 0x03 || str[1] != 0x00))
    {
        SET_FLAG(instance->wav_flags, WAV_WAVEX_HEADER_TYPE);

        if (str[0] != (char)0xFE || str[1] != (char)0xFF)
            return 1;

        // check that we have full extended header
        if (read_bytes < WAV_HEADER_MAX_SIZE)
            return 1;
    }

    // step 3. searching data chunk's payload
    str += 16;
    search_size = read_bytes - (str - (char *)instance->wav_header);

    str = search_chunk_payload(str, search_size, "data");

    // step 4. setting read position to data chunk's payload
    if (!str)
    {
        uint32_t read_bytes = TMP_BUFFER_SIZE;
        char *tmp_buffer = (char *)malloc(read_bytes);

        read_bytes = fread(tmp_buffer, 1, read_bytes, instance->wav_file_desc);

        if (!read_bytes)
        {
            free(tmp_buffer);
            return 1;
        }

        str = search_chunk_payload(tmp_buffer, read_bytes, "data");

        if (!str)
        {
            free(tmp_buffer);
            return 1;
        }

        data_chunk_offset = WAV_HEADER_MAX_SIZE + (str - tmp_buffer);

        free(tmp_buffer);
    }
    else
    {
        data_chunk_offset = str - (char *)instance->wav_header;
    }

    data_chunk_size_offset = data_chunk_offset - 4;

    result = fseek(instance->wav_file_desc, data_chunk_size_offset, SEEK_SET);
    _ASSERT(result == 0);

    fread(data_chunk_arr, 1, 4, instance->wav_file_desc);

    ((wav_header_t *)instance->wav_header)->data_chunk_size = *((uint32_t *)((void *)data_chunk_arr));

    // step 4. setting read position to data chunk's payload
    if (data_chunk_offset)
    {
        result = fseek(instance->wav_file_desc, data_chunk_offset, SEEK_SET);
        _ASSERT(result == 0);
        return 0;
    }

    return 1;
}

/*  function for updating riff chunk's and data chunk's sizes
*
*   Input
*       instance - pointer to wav_instance_t, pointer to main wave instance
*
*/
static void update_header(wav_instance_t *instance)
{
    int32_t result, pos;
    FILE *wav_file;
    wav_header_t *header;

    _ASSERT(instance);
    _ASSERT(instance->wav_header);
    _ASSERT(instance->wav_file_desc);

    wav_file = instance->wav_file_desc;
    header = (wav_header_t *)instance->wav_header;

    // step 1. save previous pos
    pos = ftell(wav_file);
    _ASSERT(pos >= 0);

    // step 2. updating header
    result = fseek(wav_file, 0, SEEK_SET);
    _ASSERT(result == 0);

    if (TEST_FLAG(instance->wav_flags, WAV_WAVEX_HEADER_TYPE))
    {
        wav_header_extended_t *header_ext = (wav_header_extended_t *)instance->wav_header;

        header_ext->data_chunk_size = instance->rw_data_size;
        header_ext->riff_chunk_size = instance->rw_data_size + WAV_HEADER_EXTENDED_SIZE - 8;

        result = fwrite(header, WAV_HEADER_EXTENDED_SIZE, 1, instance->wav_file_desc);
        _ASSERT(result > 0);
    }
    else
    {
        header->data_chunk_size = instance->rw_data_size;
        header->riff_chunk_size = instance->rw_data_size + WAV_HEADER_SIZE - 8;
        result = fwrite(header, WAV_HEADER_SIZE, 1, instance->wav_file_desc);
        _ASSERT(result > 0);
    }

    // step 3. restoring previous pos
    result = fseek(wav_file, pos, SEEK_SET);
    _ASSERT(result == 0);
}

/*  function for checking wav's header on valid fields
*   Input
*       instance - pointer to wav_instance_t, pointer to main wave instance
*   Return Values
*       1 - header is valid
*       0 - header is not valid
*/
static int is_header_params_normal(wav_instance_t *instance)
{
    wav_header_t *header;

    _ASSERT(instance);

    header = instance->wav_header;

    // check parameters
    if (header->number_of_channels == 0 || header->samplerate == 0)
        return 0;

    if (header->samplesize != 8 && header->samplesize != 16 && header->samplesize != 24 && header->samplesize != 32)
        return 0;

    //tmp = header->number_of_channels * (header->samplesize >> 3);

    //if(tmp != header->data_block_size)
    //    return 0;

    //tmp *= header->samplerate;

    //if(tmp != header->datarate)
    //    return 0;

    if (header->number_of_channels > 32)
        return 0;

    return 1;
}

//============================================================================================================
//=================================END OF STATIC FUNCTIONS====================================================
//============================================================================================================

wav_error_t rd_wav_eof(wav_instance_t *instance)
{
    _ASSERT(instance);
    _ASSERT(instance->wav_file_desc);
    _ASSERT(instance->wav_header);

    if (feof(instance->wav_file_desc) || (instance->rw_data_size >= ((wav_header_t *)instance->wav_header)->data_chunk_size))
        return WAV_ERR_EOS;

    return WAV_ERR_NONE;
}

wav_error_t rd_wav_open(wav_instance_t *instance, const char *name, char *mode, uint8_t flags)
{
    //char name_with_number[RD_WAV_MAX_FILE_NAME_LEN + 1];    // array of chars for wav filename in writing mode

    _ASSERT(instance);
    _ASSERT(name);
    _ASSERT(mode);

    // step 1. filling instance with zeros
    memset(instance, 0, sizeof(wav_instance_t));

    // step 2. defining operation mode
    if (strcmp(mode, "rb") != 0)
    {
        SET_FLAG(instance->wav_flags, WAV_WRITING_MODE);
        if (strcmp(mode, "wb") != 0)
            return WAV_ERR_INVALID_PARAMETER;
    }

    // instance->config = (ARC_API_Buffer_Configuration_t *) malloc(sizeof(ARC_API_Buffer_Configuration_t));

    // if (instance->config == NULL)
    //     return WAV_ERR_MEM_ALLOCATION;

    // memset(instance->config, 0, sizeof(ARC_API_Buffer_Configuration_t));

    // step 3. opening file
    if (TEST_FLAG(instance->wav_flags, WAV_WRITING_MODE))
    {
        //if(strlen(name) > RD_WAV_MAX_FILE_NAME_LEN)
        //    return WAV_ERR_CAN_NOT_OPEN_FILE;

        //strncpy(instance->wav_file_name, name, RD_WAV_MAX_FILE_NAME_LEN);
        //add_number_to_name(instance->wav_file_name, name_with_number, instance->last_params_number);

        instance->wav_file_desc = fopen(name, mode);
    }
    else
    {
        instance->wav_file_desc = fopen(name, mode);
    }

    if (instance->wav_file_desc == NULL)
        return WAV_ERR_CAN_NOT_OPEN_FILE;

    // step 4. allocating buffer for storing header of wav file
    instance->wav_header = (void *)malloc(WAV_HEADER_MAX_SIZE);
    _ASSERT(instance->wav_header);

    // step 5. reading wav header
    if (!TEST_FLAG(instance->wav_flags, WAV_WRITING_MODE))
    {
        // reading header
        if (read_wav_header(instance) != 0)
        {
            free(instance->wav_header);
            instance->wav_header = NULL;

            return WAV_ERR_INVALID_HEADER;
        }

        // check header parameters
        if (!is_header_params_normal(instance))
            return WAV_ERR_INVALID_HEADER;
    }
    // step 5. filling wav header with pattern
    else
    {
        instance->wav_flags |= flags & 0x03;

        if (TEST_FLAG(instance->wav_flags, WAV_WAVEX_HEADER_TYPE))
            memcpy(instance->wav_header, wav_header_pat_extended, sizeof(wav_header_pat_extended));
        else
            memcpy(instance->wav_header, wav_header_pat, sizeof(wav_header_pat));

        SET_FLAG(instance->wav_flags, WAV_PARAMS_NOT_GIVEN);
    }

    // for trick play test save data start position
    if (!TEST_FLAG(instance->wav_flags, WAV_WRITING_MODE))
        read_data_start_pos = ftell(instance->wav_file_desc);

    return WAV_ERR_NONE;
}

void rd_wav_close(wav_instance_t *instance)
{
    unsigned int result;

    _ASSERT(instance);
    _ASSERT(instance->wav_file_desc);
    _ASSERT(instance->wav_header);

    // step 1. writing header if writing mode
    if (TEST_FLAG(instance->wav_flags, WAV_WRITING_MODE) && (instance->rw_data_size > 0))
        update_header(instance);

    // step 2. closing wav file
    result = fclose(instance->wav_file_desc);
    _ASSERT(result == 0);

    // step 3. freeing allocated memory
    if (!TEST_FLAG(instance->wav_flags, WAV_CLOSE_WITHOUT_FREEING))
    {
        free(instance->wav_header);
        instance->wav_header = NULL;
    }

    if (TEST_FLAG(instance->wav_flags, WAV_CLOSE_WITHOUT_FREEING))
        CLEAR_FLAG(instance->wav_flags, WAV_CLOSE_WITHOUT_FREEING);
}

/*  function for reading audio data from wav file
 *
 *  instance  [IN]  - instance of wav file
 *  buffer       [IN]  - buffer with data
 *  buffer_size  [IN]  - max size which can be read from file
 *    read_size    [OUT] - size of read data from file to buffer
 *
 *    return             - WAV_ERR_NONE, WAV_ERR_EOS, WAV_ERR_INVALID_MODE
 *
 * */
wav_error_t rd_wav_read(wav_instance_t *instance, uint8_t *buffer, uint32_t buffer_size, uint32_t *read_size)
{
    uint32_t result;
    wav_header_t *header;

    _ASSERT(instance);
    _ASSERT(buffer);
    _ASSERT(read_size);
    _ASSERT(instance->wav_header);

    if (TEST_FLAG(instance->wav_flags, WAV_WRITING_MODE))
        return WAV_ERR_INVALID_MODE;

    if (TEST_FLAG(instance->wav_flags, WAV_EOS))
        return WAV_ERR_EOS;

    header = instance->wav_header;

    // step 1. aliging read block size to wav data_block_size
    buffer_size = buffer_size - buffer_size % ((uint32_t)header->data_block_size << 2);

    // step 2. reading data
    *read_size = fread((void *)buffer, 1, buffer_size, instance->wav_file_desc);

    // step 3. updating overall read size
    instance->rw_data_size += *read_size;

    // step 4. checking errors
    result = ferror(instance->wav_file_desc);
    _ASSERT(result == 0);

    // step 5. checking eof
    if ((rd_wav_eof(instance) == WAV_ERR_EOS) || (instance->rw_data_size >= ((wav_header_t *)instance->wav_header)->data_chunk_size))
    {
        *read_size = *read_size - *read_size % ((uint32_t)header->data_block_size << 2);

        SET_FLAG(instance->wav_flags, WAV_EOS);

        return WAV_ERR_EOS;
    }

    return WAV_ERR_NONE;
}

/*  function for reading audio data from wav file
 *
 *  instance  [IN]  - instance of wav file
 *  buffer       [IN]  - buffer with data
 *  buffer_size  [IN]  - max size which can be read from file
 *    read_size    [OUT] - size of read data from file to buffer
 *
 *    return             - WAV_ERR_NONE, WAV_ERR_EOS, WAV_ERR_INVALID_MODE
 *
 * */

uint32_t get_file_size_from_instance(wav_instance_t *instance)
{
    return ((wav_header_t *)instance->wav_header)->data_chunk_size;
}