from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QGroupBox, QLineEdit, QPlainTextEdit, QVBoxLayout
from PyQt5.QtGui import QPalette

import enum

class Status(enum.Enum):
    IDLE = { "colour" : Qt.black, "text" : "Idle" }
    LISTENING = { "colour" : Qt.blue, "text" : "Listening..." }
    PROCESSING = { "colour" : Qt.red, "text" : "Processing..." }
    EXECUTING = { "colour" : Qt.green, "text" : "Executing!" }

class StatusWidg(QGroupBox):
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