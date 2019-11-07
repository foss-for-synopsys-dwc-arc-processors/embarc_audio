G.726
-----

Introduction
~~~~~~~~~~~~

`G.726 <https://www.itu.int/rec/T-REC-G.191-201901-I/en>`__ is an
ITU-T ADPCM speech codec standard covering the transmission of voice
at rates of 16, 24, 32, and 40 kbit/s. 

Source Code
~~~~~~~~~~~

The original G.726 source code and test sequences are taken from
`G.191 <https://www.itu.int/rec/T-REC-G.191-201901-I/en>`__ version
2.0, 24 January 2000, and this codec passes the test cases successfully. 

Multi-instance and reentrance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Codec hasn't tested on multi-instance run, for more information about support of
multi-instance run see codec source documentation. Codec supports reentrance
in case if AGU address pointer registers (:exc:`AGU_AUX_APx`), AGU offset registers (:exc:`AGU_AUX_OSx`),
AGU modifier registers (:exc:`AGU_AUX_MODx`), see [1] in :ref:`Refs`, will be saved and restored.

Codec-Specific Build Options 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:exc:`LTO_BUILD` and :exc:`REMAPITU_T` options are enabled for that codec by default.

Codec-Specific Run-Time Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
To run G.726 codec in MetaWare Debugger (See [2] in :ref:`Refs`),
use the following command: 

- with variable bitrate:

  .. code:: shell

     mdb -run -cl -nsim -tcf=<default TCF from /rules/common_hw_config.mk> vbr-g726.elf [-enc|-dec] -law [A|u|l] -rate [16|24|32|40|16-24-32-40] -frame [NoOfSamples] [-noreset] [-?|-help] <InpFile> <OutFile> [<FrameSize> [<1stBlock> [<NoOfBlocks> [<Reset>]]]]

- with constant bit rate:

  .. code:: shell

     mdb -run -cl -nsim -tcf=<default TCF from /rules/common_hw_config.mk> g726demo.elf [-noreset] [-?|-help] <Law> <Transf> <Rate>  <InpFile> <OutFile> [<FrameSize> [<1stBlock> [<NoOfBlocks> [<Reset>]]]]

	 
The following tables list the parameters that can be passed to G.726.

.. table:: Command-Line Options Descriptions for G.726 with Variable Bitrate Support
   :align: center
   :widths: 20, 130
   
   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<input_file>`               | Specifies the input file          |
   +-----------------------------------+-----------------------------------+
   | :exc:`<output_file>`              | Specifies the output file         |
   +-----------------------------------+-----------------------------------+
   | :exc:`-enc`                       | Encode mode. Default: run encoder |
   |                                   | and decoder (optional)            |
   +-----------------------------------+-----------------------------------+
   | :exc:`-dec`                       | Encode mode. Default: run encoder |
   |                                   | and decoder (optional)            |
   +-----------------------------------+-----------------------------------+
   | :exc:`-law A|u`                   | The letters A or a for G.711      |
   |                                   | A-law, letter u for G.711 u-law,  |
   |                                   | or letter l for linear. If linear |
   |                                   | is chosen, A-law is used to       |
   |                                   | compress/expand samples to/from   |
   |                                   | the G.726 routines.Default is     |
   |                                   | A-law (optional)                  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-rate`                      | Bit-rate (in kbit/s): 40,         |
   |                                   | 32, 24 or 16 (in kbit/s) or a     |
   |                                   | combination of them using dashes  |
   |                                   | (For example, 32-24 or 16-24-32). |
   |                                   | Default is 32 kbit/s (optional)   |
   +-----------------------------------+-----------------------------------+
   | :exc:`-frame`                     | Number of samples per frame for   |
   |                                   | switching bit rates. Default is   |
   |                                   | 16 samples (optional)             |
   +-----------------------------------+-----------------------------------+
   | :exc:`-noreset`                   | Do not reset the encoder/decoder  |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`-?|-help`                   | Print help message (optional)     |
   +-----------------------------------+-----------------------------------+
   | :exc:`FrameSize`                  | Frame size, in number of          |
   |                                   | samples; The bitrate only         |
   |                                   | changes at the boundaries of a    |
   |                                   | frame. Default: 16 samples        |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`1stBlock`                   | The number of the first block     |
   |                                   | of the input file to be           |
   |                                   | processed. Default: first block   |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`NoOfBlocks`                 | The number of blocks to be        |
   |                                   | processed, starting with block    |
   |                                   | “1stBlock” (optional)             |
   +-----------------------------------+-----------------------------------+
   | :exc:`Reset`                      | If specified as 1, the coder and  |
   |                                   | decoder are reset at the very     |
   |                                   | beginning of the processing. If   |
   |                                   | 0, the processing starts with     |
   |                                   | the variables in an unknown       |
   |                                   | state. Default is 1 (reset ON).   |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
