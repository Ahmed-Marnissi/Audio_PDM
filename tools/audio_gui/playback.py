import sys
import threading
import wave 
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel
from PyQt5.QtCore import Qt, QThread
from PyQt5.QtGui import QIcon, QMovie
from PyQt5.QtMultimedia import QSoundEffect
from PyQt5.QtCore import QUrl
import serial
import struct
import time
class RecordThread(QThread):
    def __init__(self, serial_instance):
        super().__init__()
        self.serial_instance = serial_instance
        self.running = True
    def run(self  ) :
        pcm_file = "pcm_stm32.txt"
        with open(pcm_file, "w") as file:
            while True:
                # Place the recording logic here
                print("Recording...")
                float_data = self.read_float_data( )
                if float_data is not None:
                     # Write float data to file
                    file.write(str(float_data) + "\n")

                if self.isInterruptionRequested():
                    print("Recording stopped")
                    break
    def stop(self):
        self.running = False

    def read_float_data(self  ):
        # Read 4 bytes (size of a float) and unpack them as a float
        float_bytes = self.serial_instance .read(4)
        if len(float_bytes) == 4:
            float_value = struct.unpack('f', float_bytes)[0]
            return float_value
        else:
            return None
class PlaybackScreen(QWidget):
    def __init__(self, parent=None, serial_instance=None):
        super(PlaybackScreen, self).__init__(parent)
        self.ser = serial_instance
        self.initUI()

    def initUI(self):
        main_layout = QVBoxLayout()
        main_layout.setSpacing(20)

        self.setStyleSheet("""
            QWidget {
                background-color: #1F232A;
            }
            QPushButton {
                background-color: #007ACC;
                color: #FFFFFF;
                border: none;
                border-radius: 10px;
                font-size: 18px;
                padding: 10px;
                min-width: 120px;
                margin: 0 10px;
            }
            QPushButton:hover {
                background-color: #0056b3;
            }
        """)
        
         # Title
        self.title_label = QLabel("PlayBack", self)
        # Animation area
        self.animation_label = QLabel(self)
        self.animation_label.setAlignment(Qt.AlignCenter)
        main_layout.addWidget(self.title_label, alignment=Qt.AlignCenter)
        main_layout.addWidget(self.animation_label)
        
        # Set an animated GIF (but don't start animation yet)
        self.movie = QMovie("pics/wav.gif")
        self.animation_label.setMovie(self.movie)
        
        self.movie.start()
        self.movie.setPaused(True)
        # Buttons layout
        button_layout = QHBoxLayout()
        button_layout.setSpacing(10)

        # Start Record Button
        self.startRecordButton = QPushButton("Start Record", self)
        self.startRecordButton.setIcon(QIcon('pics/start_record.png'))
        self.startRecordButton.clicked.connect(self.startRecording)
        button_layout.addWidget(self.startRecordButton)

        # Stop Record Button
        self.stopRecordButton = QPushButton("Stop Record", self)
        self.stopRecordButton.setIcon(QIcon('pics/pause_record.png'))
        button_layout.addWidget(self.stopRecordButton)

        # Save Button
        self.saveButton = QPushButton("Save", self)
        self.saveButton.setIcon(QIcon('pics/save_record.png'))
        button_layout.addWidget(self.saveButton)
        self.saveButton.clicked.connect(self.SaveRecord)

        # Play Button
        self.playButton = QPushButton("Play", self)
        self.playButton.setIcon(QIcon('pics/play_record.png'))
        button_layout.addWidget(self.playButton)
        self.playButton.clicked.connect(self.PlayRecord)

        # Add button layout to the main layout
        main_layout.addLayout(button_layout)

        # Set main layout
        self.setLayout(main_layout)

    def startRecording(self):
        self.record_thread = RecordThread(serial_instance=self.ser )
        self.record_thread.start()
    def stopRecording(self):
        if hasattr(self, 'record_thread') and self.record_thread.isRunning():
            self.record_thread.terminate()
    def get_arm_signal(self , txt_file):
        with open(txt_file, "r") as f:
            data = f.read()
        signal = data.split() 
        signal = map(float, signal)
        return signal
    
    def SaveRecord ( self ) : 
            #smaplign freq
            sampling_f = 16000 
            pcm_file="pcm_stm32.txt"
            signal = self.get_arm_signal(pcm_file)  
            # Define WAV file path
            wav_file = "recorded.wav"
            # Open WAV file for writing
            wav = wave.open(wav_file, "w")
            wav.setparams((1,                   # nchannels
                        2,                   # sampwidth
                        sampling_f,          # framerate 
                        0,                   # nframes
                        "NONE",              # comptype
                        "not compressed"     # compname
                        ))
            packed = []
            # Write samples to WAV file
            for sample in signal:
                sample = sample *100
                # Ensure sample falls within 16-bit range
                if sample > 32767:
                    sample = 32767
                if sample < -32768:
                    sample = -32768
                packed.append(struct.pack("h", int(sample)))
            print (packed )
            wav.writeframes(b"".join(packed))
            wav.close()

            # Open WAV file for reading and print parameters
            wav = wave.open(wav_file, "r")
            print(wav.getparams())  
            wav.close()
    def PlayRecord(self):
        self.movie.start()
        # Create a QSoundEffect instance
        self.soundEffect = QSoundEffect(self)
        # Load the WAV file
        self.soundEffect.setSource(QUrl.fromLocalFile("recorded.wav"))
        # Play the sound effect
        self.soundEffect.play()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    playbackScreen = PlaybackScreen()
    playbackScreen.show()
    sys.exit(app.exec_())









