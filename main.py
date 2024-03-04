import sys
from PyQt5.QtWidgets import QWidget

import speech_recognition as sr

import keyboard as kb
import yaml
import time
import enum
import re
from num2words import num2words
import syllables

from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPalette, QIcon

icon_path = "Icon.webp"

# status enum
class Status(enum.Enum):
    IDLE = { "colour" : Qt.black, "text" : "Idle" }
    LISTENING = { "colour" : Qt.blue, "text" : "Listening..." }
    PROCESSING = { "colour" : Qt.red, "text" : "Processing..." }
    EXECUTING = { "colour" : Qt.green, "text" : "Executing!" }

def format_command(command):
    res = command.replace("-", " ").lower()
    res = re.sub(r"(\d+)", lambda x: num2words(int(x.group(0))), res)

    return re.sub(r'[^\w\s]', '', res).strip()

class StatusGroup(QGroupBox):
    def __init__(self, parent=None):
        super(QGroupBox, self).__init__(parent)
        self.setTitle("Status")

        self.status = QLineEdit()
        self.status.setReadOnly(True)

        self.outbox = QPlainTextEdit()
        self.outbox.setReadOnly(True)

        layout = QVBoxLayout()
        layout.addWidget(self.status)
        layout.addWidget(self.outbox)
        self.setLayout(layout)

    def setStatus(self, status: Status):
        palette = QPalette()
        palette.setColor(QPalette.Text, status.value["colour"])
        self.status.setPalette(palette)
        self.status.setText(status.value["text"])
    
    def print(self, text: str):
        self.outbox.insertPlainText(text + '\n')

class StratagemOpt(QWidget):
    def __init__(self, stratagem, parent=None):
        super(QWidget, self).__init__(parent)

        self.stratagem = stratagem
        name = QLabel(stratagem["name"])

        toggle = QCheckBox("Enabled")
        toggle.setChecked(stratagem["enabled"])
        toggle.toggled.connect(self.changeEnabled)

        self.triggers = QLabel()
        self.triggers.setWordWrap(True)
        self.updateTriggers()

        add = QPushButton("Add")
        add.pressed.connect(self.addTrigger)

        remove = QPushButton("Remove")
        remove.pressed.connect(self.removeTrigger)

        buttonLayout = QHBoxLayout()
        buttonLayout.addWidget(add)
        buttonLayout.addWidget(remove)
        buttons = QWidget()
        buttons.setLayout(buttonLayout)

        layout = QVBoxLayout()
        layout.addWidget(name)
        layout.addWidget(self.triggers)
        layout.addWidget(buttons)
        layout.addWidget(toggle)
        self.setLayout(layout)
    
    def changeEnabled(self, enabled: bool):
        self.stratagem["enabled"] = enabled
    
    def updateTriggers(self):
        self.triggers.setText("triggers: " + ", ".join(self.stratagem["trigger"]))
    
    def addTrigger(self):
        text, ok = QInputDialog.getText(self, "Add {0}".format(self.stratagem["name"]), "Add trigger:")

        if ok and not text in self.stratagem["trigger"] and text != "":
            self.stratagem["trigger"].append(text)
            self.updateTriggers()


    def removeTrigger(self):
        text, ok = QInputDialog.getText(self, "Remove {0}".format(self.stratagem["name"]), "Remove trigger:")

        if ok and text in self.stratagem["trigger"]:
            self.stratagem["trigger"].remove(text)
            self.updateTriggers()
        



class StratagemGroup(QGroupBox):
    def __init__(self, stratagems, parent=None):
        super(QGroupBox, self).__init__(parent)
        self.setTitle("Stratagems")
        self.stratagems = stratagems
        self.updateKeywords()

        scroll_layout = QVBoxLayout()
        for stratagem in stratagems:
            scroll_layout.addWidget(StratagemOpt(stratagem))
        
        sw = QWidget()
        sw.setLayout(scroll_layout)

        scrollArea = QScrollArea()
        scrollArea.setWidget(sw)

        apply = QPushButton("Apply")
        apply.pressed.connect(self.apply)

        layout = QVBoxLayout()
        layout.addWidget(scrollArea)
        layout.addWidget(apply)
        self.setLayout(layout)
    
    def apply(self):
        with open("stratagem.yml", 'w') as file:
            yaml.dump(self.stratagems, file)
    
    def updateKeywords(self):

        self.keyWords = []

        for stratagem in self.stratagems:
            triggers = stratagem["trigger"]
            for i in range(len(triggers)):
                triggers[i] = format_command(triggers[i])
                sylnum = syllables.estimate(triggers[i])

                self.keyWords.append((triggers[i], 1 - (pow(sylnum + 1, -1) / 16)))


class hdvs(QMainWindow):
    def __init__(self, parent=None):
        super(QMainWindow, self).__init__(parent)

        self.setWindowTitle("Helldivers Voice Stratagem")
        self.setWindowIcon(QIcon(icon_path))

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

        self.status = StatusGroup()
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
    
    def exec(self, stratagem_file, config_file):
        self.app.exec_()




app = QApplication(sys.argv)
window = hdvs()
window.show()

app.exec_()