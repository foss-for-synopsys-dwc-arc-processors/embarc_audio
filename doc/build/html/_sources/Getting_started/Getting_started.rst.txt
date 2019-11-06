Getting Started
---------------

Required Tools
~~~~~~~~~~~~~~

1. Installed MetaWare Development Toolkit P-2019.06 or newer
2. Valid license for MetaWare Development Compiler and Debugger

Clone Repository
~~~~~~~~~~~~~~~~

Clone repository to the :exc:`<target_build>/<codec_name>` folder

Build and Run
~~~~~~~~~~~~~

To build and run the codec, follow these steps:

1. Navigate to the audio codec directory: :exc:`<target_build>/<codec_name>/make`  

2. Type the following commands: 

.. code:: shell

  gmake cleanall 
  gmake all 
  gmake run
..

In step 2 -
 
  - The first command cleans all intermediate and target files.
 
  - The second command builds both static and dynamic libraries. 
    Test application executable file is :exc:`<test_app_executable>.elf`. 

  - The third command runs the application with default command line parameters.

If the execution is successful, the output log contains the string
:exc:`FC: no differences encountered`

Makefile Commands
^^^^^^^^^^^^^^^^^

The following table lists available Makefile commands:

.. table:: Makefile Commands
   :align: center
   :widths: 30, 130
   
   +------------------------------------------+-----------------------------+
   | **Command**                              | **Description**             |
   +==========================================+=============================+
   | :exc:`gmake`                             | These commands build both   |
   | :exc:`gmake all`                         | archive and static          |
   |                                          | libraries and test          |
   |                                          | application executable      |
   |                                          | files                       |
   +------------------------------------------+-----------------------------+
   | :exc:`gmake lib`                         | This command builds both    |
   |                                          | archive and static          |
   |                                          | libraries                   |
   +------------------------------------------+-----------------------------+
   | :exc:`gmake app`                         | This command builds test    |
   |                                          | application executable      |
   |                                          | files                       |
   +------------------------------------------+-----------------------------+
   | :exc:`gmake clean`                       | This command cleans all     |
   |                                          | intermediate files          |
   +------------------------------------------+-----------------------------+
   | :exc:`gmake cleanall`                    | This command cleans all     |
   |                                          | intermediate and target     |
   |                                          | files                       |
   +------------------------------------------+-----------------------------+
   | :exc:`gmake size`                        | This command shows size of  |
   |                                          | sections used by the static |
   |                                          | library or size of ELF if   |
   |                                          | build with LTO              |
   +------------------------------------------+-----------------------------+
   | :exc:`gmake run`                         | These commands run test     |
   | :exc:`[gmake run_encoder]`               | application (or only        |
   | :exc:`[gmake run_decoder]`               | encoder/decoder, the        |
   |                                          | presence of these commands  |
   |                                          | depends on specific codec)  |
   +------------------------------------------+-----------------------------+

Build Directory Structure for Specific Audio Codec
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following table represents the directory structure for the
build-related files and the associated documents (all codecs have the
same directory structure):

.. table:: Directory Structure for Specific Audio Codec
   :align: center
   :widths: 30, 180
   
   +---------------+--------------------------------+
   | **Directory** | **Description**                |
   +===============+================================+
   | include       | Header files                   |
   +---------------+--------------------------------+
   | lib           | Libraries                      |
   +---------------+--------------------------------+   
   | make          | Makefiles and executable files |
   +---------------+--------------------------------+   
   | obj           | Object files                   |
   +---------------+--------------------------------+   
   | src           | Codec source files             |
   +---------------+--------------------------------+   
   | test          | Test applications source files |
   +---------------+--------------------------------+   
   | testvectors   | Test vectors                   |
   +---------------+--------------------------------+   


Build Parameters
^^^^^^^^^^^^^^^^

To build the library with your own parameters, use the following command:

.. code:: shell

   gmake all <common_parameters> <specific_parameters>
..

where:
  :exc:`<common_parameters>` are the build parameters that are common to all codecs. 
  
  :exc:`<specific_parameters>` are the build parameters that are specific to each codec. 
  
See the codec-specific documentation for more details.

The following table enlists the common parameters for codecs:

.. table:: Common Build Parameters for All Codecs
   :align: center
   :widths: 30, 130
   
   +-----------------------------------+------------------------------------+
   | **Option**                        | **Description**                    |
   +===================================+====================================+
   | :exc:`TCF=\<platform\>`           | Specifies the target platform      |
   |                                   | template                           |
   +-----------------------------------+------------------------------------+
   | :exc:`LTO_BUILD=\<on/off\>`       | Turn on/off Link-time              |
   |                                   | optimization                       |
   +-----------------------------------+------------------------------------+
   | :exc:`REMAPITU_T=\<on/off\>`      | Turn on/off remapping for ITU_T    |
   |                                   | basop (except LDAC encoder).       |
   |                                   | Accumulator preshift option        |
   |                                   | (:exc:`-Xdsp_itu`) is necessary,   |
   |                                   | with :exc:`REMAPITU_T=on`. If it   |
   |                                   | is not supported by default you    |
   |                                   | should add -Xdsp_itu into CFLAGS   |
   |                                   | (For example,                      |
   |                                   | :exc:`CFLAGS+= -Xdsp_itu`)         |
   +-----------------------------------+------------------------------------+
   | :exc:`DEBUG_MODE=\<on/off\>`      | Turn on/off debug mode (:exc:`-g`  |
   |                                   | :exc:`-Hanno` :exc:`-Hkeepasm` )   |
   |                                   | See [1] in :ref:`Refs`.            |
   +-----------------------------------+------------------------------------+