..

.. table:: Command-Line Options Descriptions for G.726 with Constant Bitrate Support
   :align: center
   :widths: 20, 130
   
   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<input_file>`               | Specifies the input file          |
   +-----------------------------------+-----------------------------------+
   | :exc:`<output_file>`              | Specifies the output file         |
   +-----------------------------------+-----------------------------------+
   | :exc:`Transf`                     | Desired conversion on the         |
   |                                   | input file (optional):[lolo],     |
   |                                   | (A/u)log -> ADPCM -> (A/u)        |
   |                                   | log[load], (A/u)log ->            |
   |                                   | ADPCM[adlo], ADPCM -> (A/u) log   |
   +-----------------------------------+-----------------------------------+
   | :exc:`Law`                        | Desired law (either A or u)       |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`Rate`                       | The bit-rate (in kbit/s): 40,     |
   |                                   | 32, 24 or 16 (in kbit/s)or a      |
   |                                   | combination of them using dashes  |
   |                                   | (For example, 32-24 or 16-24-32). |
   |                                   | Default is 32 kbit/s (optional)   |
   +-----------------------------------+-----------------------------------+
   | :exc:`-frame`                     | Number of samples per frame for   |
   |                                   | switching bit rates. Default is   |
   |                                   | 16 samples (optional)             |
   +-----------------------------------+-----------------------------------+
   | :exc:`-noreset`                   | Do not reset the encoder/decoder  |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`-?|-help`                   | print help message (optional)     |
   +-----------------------------------+-----------------------------------+
   | :exc:`FrameSize`                  | The frame size, in number of      |
   |                                   | samples; the bitrate only         |
   |                                   | changes at the boundaries of a    |
   |                                   | frame. Default: 16 samples        |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`1stBlock`                   | The number of the first block     |
   |                                   | of the input file to be           |
   |                                   | processed. Default: first block   |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`NoOfBlocks`                 | The number of blocks to be        |
   |                                   | processed, starting with block    |
   |                                   | “1stBlock” (optional)             |
   +-----------------------------------+-----------------------------------+
   | :exc:`Reset`                      | If specified as 1, the coder and  |
   |                                   | decoder are reset at the very     |
   |                                   | beginning of the processing. If   |
   |                                   | 0, the processing starts with     |
   |                                   | the variables in an unknown       |
   |                                   | state. Default is 1 (reset ON).   |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
..

**Examples**
   
The following command instructs the codec encoding and decoding
audio file decoder to A-law log with variable bitrate:

.. code:: shell

   mdb -run -cl -nsim -tcf=em9d_voice_audio vbr-g726.elf -q -law A -rate 16-24-32-40-32-24 ../testvectors/Ref/voice.src ../testvectors/voicvbra.tst

The following command instructs the codec to encode converting from A-law
log input file to ADPCM decoded file with frame size is 16 and 1 start
block:

.. code:: shell

   mdb -run -cl -nsim -tcf=em9d_voice_audio g726demo.elf -q a load 16 ../testvectors/Ref/nrm.a ../testvectors/rn16fa.i 16 1 1024

