G.722 Appendix IV
-----------------

Introduction
~~~~~~~~~~~~

`G.722 <https://www.itu.int/rec/T-REC-G.722>`__ is an ITU-T standard 7
kHz Wideband audio codec operating at 48, 56 and 64 kbit/s.
 
Source code
~~~~~~~~~~~ 

G.722 codec, with appendix IV for the decoder, and test sequences for use
with G.722 codec + G.722 decoder appendix IV were taken from `ITU-T
G.722 <https://www.itu.int/rec/T-REC-G.722-201209-I/en>`__ Software
Release 3.00, October 2012, and this codec passes the test cases successfully.

Multi-instance and reentrance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Codec hasn't tested on multi-instance run, for more information about support of
multi-instance run see codec source documentation. Codec supports reentrance
in case if the value of :exc:`DSP_CTRL` register, AGU address pointer registers (:exc:`AGU_AUX_APx`),
AGU offset registers (:exc:`AGU_AUX_OSx`), AGU modifier registers (:exc:`AGU_AUX_MODx`)
and accumulators registers (:exc:`ACC0_LO`, :exc:`ACC0_HI`, :exc:`ACC0_GLO` and :exc:`ACC0_GHI`
in case of guard mode is on), see [1] in :ref:`Refs`, will be saved and restored. Also if you build with 
option :exc:`USE_XCCM=on` you must not use XCCM, because XCCM bank is used for storing table coefficients. 

Codec Specific Build Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:exc:`LTO_BUILD`, :exc:`USE_XCCM` and :exc:`REMAPITU_T` options are enabled for this codec by default.

.. warning:: XCCM bank is used for storing table coefficients.

The following table lists specific build options of G.722 codec:

.. table:: Codec Specific Build Options
   :widths: 30, 130
   
   +--------------------------------------------+-----------------------------+
   | **Command**                                | **Description**             |
   +============================================+=============================+
   | :exc:`gmake COMPONENT=\<ENCODER/DECODER\>` | These commands build only   |
   |                                            | encoder (**encg722.elf**)or |
   |                                            | decoder (**decg722.elf**)   |
   +--------------------------------------------+-----------------------------+
   | :exc:`WMOPS=on`                            | Display information about   |
   |                                            | each frame, frame number,   |
   |                                            | total Weighted MOPS,        |
   |                                            | computational complexity of |
   |                                            | the encoder or decoder for  |
   |                                            | that frame, the average     |
   |                                            | WMOPS figure for the frames |
   |                                            | processed, the observed     |
   |                                            | worst case WMOPS figure and |
   |                                            | the observed “worst worst   |
   |                                            | case” figure of the encoder |
   |                                            | or decoder for the current  |
   |                                            | frame                       |
   +--------------------------------------------+-----------------------------+
   | :exc:`USE_XCCM=on`                         | Use XCCM bank for storing   |
   |                                            | tables of coefficients in   |
   |                                            | funcg722.c                  |
   +--------------------------------------------+-----------------------------+

**Example**

The following command builds the encoder application and library with WMOPS:

.. code:: shell

   gmake COMPONENT=ENCODER WMOPS=on
..
   
Codec-Specific Run-Time Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To run G.722 codec in MetaWare Debugger (See [2] in :ref:`Refs`), use the 
following command:

 - For G.722 encoder:

   .. code:: shell

      mdb -run -cl -nsim -tcf=<default TCF from /rules/common_hw_config.mk> encg722.elf [-q] [-mode <M>] [-byte] [-fsize N] [-frames N2] <file.inp> <file.out>
   ..
  
 - For G.722 decoder, appendix IV:

   .. code:: shell

      mdb -run -cl -nsim -tcf=<default TCF from /rules/common_hw_config.mk> decg722.elf [-fsize N] <file.inp> <file.out>
   ..
   
 - For G.722 test encoder:

   .. code:: shell

      mdb -run -cl -nsim -tcf=<default TCF from /rules/common_hw_config.mk> tstcg722.elf <file.inp> <file.ref>
   ..
   
 - For G.722 test decoder:

   .. code:: shell

      mdb -run -cl -nsim -tcf=<default TCF from /rules/common_hw_config.mk> tstdg722.elf <file.inp> <low_file.ref> <high_file.ref>
   ..
   
