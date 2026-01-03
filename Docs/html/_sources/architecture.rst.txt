Architecture
============

Signal pipeline
---------------

The firmware implements a straight-through processing chain from microphone
input to host visualization.

.. graphviz::

   digraph pipeline {
     rankdir=LR;
     node [shape=box, style="rounded,filled", fillcolor=lightgray];
     Mic [label="PDM Mic"];
     DMA [label="DMA Capture"];
     Decim [label="PDM Decimation\n+ Low-pass"];
     PCM [label="PCM Buffer"];
     FFT [label="FFT (CMSIS-DSP)"];
     USB [label="USB CDC/UART"];
     GUI [label="Python GUI"];
     Mic -> DMA -> Decim -> PCM -> FFT -> USB -> GUI;
   }

Task and queue model
--------------------

FreeRTOS queues move audio frames between tasks. The exact task names may
vary depending on the firmware configuration, but the topology is consistent.

.. graphviz::

   digraph tasks {
     rankdir=LR;
     node [shape=box, style="rounded,filled", fillcolor=lightyellow];
     PDM [label="PDM Task"];
     FFT [label="FFT Task"];
     OUT [label="USB/UART Task"];
     Q1 [label="Queue: PDM->PCM", shape=ellipse, fillcolor=lightblue];
     Q2 [label="Queue: PCM->FFT", shape=ellipse, fillcolor=lightblue];
     Q3 [label="Queue: FFT->Host", shape=ellipse, fillcolor=lightblue];
     HOST [label="PC Host", shape=box, fillcolor=lightgray];
     PDM -> Q1 -> FFT -> Q2 -> OUT -> Q3 -> HOST;
   }
