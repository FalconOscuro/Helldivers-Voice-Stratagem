from PyQt5.QtWidgets import QApplication
import sys
import app
import yaml

print("Loading config...")
with open("stratagem.yml", 'r') as file:
    stratagems = yaml.safe_load(file)

with open("config.yml", 'r') as file:
    config = yaml.safe_load(file)

qapp = QApplication(sys.argv)
hdvs = app.hdvs(stratagems, config)
hdvs.show()

qapp.exec_()