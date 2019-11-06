GSM-FR Codec
------------

Introduction
~~~~~~~~~~~~

Full Rate (`GSM-FR <http://www.quut.com/gsm/>`__) is the first
digital speech coding standard used in the GSM digital mobile phone
system. The bit rate of this codec is 13 kbit/s, or 1.625 bits/audio
sample (often padded out to 33 bytes/20 ms or 13.2 kbit/s).

Source code
~~~~~~~~~~~
The source code is based on the original GSM-FR code (`3GPP TS
06.10 <http://www.quut.com/gsm/>`__) created by Jutta Degener and
Carsten Bormann, Technische Universitaet Berlin, and then updated to 18
pathlevel. The Application of this codec is to get RPE-LTP from
`G722 <https://github.com/foss-for-synopsys-dwc-arc-processors/G722/tree/master/rpeltp>`__, commit eddb5a231b51,
and
`G711 <https://github.com/foss-for-synopsys-dwc-arc-processors/G722/tree/master/g711>`__, same commit,
for α-law and µ-law compression. Test sequences are taken
from `3GPP TS
06.10 <http://www.3gpp.org/ftp/Specs/archive/06_series/06.10/0610-820.zip>`__
version 8.2.0 Release 99, June 2001, and this codec passses the test cases successfully.

Multi-instance and reentrance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Codec hasn't tested on multi-instance run, for more information about support of
multi-instance run see codec source documentation. Codec supports reentrance
in case if the value of :exc:`DSP_CTRL` register, AGU address pointer registers (:exc:`AGU_AUX_APx`),
AGU offset registers (:exc:`AGU_AUX_OSx`), AGU modifier registers (:exc:`AGU_AUX_MODx`)
and accumulators registers (:exc:`ACC0_LO`, :exc:`ACC0_HI`, :exc:`ACC0_GLO` and :exc:`ACC0_GHI`
in case of guard mode is on), see [1] in :ref:`Refs`, will be saved and restored.

Codec-Specific Build Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:exc:`LTO_BUILD` and :exc:`REMAPITU_T` options are enabled for this codec by default.

Codec-Specific Run-Time Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To run GSM-FR codec in MetaWare Debugger (See [2] in :ref:`Refs`) use the following commands:
  
For GSM-FR codec:

.. code:: shell

   mdb -cl -run -nsim -tcf=<default TCF from /rules/common_hw_config.mk> ARC_FR_CODEC_app.elf <number of frames> [-l|-u|-A] [-enc|-dec] <input_file.inp> <output_file.cod> <BlockSize> <1stBlock> <NoOfBlocks>

The following table lists the parameters for this codec:

.. table:: Command-Line Options Descriptions for GSM-FR Encoder
   :align: center
   :widths: 50,100

   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<input_file.inp>`           | Specifies the input \*.inp file   |
   +-----------------------------------+-----------------------------------+
   | :exc:`<output_file.cod>`          | Specifies the output \*.cod file  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-l`                         | Input data for encoding and       |
   |                                   | output data for decoding are in   |
   |                                   | linear format (DEFAULT)           |
   +-----------------------------------+-----------------------------------+
   | :exc:`-A`                         | Input data for encoding and       |
   |                                   | output data for decoding are in   |
   |                                   | α-law (G.711) format              |
   +-----------------------------------+-----------------------------------+
   | :exc:`-u`                         | Input data for encoding and       |
   |                                   | output data for decoding are in   |
   |                                   | µ-law (G.711) format              |
   +-----------------------------------+-----------------------------------+
   | :exc:`<BlockSize>`                | Block size, in number of          |
   |                                   | samples (default = 160)           |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`<1stBlock>`                 | Number of the first block         |
   |                                   | of the input file to be processed |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`<NoOfBlocks>`               | Number of blocks to be            |
   |                                   | processed, starting with block    |
   |                                   | <1stBlock>                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`<number of frames>`         | Number of frames to be processed  |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`-enc`                       | Run only the encoder              |
   +-----------------------------------+-----------------------------------+

.. table:: Command-Line Options Descriptions for GSM-FR Decoder
   :align: center
   :widths: 50,100

   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<input_file.cod>`           | Specifies the input \*.cod file   |
   +-----------------------------------+-----------------------------------+
   | :exc:`<output_file.out>`          | Specifies the output \*.out file  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-l`                         | Input data for encoding and       |
   |                                   | output data for decoding are in   |
   |                                   | linear format (DEFAULT)           |
   +-----------------------------------+-----------------------------------+
   | :exc:`-A`                         | Input data for encoding and       |
   |                                   | output data for decoding are in   |
   |                                   | α-law (G.711) format              |
   +-----------------------------------+-----------------------------------+
   | :exc:`-u`                         | Input data for encoding and       |
   |                                   | output data for decoding are in   |
   |                                   | µ-law (G.711) format              |
   +-----------------------------------+-----------------------------------+
   | :exc:`<BlockSize>`                | Block size, in number of          |
   |                                   | samples (default = 160)           |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`<1stBlock>`                 | Number of the first block         |
   |                                   | of the input file to be processed |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`<NoOfBlocks>`               | Number of blocks to be            |
   |                                   | processed, starting with block    |
   |                                   | <1stBlock>                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`<number of frames>`         | Number of frames to be processed  |
   |                                   | (optional)                        |
   +-----------------------------------+-----------------------------------+
   | :exc:`-dec`                       | Run only the decoder              |
   +-----------------------------------+-----------------------------------+

**Examples**

The following command encodes the linear **Seq01.inp** stream:

.. code:: shell

   mdb -cl -run -nsim -tcf=em9d_voice_audio ARC_FR_CODEC_app.elf -l -enc ../testvectors/ref/Seq01.inp ../testvectors/Seq01.cod
..

The following command decodes the linear **Seq01.cod** stream without any additional options:

.. code:: shell

   mdb -cl -run -nsim -tcf=em9d_voice_audio ARC_FR_CODEC_app.elf -l -dec ../testvectors/ref/Seq01.cod ../testvectors/Seq01.out
..
