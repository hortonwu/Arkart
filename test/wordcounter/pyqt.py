#!/user/bin/python
#-*- coding: utf-8 -*-

"""
Description: this program is to create a generic class with main window, status bar, menu bar, button,

Designed by Haotian WU

Date: Oct 16th 2016


"""

import sys
from PyQt4 import QtGui, QtCore


class Window(QtGui.QMainWindow):
    def __init__(self):
        super(Window, self).__init__()
        self.initUI()

    def initUI(self):
        #initialize text editor
        self.initText()

        #define Actions: exit, new...
        exAction = QtGui.QAction(QtGui.QIcon('/home/hwu/PycharmProjects/Arkart/data/logo.png'), '&Exit', self)
        exAction.setShortcut('Ctrl+Q')
        exAction.setStatusTip('Exit Application!')
        #exAction.triggered.connect(QtGui.qApp.quit)
        exAction.triggered.connect(self.closeApp)

        #init menubar
        menubar = self.menuBar()
        filemenu = menubar.addMenu('&File')
        filemenu.addAction(exAction)

        #init toolbar
        toolbar = self.addToolBar('Exit')
        toolbar.addAction(exAction)


        #init main window
        self.initWindow()

        #display the window
        self.show()

    def initText(self):
        textEd= QtGui.QTextEdit()
        self.setCentralWidget(textEd)


    def initWindow(self):
        self.setGeometry(50, 50, 500, 300)
        self.setWindowTitle("Arkart Console")
        self.setWindowIcon(QtGui.QIcon('/home/hwu/PycharmProjects/Arkart/data/logo.png'))
        self.statusBar().showMessage('Ready')


    # def home method
    def home(self):
        #### create button
        btn = QtGui.QPushButton("Quit", self)
        #set button events
        # quite the instance
        #btn.clicked.connect(QtCore.QCoreApplication.instance().quit)

        # customize the callback function
        btn.clicked.connect(closeApp)
        #set button attribute
        #btn.resize(100,50)
        btn.resize(btn.sizeHint())
        btn.resize(btn.minimumSizeHint())
        btn.move(100,100)

        exAction = QtGui.QAction(QtGui.QIcon('/home/hwu/PycharmProjects/Arkart/data/logo.png'), '&Exit', self)
        exAction.setShortcut('Ctrl+Q')
        exAction.setStatusTip('Exit Application!')
        exAction.triggered.connect(QtGui.qApp.quit)
        self.statusBar()

        self.toolBar = self.addToolBar("Toolbar")
        self.toolBar.addAction(self.closeApp)


        self.show()

    # def the customize call back method
    def closeApp(self):
        # print("Save the file before close!")
        option = QtGui.QMessageBox.question(self, 'Message', "Do you want to close app without save?",
                                            QtGui.QMessageBox.Yes | QtGui.QMessageBox.No, QtGui.QMessageBox.No)
        if option == QtGui.QMessageBox.Yes:
            print ("Exit the app without save!!!")
            sys.exit()
        else:
            pass




def main():
    app = QtGui.QApplication(sys.argv)
    ArkGui= Window()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