Code was tested with following hardware templates: 
 - em5d voice audio
 - em7d voice audio
 - em9d voice audio
 - em11d voice audio
 - hs45d voice audio

The following is an example of a command to build a codec without :ref:`LTO`\, legacy 
basop32 library under EM9D voice audio platform:

.. code:: shell

   gmake all TCF=em9d_voice_audio LTO_BUILD=off REMAPITU_T=off

Run Parameters
^^^^^^^^^^^^^^

To run the codec test application with your own parameters, use the following command:

.. code:: shell

   mdb <mdb parameters> <test_app_executable>.elf <test_app_executable_params>

where:

  :exc:`<mdb_parameters>` are debugger/platform parameters, which may include:

  - details of platform configuration for software simulator 

  - paths to drivers and details of JTAG interface configuration. 

  These parameters do not depend on the particular codec component specifics, 
  except hardware configuration that must match the hardware expected by the 
  component. 

For documentation on details on MDB and relevant software and hardware 
configuration, see [2] and [3] in :ref:`Refs`.

**<test_app_executable>.elf** is a binary image of an executable file to
be loaded by the debugger.

:exc:`<test_app_executable_params>` is a command line for the test
application. Details of the test application command lines depend on the 
particular test application, and do not depend on the MDB/hardware
configuration. Usually, these parameters provide test-application
specific information, such as I/O file names and information of their
format interpretation and codec-specific configuration information. For
details on command lines for specific codec test applications, see
the command line option descriptions of the test application in
the subsequent sections. Additionally, see the Reference guide document
for codec-specific structure fields of an individual component.

:exc:`<mdb parameters>` usually is :exc:`-run -cl -nsim` as shown in the following sample command:

.. code:: shell

   mbd -run -cl -nsim <test_app_executable>.elf <test_app_executable_params>

To invoke GUI interface of debugger, omit options -run and -cl:

.. code:: shell

   mbd -nsim <test_app_executable>.elf <test_app_executable_params>

For more information about build and run options, see the codec-specific
documentation.

Test Sequences
~~~~~~~~~~~~~~

Some codecs have test sequences for checking the bit-exactitude of the
result: G.711, G.722, G.726, GSM-FR. All of them
completely passed the given test sequences on the `specified
platforms <#build-parameters>`__.

Modify the Hardware Configuration File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After unpacking archives, edit the hardware configuration file located 
in **<target_build>/rules/common_hw_config.mk** to ensure that it 
corresponds to the target hardware platform.

The default hardware platform is specified by one of the standard tcf
files provided by MetaWare. Each core has a separate TCF file. For
example:
:exc:`-tcf=hs45d_voice_audio`

If the target hardware platform differs from the default, best practice 
is to use a reference to the TCF file generated by the
ARChitect for your project:
:exc:`-tcf=/build/tool_config/arc.tcf`

Usage of low-level hardware configuration options in **<target_build>/rules/common_hw_config.mk** file
(like :exc:`-Xbarrel_shifter`) is possible but not recommended. In this case, 
the corresponding C/C++ Preprocessor macro definitions are required for
successfully building the codec. (For example, :exc:`-Dcore_config_barrel_shifter=1`).

Additionally, the key option :exc:`-tcf_core_config` automatically enables all hardware
related C/C++ preprocessor macro definitions when using TCF and
you should not explicitly define them. See MetaWare the Debugger 
documentation for more details.

The option :exc:`-tcf` specifies the tool configuration file that is generated
when you build a processor by using the ARChitect configuration tool that
uses a newer version of the ARC IP libraries. The TCF file
encapsulates the information that development tools need, to support a
given processor build into a single file. After you have built the
processor using the ARChitect tool, the TCF file is created in the
following location of your ARChitect processor project
folder:\ **<hardware_platform_project>/build/tool_config/arc.tcf**.
For more details, see the MetaWare Debugger documentation.

.. _Refs:

References
~~~~~~~~~~

1. DesignWare® C/C++ Programmer’s Guide for the ccac Compiler for the
   ARC EM, ARC HS and EV6x Processors
2. DesignWare® MetaWare Debugger User’s Guide for ARC EM, ARC HS, and
   EV6x Processors
3. DesignWare® MetaWare Toolkit Quick Start Guide For ARC EM, ARC HS,
   and EV6x Processors
