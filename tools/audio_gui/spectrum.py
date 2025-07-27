import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QFrame
from PyQt5.QtCore import QThread, pyqtSignal
import numpy as np
import pyqtgraph as pg
import struct
import serial

# Define a custom thread to generate random FFT results
class SpectrumThread(QThread):
    dataReceived = pyqtSignal(list)

    def __init__(self, serial_instance):
        super().__init__()
        self.serial_instance = serial_instance

    def run(self):
        fft_size = 256
        fft_frame = []
        frame_counter = 0
        while True:
            try:
                print(frame_counter)
                fft_value = self.read_float_data()
                if fft_value is None:
                    print("none")
                    pass
                else:
                    print("data float readen")
                    fft_frame.append(fft_value)
                    frame_counter += 1
                    if frame_counter >= fft_size:
                        print(fft_frame)
                        self.dataReceived.emit(fft_frame[:])  # Emit a copy of the list
                        fft_frame.clear()
                        frame_counter = 0
                
            except Exception as e:
                print("Error:", e)
                break

    def read_float_data(self):
        # Read 4 bytes (size of a float) and unpack them as a float
        float_bytes = self.serial_instance.read(4)
        if len(float_bytes) == 4:
            float_value = struct.unpack('f', float_bytes)[0]
            return float_value
        else:
            return None

class SpectrumScreen(QMainWindow):
    def __init__(self, parent=None, serial_instance=None):
        super(SpectrumScreen, self).__init__(parent)
        self.ser = serial_instance
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Spectrum Analysis Mode")
        self.setGeometry(100, 100, 800, 600)

        # Create a frame to contain the graph
        self.frame = QFrame()
        self.frame.setStyleSheet("""
            QFrame {
                background-color: #1F232A;
                border: 2px solid #007ACC;
                border-radius: 10px;
            }
        """)

        layout = QVBoxLayout(self.frame)

        # Create a plot widget
        self.plotWidget = pg.PlotWidget(background='#1F232A')  # Set background color same as widget
        self.plotWidget.showGrid(x=True, y=True)  # Add grids
        self.plotWidget.setLabel('left', 'Amplitude', color='k' ,size ='10pt')  # Set label for y-axis
        self.plotWidget.setLabel('bottom', 'Frequency (Hz)', color='k' ,size ='10pt')  # Set label for x-axis

        # Set static x-axis range
        fft_size = 256
        resolution = 61.5
        freq = np.arange(fft_size) * resolution  # Calculate frequency bins
        self.plotWidget.setXRange(freq[0], freq[-1])
        self.plotWidget.setYRange(0, 20000, padding=0)
        self.plotWidget.setTitle("Audio FFT Spectrum ", color='w', size='20pt')
        layout.addWidget(self.plotWidget)
        self.setCentralWidget(self.frame)

        # Initialize and start the SpectrumThread
        self.spectrumThread = SpectrumThread(serial_instance=self.ser)
        self.spectrumThread.dataReceived.connect(self.updatePlot)
        self.spectrumThread.start()

    def updatePlot(self, fft_data):
        fft_data[0] = 0
            # Set the second half of fft_data list to 0
        for i in range(len(fft_data)):
            if fft_data[i] < 3000  :
                        fft_data[i] = 0
        midpoint = len(fft_data) // 2
        for i in range(midpoint, len(fft_data)):
                fft_data[i] = 0
        # Plot FFT data
        resolution = 61.5  # FFT resolution
        freq = np.arange(len(fft_data)) * resolution  # Calculate frequency bins
        self.plotWidget.clear()
        self.plotWidget.plot(freq, np.abs(fft_data), pen=pg.mkPen('b'))  # Plot frequency vs. amplitude

if __name__ == "__main__":
    app = QApplication(sys.argv)

    # Create a dummy serial instance for testing
    class DummySerial:
        def read(self, n):
            return struct.pack('f', np.random.rand())

    dummy_serial = DummySerial()

    spectrumScreen = SpectrumScreen(serial_instance=dummy_serial)
    spectrumScreen.show()
    sys.exit(app.exec_())