The following tables list the parameters that can be passed to the G.722 codec:

.. table:: Command-Line Options Descriptions for G.722 Encoder
   :align: center
   :widths: 30, 130
   
   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<file.inp>`                 | Specifies the input \*.bin file   |
   +-----------------------------------+-----------------------------------+
   | :exc:`<file.out>`                 | Specifies the output \*.cod file  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-byte`                      | Provide encoder output data in    |
   |                                   | legacy byte-oriented format       |
   |                                   | (default is g192)                 |
   +-----------------------------------+-----------------------------------+
   | :exc:`-fsize`                     | Number of 16 kHz input samples    |
   |                                   | per frame (must be an even        |
   |                                   | number). Default is 160           |
   |                                   | samples(16 kHz) (10 ms)           |
   +-----------------------------------+-----------------------------------+
   | :exc:`-frames`                    | Number of frames to process       |
   |                                   | (values -1 or 0 processes the     |
   |                                   | whole file)                       |
   +-----------------------------------+-----------------------------------+
   | :exc:`-mode <M>`                  | Operating mode (1,2,3) (or rate   |
   |                                   | 64, 56, 48 in kbps) . Default is  |
   |                                   | mode 1 (= 64 kbps)                |
   +-----------------------------------+-----------------------------------+
   | :exc:`-h/-help`                   | Print help message                |
   +-----------------------------------+-----------------------------------+
   | :exc:`-q`                         | Suppress debug information        |
   +-----------------------------------+-----------------------------------+

.. table:: Command-Line Options Descriptions for G.722 Decoder of appendix IV
   :align: center
   :widths: 30, 180
   
   +--------------------+-------------------------------------------------------+
   | **Option**         | **Description**                                       |
   +====================+=======================================================+
   | :exc:`<file.inp>`  | Specifies the input file \*.bst file                  |
   +--------------------+-------------------------------------------------------+
   | :exc:`<file.out>`  | Specifies the output file \*.out file                 |
   +--------------------+-------------------------------------------------------+
   | :exc:`-fsize`      | Define frame size for g192 operation and file reading |
   +--------------------+-------------------------------------------------------+

.. table:: Command-Line Options Descriptions for Test G.722 Decoder
   :align: center
   :widths: 30, 150
   
   +------------------------+-------------------------------------------+
   | **Option**             | **Description**                           |
   +========================+===========================================+
   | :exc:`<file.inp>`      | Specifies the input file \*.cod file      |
   +------------------------+-------------------------------------------+
   | :exc:`<low_file.ref>`  | Specifies the low part of reference file  |
   +------------------------+-------------------------------------------+
   | :exc:`<high_file.ref>` | Specifies the high part of reference file |
   +------------------------+-------------------------------------------+
..
   
.. table:: Command-Line Options Descriptions for Test G.722 Encoder
   :align: center
   :widths: 30, 180
   
   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<file.inp>`                 | Specifies the input file \*.cod   |
   |                                   | file                              |
   +-----------------------------------+-----------------------------------+
   | :exc:`<file.ref>`                 | Specifies the reference file for  |
   |                                   | checking the correctness of       |
   |                                   | Encoder                           |
   +-----------------------------------+-----------------------------------+

**Examples**

.. note::
   The decoder supports only g192 byte format. For this reason, if you will encode a file in the legacy byte-oriented
   format using [-byte] option, you can not decode this file by the decoder.

The following command encodes the **inpsp.bin** stream to legacy byte-oriented format:

.. code:: shell

   mdb -run -cl -nsim -tcf=em9d_voice_audio encg722.elf -byte ../testvectors/inp/inpsp.bin ../testvectors/temp.cod

The following command decodes the **test10.bst** stream without any additional options:

.. code:: shell

   mdb -run -cl -nsim -tcf=em9d_voice_audio decg722.elf ../testvectors/inp/test10.bst ../testvectors/test10.out
