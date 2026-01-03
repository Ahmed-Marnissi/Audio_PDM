Build and Run
=============

Firmware build
--------------

#. Open the project in STM32CubeIDE or Keil uVision.
#. Build the firmware and flash the STM32F407 Discovery board.
#. Connect the board to the PC via USB (CDC) or UART.

GUI setup
---------

#. Navigate to `tools/audio_gui`.
#. Install dependencies:

   .. code-block:: console

      pip install pyqt5 pyserial

#. Start the GUI:

   .. code-block:: console

      python main.py

Verification checklist
----------------------

- The COM port appears in the GUI drop-down list.
- Selecting a mode sends the correct device command.
- FFT data updates live in spectrum mode.

Documentation build
-------------------

From the repository root:

.. code-block:: console

   pip install sphinx sphinx_rtd_theme
   sphinx-build -b html Docs/sphinx Docs/sphinx/_build/html
