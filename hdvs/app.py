from PyQt5.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import pyqtSignal

import speech_recognition as sr
import keyboard as kb

import time

from core import *

class hdvs(QMainWindow):
    statusPrint = pyqtSignal(str)
    changeStatus = pyqtSignal(Status)

    def __init__(self, stratagems, config, parent=None):
        super(QMainWindow, self).__init__(parent)

        self.setWindowTitle("Helldivers Voice Stratagem")
        self.setWindowIcon(QIcon("icons/Icon.webp"))

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

        print("Ready")

        self.status = StatusWidg(self)
        self.statusPrint.connect(self.status.print)
        self.changeStatus.connect(self.status.setStatus)

        self.stratagemopts = StratagemGroup(self.stratagems)
        self.options = Options(self.config, self.listen)

        rhsLayout = QVBoxLayout()
        rhsLayout.addWidget(self.status)
        rhsLayout.addWidget(self.options)

        rhs = QWidget()
        rhs.setLayout(rhsLayout)

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.stratagemopts)
        mainLayout.addWidget(rhs)

        main = QWidget()
        main.setLayout(mainLayout)
        self.setCentralWidget(main)
        self.changeStatus.emit(Status.IDLE)

        self.active = False

    def execute_stratagem(self, stratagem):
        self.changeStatus.emit(Status.EXECUTING)
        config = self.config
        self.sPrint("Executing {0}.".format(stratagem["name"]))
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
                self.sPrint("Warning: '{0}' is not a valid code symbol. Please check stratagem.yml.".format(key))

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
                    self.sPrint("Found match: {0}".format(stratagem["name"]))
                    self.execute_stratagem(stratagem)
                    return True
        return False

    def listen(self):
        recog = self.recog

        self.changeStatus.emit(Status.LISTENING)
        self.sPrint("Listening...")
        with self.mic as source:
            audio = recog.listen(source)

        self.sPrint("Input recieved, converting... ")
        self.changeStatus.emit(Status.PROCESSING)
        try:
            command = recog.recognize_sphinx(audio, keyword_entries=self.stratagemopts.keyWords)
            self.sPrint("Heard: {0}".format(command))

            if self.interpret_stratagem(command):
                while kb.is_pressed(self.strat_key):
                    time.sleep(1)           # I don't like this but keyboard module cannot properly detect KEY_UP events & without this hotkey will keep triggering
                
            else:
                self.sPrint("{0} was not a valid stratagem.".format(command))

        except sr.UnknownValueError:
            self.sPrint("Could not understand audio.")
        except sr.RequestError as e:
            self.sPrint("Could not decode, recieved error: {0}".format(e))

        self.changeStatus.emit(Status.IDLE)

    def sPrint(self, msg : str):
        self.statusPrint.emit(msg)