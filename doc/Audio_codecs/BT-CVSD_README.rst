BT-CVSD Codec
-------------

Introduction
~~~~~~~~~~~~

Bluetooth Continuous Variable Slope Delta Modulation
(`BT-CVSD <https://www.bluetooth.com/specifications/bluetooth-core-specification>`__)
is a voice coding method. It uses delta modulation with a variable step
size a special case of adaptive delta modulation. The input to the CVSD encoder
is 64000 samples per second linear PCM (16 bits per sample). This design is according to the Bluetooth Core Specification v 5.1.

Source Code
~~~~~~~~~~~

BT-CVSD codec is developed according to `Bluetooth Core Specification v
5.1 <https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=457080>`__, 21 January 2019.

Upsampling and Downsampling
~~~~~~~~~~~~~~~~~~~~~~~~~~~

8x-upsampler and 8x-downsampler are parts of the test application and provided as examples only.
FIR filter parameters coefficients are designed per Bluetooth Core 5.1 spectral mask requirements
and provided as an example also. Test application processes 8 kHz sampled signal at the input and provides
8 kHz samples signal as output.

Multi-instance and reentrance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BT-CVSD codec implementation is reentrant and supports multi-instance. The codec doesn't use 
any DSP registers directly. It is independent of DSP modes like saturation, guard 
bits and accumulator shift. If CVSD codec process frame is interrupted e.g. by interrupt
it is the responsibility of ISR to restore DSP specific registers.

Codec-Specific Build Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :exc:`LTO_BUILD` and :exc:`REMAPITU_T` options are not available for this codec. 

Codec-Specific Run-time Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use the following command to run the BT-CVSD codec in MetaWare Debugger
(See [2] in :ref:`Refs`):

.. code:: shell

   mdb -cl -run -nsim -tcf=<default TCF from /rules/common_hw_config.mk> ARC_CVSD_CODEC_app.elf [-u| -h] [-F <frame_len>] [-enc|-dec|-encdec] -i <input_file.inp> -o <output_file.cod>

The following table lists the parameters that can be passed to this codec:

.. table:: Command-Line Option Descriptions for CVSD Codec
   :align: center
   :widths: 20,130

   +-----------------------------------+-----------------------------------+
   | **Option**                        | **Description**                   |
   +===================================+===================================+
   | :exc:`<input_file.inp>`           | Specifies the input \*.inp file   |
   +-----------------------------------+-----------------------------------+
   | :exc:`<output_file.cod>`          | Specifies the output \*.cod file  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-u/-h`                      | Print help message                |
   +-----------------------------------+-----------------------------------+
   | :exc:`-F <frame_len>`             | Number of samples, default: 256   |
   +-----------------------------------+-----------------------------------+
   | :exc:`-enc`                       | Run encoder only                  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-dec`                       | Run decoder only                  |
   +-----------------------------------+-----------------------------------+
   | :exc:`-encdec`                    | Run encoder and decoder           |
   +-----------------------------------+-----------------------------------+

**Examples**

.. note::
   The encoder output and the decoder input for this codec may have a format different from Bluetooth format.

The following command encodes the
**test_vector_8000.raw** stream with a frame length of 2056 samples:

.. code:: shell

   mdb -cl -run -nsim -tcf=em9d_voice_audio ARC_CVSD_CODEC_app.elf -enc -F 2056 -i "../testvectors/inp/test_vector_8000.raw"  -o "../testvectors/test_vector_64000.cod"

The following command decodes the
**test_vector_64000.cod** stream with a frame length 4096 samples:

.. code:: shell

   mdb -cl -run -nsim -tcf=em9d_voice_audio ARC_CVSD_CODEC_app.elf -dec -F 4096 -i "../testvectors/ref/test_vector_64000.cod"  -o "../testvectors/test_vector_8000.out"

The following command encodes and decodes the
**test_vector_8000.raw** to **test_vector_8000.out** stream
with a frame length of 4096 samples:

.. code:: shell

   mdb -cl -run -nsim -tcf=em9d_voice_audio ARC_CVSD_CODEC_app.elf -encdec -F 4096 -i "../testvectors/ref/test_vector_8000.raw"  -o "../testvectors/test_vector_8000.out"
