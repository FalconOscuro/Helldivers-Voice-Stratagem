from PyQt5.QtWidgets import QWidget, QLabel, QCheckBox, QPushButton, QHBoxLayout, QVBoxLayout, QInputDialog, QGroupBox, QScrollArea

from yaml import dump
from num2words import num2words
import syllables
import re
from util import CollapsibleBox

def format_command(command):
    res = command.replace("-", " ").lower()
    res = re.sub(r"(\d+)", lambda x: num2words(int(x.group(0))), res)

    return re.sub(r'[^\w\s]', '', res).strip()

class StratagemOpt(CollapsibleBox):
    def __init__(self, stratagem, parent=None):
        super(StratagemOpt, self).__init__(stratagem["name"], parent)

        self.stratagem = stratagem

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
        layout.addWidget(self.triggers)
        layout.addWidget(buttons)
        layout.addWidget(toggle)
        self.setContentLayout(layout)
    
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
        scrollArea.setWidgetResizable(True)
        scrollArea.setWidget(sw)

        apply = QPushButton("Apply")
        apply.pressed.connect(self.apply)

        layout = QVBoxLayout()
        layout.addWidget(scrollArea)
        layout.addWidget(apply)
        self.setLayout(layout)
    
    def apply(self):
        with open("stratagem.yml", 'w') as file:
            dump(self.stratagems, file)
    
    def updateKeywords(self):

        self.keyWords = []

        for stratagem in self.stratagems:
            triggers = stratagem["trigger"]
            for i in range(len(triggers)):
                triggers[i] = format_command(triggers[i])
                sylnum = syllables.estimate(triggers[i])

                self.keyWords.append((triggers[i], 1 - (pow(sylnum + 1, -1) / 16)))