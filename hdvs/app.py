from PyQt5.QtWidgets import QMainWindow, QHBoxLayout, QWidget
from PyQt5.QtGui import QIcon

import speech_recognition as sr
import keyboard as kb

import time

from core import *

class hdvs(QMainWindow):
    def __init__(self, stratagems, config, parent=None):
        super(QMainWindow, self).__init__(parent)

        self.setWindowTitle("Helldivers Voice Stratagem")
        self.setWindowIcon(QIcon("Icon.webp"))

        print("Starting stratagem recogition")
        print("SpeechRecognition Version: {0}".format(sr.__version__))

        self.stratagems = stratagems
        self.config = config

        self.strat_key = self.config["keys"]["stratagem"]

        self.recog = sr.Recognizer()
        self.mic = sr.Microphone()
        print("Calibrating for ambient noise...")
        with self.mic as source:
            self.recog.adjust_for_ambient_noise(source)

        kb.add_hotkey(self.strat_key, self.listen)

        print("Ready")

        self.status = StatusWidg()
        self.stratagemopts = StratagemGroup(self.stratagems)

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.stratagemopts)
        mainLayout.addWidget(self.status)

        main = QWidget()
        main.setLayout(mainLayout)
        self.setCentralWidget(main)
        self.status.setStatus(Status.IDLE)

        self.active = False

    def execute_stratagem(self, stratagem):
        self.status.setStatus(Status.EXECUTING)
        config = self.config
        self.status.print("Executing {0}.".format(stratagem["name"]))
        for key in stratagem["code"]:
            if key == "U":
                kb.send(config["keys"]["up"])
            elif key == "D":
                kb.send(config["keys"]["down"])
            elif key == "L":
                kb.send(config["keys"]["left"])
            elif key == "R":
                kb.send(config["keys"]["right"])
            else:
                self.status.print("Warning: '{0}' is not a valid code symbol. Please check stratagem.yml.".format(key))

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
                    self.status.print("Found match: {0}".format(stratagem["name"]))
                    self.execute_stratagem(stratagem)
                    return True
        return False

    def listen(self):
        recog = self.recog
        status = self.status

        status.setStatus(Status.LISTENING)
        self.status.print("Listening...")
        with self.mic as source:
            audio = recog.listen(source)

        self.status.print("Input recieved, converting... ")
        status.setStatus(Status.PROCESSING)
        try:
            command = recog.recognize_sphinx(audio, keyword_entries=self.stratagemopts.keyWords)
            self.status.print("Heard: {0}".format(command))

            if self.interpret_stratagem(command):
                while kb.is_pressed(self.strat_key):
                    time.sleep(1)           # I don't like this but keyboard module cannot properly detect KEY_UP events & without this hotkey will keep triggering
                
            else:
                self.status.print("{0} was not a valid stratagem.".format(command))

        except sr.UnknownValueError:
            self.status.print("Could not understand audio.")
        except sr.RequestError as e:
            self.status.print("Could not decode, recieved error: {0}".format(e))

        status.setStatus(Status.IDLE)