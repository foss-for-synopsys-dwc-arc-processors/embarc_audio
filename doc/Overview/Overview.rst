.. _Overview: 

Overview
--------

.. _intro:

Introduction
~~~~~~~~~~~~

About the Project
^^^^^^^^^^^^^^^^^

ARC DSP and the MetaWare tools provide means to address different
scenarios through different options.

The required code transformations can be different, depending on the
type and quality of reference code and the desired result (optimization
targets) and thus require different development effort.

Out-of-the box optimization (OOB) technique targets minimal effort with
reasonable performance gain.

Targeting better optimizations requires more code transformations and
deeper optimizations: 

- Algorithmic optimizations, including changing the precision of output 
  (if applicable) 
  
- Data flow and memory-mapping optimizations that minimize memory usage 
  and access 
  
- Utilization of FXAPI primitives for fractional operations and mapping 
  of data types to enable usage of fractional DSP instructions by the 
  compiler 
  
- Re-writing code patterns, to make them more compiler and platform 
  friendly. In particular: 
  
  - Manual loop transformations (such as loops splitting to enable
    accumulator usage in MAC instructions)  

  - Using 32-bit data types (instead of accumulator data types) where 
    multiple, frequently accessed variables are needed. 
	  
In general, 32-bit data types provide the best efficiency cycle-wise and 
code-size-wise.

embARC Audio codec collection is an OOB (Out-Of-the Box) optimized library of speech and
audio codecs for
`ARCv2DSP <https://www.synopsys.com/designware-ip/processor-solutions/arc-processors/arc-dsp-solutions.html>`__.

For better optimization, contact Synopsys.

Out-Of-the-Box Optimizations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Out-Of-the Box optimization (OOB optimization) technique usually
achieves reasonable performance with minimal or no source code
modifications and thus minimal effort. This typically includes following
steps:

-  Replacing ETSI/ITU-T standard BASOP headers by Metware Development 
   Toolkit provided and excluding from compilation BASOP sources (C files)
   
-  Tuning the compiler options to find the best performance vs. code-size
   trade-offs. For global optimization, try :exc:`-Hmerge` or :exc:`-Hlto`:
   
   - :exc:`-Hmerge` requires compiling all files at once by specifying all C files
     in command line 

   - :exc:`-Hlto` (enables Link Time Optimization) allows file-by-file compilation 
     and linking. It uses higher-level intermediate representation in object 
     files instead of platform object code, allowing to proceed with global 
     optimizations at the “linking” step 

   Selection of the best options is an iterative step with several sets of 
   options. For more details, see [1] in :ref:`Refs`.

.. _terms_and_defs:
   
Terms and definitions
~~~~~~~~~~~~~~~~~~~~~

.. glossary::
   :sorted:

   AGU
      Address Generation Unit

   API 
      Application Programming Interface
  
   ARCv2DSP 
      Synopsys DesignWare® ARC® Processors Family of 32-bit CPUs 

   ARC EMxD 
      Family of 32-bit ARC Processor Cores. Single-core, 3-Step Pipeline, ARCv2DSP 

   ARC HS4xD 
      Family of 32-bit ARC Processor Cores. Multi-core, Dual-Issue, 10-Step Pipeline, ARCv2DSP
	  
   CCAC 
      MetaWare Compiler 

   DMA 
      Direct Memory Access 
	  
   DSP
      Digital Signal Processor 
	  
   FXAPI 
      Fixed-point API 
	 
   LTO 
      Link-Time Optimization 

   MAC
      Multiply-Accumulate 

   MDB 
      MetaWare Debugger

   MPY 
      Multiply Command 

   MWDT
      MetaWare Development Toolset
	  
   nSIM 
      Instruction Set Simulator
	  
   OOB
      Out-Of-the Box   

   PCM 
      Pulse Code Modulation 
   
   TCF
      Tool Configuration File. Holds information about ARC processor build configuration and extensions. 
	  
   xCAM 
      Cycle Accurate Model
	  
.. _Copyright:
  
Copyright
~~~~~~~~~

Copyright (c) 2019-present, Synopsys, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

* Neither the name Synopsys nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
