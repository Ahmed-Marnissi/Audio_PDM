Overview
========

This project implements a real-time audio processing pipeline on the
STM32F407 Discovery board. The on-board PDM microphone is converted to PCM
and analyzed with an FFT. Results are streamed to a PC for visualization
through a Python GUI.

Key features
------------

- PDM to PCM decimation with low-pass filtering.
- Real-time FFT analysis using CMSIS-DSP.
- USB CDC or UART data output for host visualization.
- PyQt5 GUI for playback and spectrum modes.

Hardware and software
---------------------

- STM32F407 Discovery board with integrated PDM microphone.
- STM32 HAL drivers and CMSIS-DSP.
- Python GUI in `tools/audio_gui` with PyQt5 and pyserial.

Workflow at a glance
--------------------

#. Capture PDM via DMA.
#. Convert PDM to PCM with decimation and low-pass filtering.
#. Run FFT on PCM frames.
#. Stream results to the host and visualize in the GUI.
