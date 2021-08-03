embARC Audio
=====
![](https://foss-for-synopsys-dwc-arc-processors.github.io/images/icons/icon_audio.jpg)  
###### This repository contains source code of voice and audio codecs, test library and documentation.
## Release notes
The following codecs were tuned for using MWDT compiler on processors families ARC EM/HS4xD voice audio:
- BT-CVSD
- G.711  
- G.722 Appendix IV
- G.726
- GSM-FR codec
- Sony LDAC encoder

## Package structure
[doc](/doc) - contains the API documentation of the embARC Audio Codecs library  
[BT-CVSD](/cvsd) - Bluetooth CVSD codec  
[G.711](/g711) - G.711 codec  
[G.722](/g722) - G.722 codec  
[G.726](/g726) - G.726 codec  
[GSM-FR](/gsm_fr) - GSM-FR codec  
[LDAC encoder](/ldac_encoder) - Sony LDAC encoder  
[rules](/rules) - Common parts of makefiles for build and run codecs  
[Testlib](/testlib) - test library for benchmarking codecs  

## Prerequisites
1. Installed MetaWare Development Toolkit P-2019.09 or newer 
2. Valid license for MetaWare Development compiler and debugger
3. HW platform shall support accumulator preshift option (-Xdsp_itu) for codecs based on ITU-T BASOP, with REMAPITU_T=on.

## Supported platforms
For codecs where conformance (bit exactness tests) test suite is required to pass following ARCv2 DSP platforms are tested:
* em5d voice audio;
* em7d voice audio;
* em9d voice audio;
* em11d voice audio;
* hs45d voice audio;  

## Getting started
All codecs follow a general approach to build and run a codec application. For more information see in the README.md of a specific codec.
#####  Build and run procedure for default platform:
1. Open command line in folder of specific codec
2. `gmake cleanall`
3. `gmake all`
4. `gmake run`  

##### Build and run procedure for specific HW platform
EM9D voice audio HW template is used for example.
1. Open command line in folder of specific codec
2. `gmake cleanall`
3. `gmake all TCF=em9d_voice_audio`
4. `gmake run TCF=em9d_voice_audio`

At the end of successful execution of codec application the output log is expected to contain a string:  
`FC: no differences encountered`
## Known Issues
None

## Contact information
* Aleksandr Kiiaev <Alexander.Kiyaev@synopsys.com>
* Kirill Arestov <Kirill.Arestov@synopsys.com>
* Ilya Pozdnov <Ilya.Pozdnov@synopsys.com>
