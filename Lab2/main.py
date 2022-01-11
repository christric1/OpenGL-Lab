from PyQt5 import QtWidgets
import sys
import Controller
import Controller_thread

if __name__ == '__main__':

    app = QtWidgets.QApplication(sys.argv)
    widget = QtWidgets.QStackedWidget()
    widget.setGeometry(720, 390, 480, 300)

    # window = Controller.MainWindow(widget)
    window = Controller_thread.MainWindow(widget)   # thread 版

    widget.addWidget(window)
    widget.show()

    sys.exit(app.exec_())

