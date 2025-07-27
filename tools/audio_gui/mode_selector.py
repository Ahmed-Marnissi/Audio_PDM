import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QPushButton, QLabel, QComboBox, QLineEdit
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon 
import serial
import serial.tools.list_ports
from playback import PlaybackScreen
from spectrum import SpectrumScreen 
class ModeSelector(QWidget):
    def __init__(self, parent=None):
        super(ModeSelector, self).__init__(parent)
        self.serialPort = None
        self.initUI()

    def initUI(self):
        layout = QVBoxLayout()
        self.setStyleSheet("""
            QWidget {
                background-color: #1F232A;
            }
            QLabel {
                color: #FFFFFF;
                font-size: 24px;
                font-weight: bold;
            }
            QPushButton {
                background-color: #007ACC;
                color: #FFFFFF;
                border: none;
                border-radius: 10px;
                font-size: 18px;
                padding: 10px;
                min-width: 200px;
                margin: 5px;
            }
            QPushButton:hover {
                background-color: #0056b3;
            }
            QComboBox {
                background-color: #1F232A;
                color: #FFFFFF;
                border: 1px solid #007ACC;
                padding: 5px;
                font-size: 18px;
                min-width: 200px;
            }
            QLineEdit {
                background-color: #1F232A;
                color: #FFFFFF;
                border: 1px solid #007ACC;
                padding: 5px;
                font-size: 18px;
                min-width: 200px;
            }
        """)

        # Title
        self.title_label = QLabel("STM32 Audio ", self)
        layout.addWidget(self.title_label, alignment=Qt.AlignCenter)

        # Spacer
        layout.addSpacing(20)

        # Mode Selection Buttons
        self.playbackButton = QPushButton("  Playback Mode", self)
        self.playbackButton.setIcon(QIcon('pics/start_record.png'))
        self.playbackButton.clicked.connect(self.onPlayback)
        layout.addWidget(self.playbackButton, alignment=Qt.AlignCenter)

        self.spectrumButton = QPushButton(" Spectrum Analysis Mode", self)
        self.spectrumButton.setIcon(QIcon('pics/spectrum.png'))
        self.spectrumButton.clicked.connect(self.onSpectrum)
        layout.addWidget(self.spectrumButton, alignment=Qt.AlignCenter )

        # Spacer
        layout.addSpacing(20)

        # ComboBox for USB VCP selection
        self.usbComboBox = QComboBox(self)
        self.populate_com_ports()
        layout.addWidget(self.usbComboBox, alignment=Qt.AlignCenter)

        # Connect Button
        self.connectButton = QPushButton("Connect", self)
        self.connectButton.clicked.connect(self.onConnect)
        layout.addWidget(self.connectButton, alignment=Qt.AlignCenter)

        # Message Line Edit
        self.messageLineEdit = QLineEdit(self)
        self.messageLineEdit.setReadOnly(True)
        self.messageLineEdit.setText("Connect to...")
        layout.addWidget(self.messageLineEdit, alignment=Qt.AlignCenter)

        # Set layout
        self.setLayout(layout)

    def populate_com_ports(self):
        ports = serial.tools.list_ports.comports()
        if ports:
            for port, desc, hwid in sorted(ports):
                self.usbComboBox.addItem(f"{port} - {desc}")
        else:
            self.usbComboBox.addItem("No COM ports available")

    def onConnect(self):
        selected_usb = self.usbComboBox.currentText().split(' - ')[0]
        if selected_usb == "No COM ports available":
            self.messageLineEdit.setText("No COM ports available")
            return

        try:
            self.serialPort = serial.Serial(selected_usb, 9600)
            self.messageLineEdit.setText("Connection done")
        except serial.SerialException:
            self.messageLineEdit.setText("Connection failed")

    def sendMessage(self, message):
        if self.serialPort and self.serialPort.is_open:
            try:
                self.serialPort.write(message.encode())
                self.messageLineEdit.setText(f"Sent: {message}")
            except serial.SerialException as e:
                self.messageLineEdit.setText(f"Failed to send: {e}")
        else:
            self.messageLineEdit.setText("Port not connected")

    def onPlayback(self):
        print("Playback Mode Selected")
        self.sendMessage("PB_MODE")
        playback_screen = PlaybackScreen(serial_instance=self.serialPort, parent=self)
        self.parent().setCentralWidget(playback_screen )

    def onSpectrum(self):
        print("Spectrum Analysis Mode Selected")
        self.sendMessage("SP_MODE")
        spectre_screen = SpectrumScreen(serial_instance=self.serialPort, parent=self)
        self.parent().setCentralWidget(spectre_screen )
        # Switch to spectrum analysis screen (to be implemented)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("STM32 Audio Control")
        self.setGeometry(100, 100, 800, 600)  # Set window size to 800x600 pixels

        self.mode_selector = ModeSelector(self)
        self.setCentralWidget(self.mode_selector)

        self.setStyleSheet("""
            QMainWindow {
                background-color: #1F232A;
            }
        """)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
