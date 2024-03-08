from PyQt5.QtWidgets import QGroupBox, QWidget, QLabel, QHBoxLayout, QVBoxLayout, QPushButton
import keyboard as kb
from yaml import dump

class KeyOpt(QWidget):
    def __init__(self, name, keys, parent=None):
        super(QWidget, self).__init__(parent)

        self.name = name
        self.keys = keys
        self.onChange = []

        self.info = QLabel()
        self.btn = QPushButton("Remap")
        self.btn.pressed.connect(self.Remap)

        layout = QHBoxLayout()
        layout.addWidget(self.info)
        layout.addWidget(self.btn)

        self.setLayout(layout)
        self.UpdateInfo()

    def UpdateInfo(self):
        self.info.setText("{0}: {1}".format(self.name, self.keys[self.name]))
    
    def Remap(self):
        self.keys[self.name] = kb.read_key()
        self.UpdateInfo()

        for listener in self.onChange:
            listener()


class Options(QGroupBox):
    def __init__(self, config, stratagemCallback, parent=None):
        super(QGroupBox, self).__init__(parent)
        self.setTitle("Options")
        self.config = config

        self.stratagemCallback = stratagemCallback
        self.SetHotkey()

        layout = QVBoxLayout()

        for name in self.config["keys"].keys():
            keyOpt = KeyOpt(name, self.config["keys"])

            if (name == "stratagem"):
                keyOpt.onChange.append(self.SetHotkey)

            layout.addWidget(keyOpt)
        
        self.btn = QPushButton("Apply")
        self.btn.pressed.connect(self.Apply)
        layout.addWidget(self.btn)

        layout.setSpacing(0)
        self.setLayout(layout)
    
    def SetHotkey(self):
        if hasattr(self, "hk"):
            kb.remove_hotkey(self.hk)
        
        self.hk = kb.add_hotkey(self.config["keys"]["stratagem"], self.stratagemCallback)

    def Apply(self):
        with open("config.yml", 'w') as file:
            dump(self.config, file)