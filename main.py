from PyQt5.QtWidgets import QApplication
import hdvs
import sys
import yaml

print("Loading config...")
with open("data/stratagem.yml", 'r') as file:
    stratagems = yaml.safe_load(file)

with open("data/config.yml", 'r') as file:
    config = yaml.safe_load(file)

qapp = QApplication(sys.argv)
wnd = hdvs.app.hdvs(stratagems, config)
wnd.show()

qapp.exec_()