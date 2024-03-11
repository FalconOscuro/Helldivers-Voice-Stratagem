from PyQt5.QtWidgets import QWidget, QLabel, QCheckBox, QPushButton, QHBoxLayout, QVBoxLayout, QInputDialog, QGroupBox, QScrollArea, QSizePolicy
from PyQt5.QtGui import QPixmap

from yaml import dump
from num2words import num2words
import syllables
import re
from hdvs.util import CollapsibleBox

def format_command(command):
    res = command.replace("-", " ").lower()
    res = re.sub(r"(\d+)", lambda x: num2words(int(x.group(0))), res)

    return re.sub(r'[^\w\s]', '', res).strip()

class StratagemOpt(CollapsibleBox):
    def __init__(self, stratagem, parent=None):
        super(StratagemOpt, self).__init__(stratagem["name"], parent)

        self.stratagem = stratagem
        
        iconSrc = ""

        if "icon" in self.stratagem.keys():
            iconSrc = self.stratagem["icon"]
        
        else:
            iconSrc = self.stratagem["name"].split()[0].replace('/', '')
        
        pixmap = QPixmap("data/icons/{0}.webp".format(iconSrc))

        if pixmap.isNull():
            print("Error, could not load 'icons/{0}.webp'".format(iconSrc))
            pixmap = QPixmap("data/icons/Icon.webp")
        
        self.setIcon(pixmap)

        toggle = QCheckBox("Enabled")
        toggle.setChecked(stratagem["enabled"])
        toggle.toggled.connect(self.changeEnabled)

        self.triggers = QLabel(self)
        self.triggers.setWordWrap(True)
        self.updateTriggers()

        add = QPushButton("Add")
        add.pressed.connect(self.addTrigger)

        remove = QPushButton("Remove")
        remove.pressed.connect(self.removeTrigger)

        buttonLayout = QHBoxLayout()
        buttonLayout.addWidget(add)
        buttonLayout.addWidget(remove)
        buttons = QWidget(self)
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

        self.stratagemopts = []

        scroll_layout = QVBoxLayout(self)
        for stratagem in stratagems:
            stratagemopt = StratagemOpt(stratagem, self)
            self.stratagemopts.append(stratagemopt)
            scroll_layout.addWidget(stratagemopt)
        
        sw = QWidget(self)
        sw.setLayout(scroll_layout)

        scrollArea = QScrollArea(self)
        scrollArea.setWidgetResizable(True)
        scrollArea.setWidget(sw)

        apply = QPushButton("Apply")
        apply.pressed.connect(self.apply)

        layout = QVBoxLayout(self)
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