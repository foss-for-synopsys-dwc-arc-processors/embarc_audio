LDAC Encoder
------------

Introduction
~~~~~~~~~~~~

Lossless Digital Audio Coding technology
(`LDAC <https://www.sony.net/Products/LDAC/?j-short=LDAC>`__) is a
proprietary audio codec made by Sony used to stream high-quality music
to a Bluetooth device. This is a lossy codec that compresses audio with
different efficiency. LDAC supports three fixed rates: 330, 660, 990
kb/s and an adaptive bitrate. Source code of LDAC encoder is available in
Android Open Source Project.

Source Code
~~~~~~~~~~~

The original LDAC source code of the encoder was taken from
`AOSP <https://android.googlesource.com/platform/external/libldac/+/refs/heads/master>`__.
Version of the code taken: commit
300aa818a1a6ee79c9a2fab67be0fdbccefc865b, 20 February 2019.

Multi-instance and reentrance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Codec hasn't tested on multi-instance run, for more information about support of
multi-instance run see codec source documentation. Codec supports reentrance
in case if the value of :exc:`DSP_CTRL` register and accumulators registers (:exc:`ACC0_LO`,
:exc:`ACC0_HI`, :exc:`ACC0_GLO` and :exc:`ACC0_GHI` in case of guard mode is on), see [1] in :ref:`Refs`, will be saved and restored.

Codec-Specific Build Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:exc:`LTO_BUILD` option is disabled for this codec by default. To build the LDAC 
encoder, you must use this specific option: :exc:`FX_MAPPING=ON/OFF`

This option turns ON/OFF remapping of original mathematical fixed point functions
to the optimized ones for ARCv2 DSP processor family. This option is enabled 
by default.

**Example**

To build application and library with original (not optimized) mathematical 
fixed point functions, use the following command:

.. code:: shell

   gmake all FX_MAPPING=off
..

   
Codec-Specific Run-Time Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To run LDAC encoder in MetaWare Debugger (See [2] in :ref:`Refs`),
use the following command:

.. code:: shell

   mdb <mdb parameters> -tcf=<default TCF from /rules/common_hw_config.mk> LDAC_ENCODER_app.elf -i <input_file> -o <output_file> -F <output_frame_size>
..
   
The following table lists parameters that can be passed to the 
LDAC encoder application:

.. table:: Command-Line Options Descriptions for LDAC Encoder
   :align: center
   :widths: 40, 50, 70

   +-------------------------+-----------------------+-----------------------+
   | **Option**              | **Description**       | **Available values**  |
   +=========================+=======================+=======================+
   | :exc:`-i <input_file>`  | Specifies the input   | Any name of .wav file |
   |                         | WAV file              | without spaces        |
   +-------------------------+-----------------------+-----------------------+
   | :exc:`-o <output_file>` | Specifies the output  | Any name of output    |
   |                         | file                  | file without spaces   |
   +-------------------------+-----------------------+-----------------------+
   | :exc:`-F`               | Set output frame size | 165, 110, 82, 66, 55, |
   |                         | per one channel (in   | 47, 41, 36, 33, 30,   |
   |                         | bytes)                | 27, 25, 23            |
   +-------------------------+-----------------------+-----------------------+

**Example**

The following command runs the encoder in ARC nSIM simulator and encodes the 
wav file with a 110-byte output frame size:

.. code:: shell

   mdb -nsim -run -cl -tcf=em9d_voice_audio LDAC_ENCODER_app.elf -i example_96k_2byte_2ch.wav -o example_96k_2byte_2ch_FS110.bin -F 110

Output file **example_96k_2byte_2ch_FS110.bin** must be bit-exact with
the similarly named reference file in the :exc:`ldac_encoder/testvectors/ref` 
folder.
