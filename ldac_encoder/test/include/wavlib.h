/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#ifndef WAV_LIB_H_
#define WAV_LIB_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#ifndef _Rarely
#define _Rarely(cond) cond
#endif

#ifndef _brk
#define _brk()
#endif
#endif

#define _ASSERT(cond)    \
    do { if(_Rarely(!(cond))) {  \
        printf("ARC API ASSERTION FAILED on "__FILE__":%d [%s]\n",__LINE__, #cond); \
        _brk();         \
        }               \
    } while(0)




// definition of potential errors
typedef enum {
	WAV_ERR_NONE,

	WAV_ERR_INVALID_PARAMETER,			// invalid parameter were passed to function

	WAV_ERR_INVALID_HEADER,				// file has invalid header
	WAV_ERR_CAN_NOT_OPEN_FILE,			// wav library can't open file

	WAV_ERR_PARAMS_NOT_GIVEN,			// access to write function is forbidden until audio parameters will be given (e.g. set_params must be called before write)
	WAV_ERR_INVALID_MODE,				// function can't be called in this mode (e.g. we call write when mode is read)
	WAV_ERR_ACCESS_FORBIDDEN,			// function can't be called (used when call of set_params were forbidden)

    WAV_ERR_MEM_ALLOCATION,

	WAV_ERR_EOS							// end of file were reached during reading
} wav_error_t;

// flags used during operation
typedef enum {

	/* public user's flags */

	WAV_WAVEX_HEADER_TYPE,	   // it defines wav header type, if set - WAVEX, otherwise - WAVE
	WAV_UPDATE_DATA_SIZE,	   // it defines updating of data and riff chunks sizes, if set - updating every write operation, otherwise - before closing file

	WAV_LAST_FLAG,             // unused

} wav_public_flags_t;


// definition of wav file's instance
#ifdef PATH_MAX
#define RD_WAV_MAX_FILE_NAME_LEN	PATH_MAX
#else
#define RD_WAV_MAX_FILE_NAME_LEN	(256)
#endif
// main wave instance structure
typedef struct {

	/* DO NOT MODIFY ANY MEMBERS EXPLICITLY */
	FILE         	 *wav_file_desc;
	char              wav_file_name[RD_WAV_MAX_FILE_NAME_LEN + 1];		// +1 char for '\0' symbol
	uint8_t			  last_params_number;								// number of last parameters' set
	void 		     *wav_header;
	uint8_t			  wav_flags;
	uint32_t		  rw_data_size;
	/*ARC_API_Buffer_Configuration_t * config;*/
	
} wav_instance_t;


// WAV header
#pragma pack(1)
typedef struct
{
	char     riff_chunk_id[4];
	uint32_t riff_chunk_size;
	char     wav_chunk_id[4];
	char     fmt_chunk_id[4];
	uint32_t fmt_chunk_size;
	uint16_t fmt_code;
	uint16_t number_of_channels;
	uint32_t samplerate;
	uint32_t datarate;
	uint16_t data_block_size;
	uint16_t samplesize;
	char     data_chunk_id[4];
	uint32_t data_chunk_size;
} wav_header_t;
#pragma pack()

#define WAV_HEADER_SIZE sizeof(wav_header_t)

// WAVX header
#pragma pack(1)
typedef struct
{
	char     riff_chunk_id[4];
	uint32_t riff_chunk_size;
	char     wav_chunk_id[4];
	char     fmt_chunk_id[4];
	uint32_t fmt_chunk_size;
	uint16_t fmt_code;
	uint16_t number_of_channels;
	uint32_t samplerate;
	uint32_t datarate;
	uint16_t data_block_size;
	uint16_t samplesize;
	uint16_t extra_format_bytes_number;
	uint16_t not_used_data;
	uint32_t channel_mask;
	uint32_t guid[4];
	char     data_chunk_id[4];
	uint32_t data_chunk_size;
} wav_header_extended_t;
#pragma pack()

#define WAV_HEADER_EXTENDED_SIZE sizeof(wav_header_extended_t)



/* function for opening wav file
*  Input
*       instance - pointer to wav_instance_t, pointer to main wave instance
*       name	 - pointer to char, name of wav file to open
*       mode	 - pointer to char, file open mode ("rb" or "wb")
*       flags    - uint8_t, flags for "wb" mode (0 : if set - output WAVEX, otherwise - WAVE
*  								    1 : if set - updating data size field in header after writing, otherwise - before file closing
*  								 	2 : if set - permission of audio parameters changing during writing, otherwise - not)
*  Return Values
*       WAV_ERR_NONE - operation succeeded
*       WAV_ERR_INVALID_PARAMETER - incorrect file mode
*       WAV_ERR_CAN_NOT_OPEN_FILE - filename is empty of file cannot be opened
*       WAV_ERR_INVALID_HEADER - incorrect WAVE header in file
*
*/
wav_error_t rd_wav_open(wav_instance_t *instance, const char *name, char *mode, uint8_t flags);

/* function for closing wav file
*  Input
*       instance - pointer to wav_instance_t, pointer to main wave instance
*
*/
void rd_wav_close(wav_instance_t *instance);

/*  function for reading data from WAV file
*   Input
*       instance - pointer to wav_instance_t, pointer to main wave instance
*       buffer	 - pointer to uint8_t, storage for data read
*       max_size - uint32_t, size of buffer
*       read_size- pointer to uint32_t, size of read portion of data
*   Output
        buffer	 - pointer to uint8_t, read data
        read_size- pointer to uint32_t, filled read data size
*   Return Values
*       WAV_ERR_NONE - operation succeeded
*       WAV_ERR_INVALID_MODE - an attempt to read data from file opened for writing is made
*       WAV_ERR_EOS - end of stream has already been reached
*/
wav_error_t rd_wav_read(wav_instance_t *instance, uint8_t *buffer, uint32_t max_size, uint32_t *read_size);

uint32_t get_file_size_from_instance(wav_instance_t *instance);

#endif /* WAV_LIB_H_ */



