Firmware
========

PDM capture and decimation
--------------------------

- The PDM microphone is sampled via DMA to minimize CPU usage.
- A decimation filter converts the 1-bit PDM stream into PCM frames.
- A low-pass filter removes out-of-band noise introduced by decimation.

FFT processing
--------------

- PCM frames are windowed and transformed using CMSIS-DSP.
- Typical FFT sizes are 256, 512, or 1024 points.
- Magnitude scaling is applied before streaming to the host.

Output streaming
----------------

- Spectral data is transmitted over USB CDC or UART.
- The host GUI interprets the stream and renders plots in real time.
