Audio Codecs
------------

An audio codec is a hardware device or computer program capable of encoding
or decoding a digital audio signal including music and human speech. 
In software terms, an audio codec is a computer program implementing 
an algorithm that compresses and decompresses digital audio data. 

Telecommunications codecs had written using ITU-T/ETSI BASOPs for elementary 
operations require bit-exact operations.

The objective of the algorithm is to represent the audio
signal with minimum number of bits while retaining acceptable
perceptual quality characteristics. This reduces the storage or the
bandwidth requirements to store or transmit digital audio.
The embARC Audio codecs are set of software libraries implementing 
legacy and commonly used codecs standardized by
`ETSI <https://www.etsi.org/>`__/`3GPP <https://www.3gpp.org/>`__ and
`ITU-T <https://www.itu.int/en/ITU-T/about/Pages/default.aspx>`__. 

DSP algorithms can be implemented using different data types, 
can come from different sources, can have different requirements
and can evolve differently. 
Some usual cases are: 

 - Porting of integer C code using standard C integer types 
 - Porting of C code using floating-point (either using a floating-point 
   unit or emulation) 
 - Porting fixed-point algorithms written with C integer types and 
   operations 
 - Porting fixed-point algorithms written using special macros for 
   fixed-point multiplication, multiply-add operations, and so on

   
Codecs Supported by embARC Audio
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ETSI/3GPP Codecs 
^^^^^^^^^^^^^^^^

Codecs of this group are designed to transmit speech over a cellular network. 
GSM Full Rate (GSM-FR) was the first digital speech coding standard used in
early deployments of GSM digital mobile phone systems. The GSM-FR
bitrate is 13 kbps and has very moderate MIPS/memory requirements.

ITU-T Codecs
^^^^^^^^^^^^

These codecs are implementations of `G series of ITU-T
standards <https://www.itu.int/rec/T-REC-G/en>`__. They are designed
for voice transmission over Plain Switched Telephone Networks, Voice
over IP, and mobile networks. 

Bluetooth CVSD 
^^^^^^^^^^^^^^

Continuously Variable Slope Delta Modulation (BT-CVSD) is a mandatory 
codec for Bluetooth to transmit speech at 8 kHz sampling rate at 64 kbps. 

Sony LDAC 
^^^^^^^^^

LDAC is an audio coding technology developed by
`Sony <https://www.sony.net/Products/LDAC/>`__, which allows streaming
audio over Bluetooth connections up to 990 kbps at 24 bit/96 kHz (also
called high-resolution audio). It is used by various Sony products.
 
Optimizations
~~~~~~~~~~~~~

ITU-T Basop Optimization
^^^^^^^^^^^^^^^^^^^^^^^^

To improve performance of ITU-T and ETSI/3GPP codecs at early optimization 
step, use Synopsys MetaWare Development Tools which efficiently replace  
`ITU-T G.191 STL basic operators <https://www.itu.int/rec/T-REC-G.191/en>`__ 
with ARCv2 fixed point intrinsics at compile time.

.. _LTO:

Link Time Optimization 
^^^^^^^^^^^^^^^^^^^^^^

Link Time Optimization (LTO) is an optimization technique of intermodular 
optimization performed during the link stage. 

Compiler Optimization Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 
To increase the performance of the code, use the following compiler optimizing 
options:
 
 - :exc:`Hinline_threshold` 
 - :exc:`Haggressive_unroll` 
 - :exc:`Hmisched` 
 - :exc:`Hunswitch` 
 - :exc:`Hmax_predicate` 
 - :exc:`Hdense_prologue` 
 - :exc:`fslp-vectorize-aggressive` 
 - :exc:`funroll-loops` 
 - :exc:`Hon=Zd_loops`
 - :exc:`Hoff=Zd_loops` 
 - :exc:`Hinlsize` 
 - :exc:`Hunroll` 
 - :exc:`Hloop_sms` 
 
For more details, see [1] in :ref:`Refs`.

Hmerge 
^^^^^^

This optimization option is similar to LTO. The difference
is Hmerge occurs at the compilation stage, not at the linking 
stage. In this case, all the source code is collected in a
single object file. 

Supported Codecs
~~~~~~~~~~~~~~~~

.. toctree::
   :maxdepth: 2
   :caption: ETSI/3GPP codecs:
   
   GSM-FR_README.rst
   
.. toctree::
   :maxdepth: 2
   :caption: ITU-T G codecs:

   G_711_README.rst
   G_722_README.rst
   G_726_README.rst
   
.. toctree::
   :maxdepth: 2
   :caption: Others:
   
   BT-CVSD_README.rst
   LDAC_encoder_README.rst

