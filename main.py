import sys

import speech_recognition as sr
import keyboard as kb
import yaml
import time
import enum

from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPalette

# status enum
class Status(enum.Enum):
    IDLE = { "colour" : Qt.black, "text" : "Idle" }
    LISTENING = { "colour" : Qt.blue, "text" : "Listening..." }
    PROCESSING = { "colour" : Qt.red, "text" : "Processing..." }
    EXECUTING = { "colour" : Qt.green, "text" : "Executing!" }

class status_box(QLineEdit):
    def __init__(self, *args, **kwargs):
        QLineEdit.__init__(self, *args, **kwargs)

        self.setReadOnly(True)
    
    def setStatus(self, status: Status):
        palette = QPalette()
        palette.setColor(QPalette.Text, status.value["colour"])
        self.setPalette(palette)
        self.setText(status.value["text"])

class output_box(QPlainTextEdit):
    def __init__(self, *args, **kwargs):
        QPlainTextEdit.__init__(self, *args, **kwargs)

        self.setReadOnly(True)

def format_command(command):
    return command.replace("-", " ").lower()

class hdvs(QMainWindow):
    def __init__(self, parent=None):
        super(QMainWindow, self).__init__(parent)

        self.setWindowTitle("Helldivers Voice Stratagem")
        layout = QVBoxLayout()

        status = status_box()
        layout.addWidget(status)
        self.status = status

        outbox = output_box()
        layout.addWidget(outbox)
        self.outbox = outbox

        main = QWidget()
        main.setLayout(layout)
        self.setCentralWidget(main)

        print("Starting stratagem recogition")
        print("SpeechRecognition Version: {0}".format(sr.__version__))

        print("Loading config...")
        with open("stratagem.yml", 'r') as file:
            self.stratagems = yaml.safe_load(file)

        with open("config.yml", 'r') as file:
            self.config = yaml.safe_load(file)

        self.strat_key = self.config["keys"]["stratagem"]

        self.recog = sr.Recognizer()
        self.mic = sr.Microphone()
        print("Calibrating for ambient noise...")
        with self.mic as source:
            self.recog.adjust_for_ambient_noise(source)

        kb.add_hotkey(self.strat_key, self.listen)

        print("Ready")

        self.status.setStatus(Status.IDLE)

    def execute_stratagem(self, stratagem):
        self.status.setStatus(Status.EXECUTING)
        config = self.config
        self.myprint("Executing {0}.".format(stratagem["name"]))
        for key in stratagem["code"]:
            if key == "U":
                kb.send(config["keys"]["up"])
            elif key == "D":
                kb.send(config["keys"]["down"])
            elif key == "L":
                kb.send(config["keys"]["left"])
            else:
                kb.send(config["keys"]["right"])
        
            # Ignore on last?
            time.sleep(config["dialling-speed"])


    def interpret_stratagem(self, command):
        command = format_command(command)

        for stratagem in self.stratagems:
            if not stratagem["enabled"]:
                continue

            # Could be accomplished using a trigger map
            # Has not been done in yaml file for readability
            for trigger in stratagem["trigger"]:
                if command == format_command(trigger):
                    self.myprint("Found match: {0}".format(stratagem["name"]))
                    self.execute_stratagem(stratagem)
                    return True
        return False

    def listen(self):
        recog = self.recog
        status = self.status

        while True:
            status.setStatus(Status.LISTENING)
            self.myprint("Listening...")
            with self.mic as source:
                audio = recog.listen(source)
    
            self.myprint("Input recieved, converting... ")
            status.setStatus(Status.PROCESSING)
            try:
                command = recog.recognize_google(audio)
                self.myprint("Heard: {0}".format(command))

                if self.interpret_stratagem(command):
                    break
                else:
                    self.myprint("{0} was not a valid stratagem.".format(command))

            except sr.UnknownValueError:
                self.myprint("Could not understand audio.")
            except sr.RequestError as e:
                self.myprint("Could not decode, recieved error: {0}".format(e))
        
            if not kb.is_pressed(self.strat_key):    # Break if strategem key no longer held
                self.myprint("Stratagem key not pressed, listening stopped.")
                break
        
        status.setStatus(Status.IDLE)
    
    def myprint(self, text: str):
        self.outbox.insertPlainText(text + "\n")
    
    def exec(self, stratagem_file, config_file):
        self.app.exec_()




app = QApplication(sys.argv)
window = hdvs()
window.show()

app.exec_()