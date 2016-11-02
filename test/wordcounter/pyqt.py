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
        ##initialize text editor
        #self.initText()

        ##define Actions: exit, new...
        # exit action
        exAction = QtGui.QAction(QtGui.QIcon('/home/hwu/PycharmProjects/Arkart/data/logo.png'), '&Exit', self)
        exAction.setShortcut('Ctrl+Q')
        exAction.setStatusTip('Exit Application!')
        exAction.triggered.connect(self.closeApp)

        # open editor action
        openeditor = QtGui.QAction("&Eidtor", self)
        openeditor.setShortcut("Ctrl+E")
        openeditor.setStatusTip('Open Text Editor')
        openeditor.triggered.connect(self.editorOpen)

        # open file
        openfile = QtGui.QAction("&Open File", self)
        openfile.setShortcut("Ctrl+O")
        openfile.setStatusTip('Open File')
        openfile.triggered.connect(self.fileOpen)

        # open file
        savefile = QtGui.QAction("&Save File", self)
        savefile.setShortcut("Ctrl+S")
        savefile.setStatusTip('Save File')
        savefile.triggered.connect(self.fileSave)

        ##init menubar
        menubar = self.menuBar()
        filemenu = menubar.addMenu('&File')
        filemenu.addAction(exAction)
        filemenu.addAction(openfile)
        filemenu.addAction(savefile)

        #text editor menu
        editormenu = menubar.addMenu("&Editor")
        editormenu.addAction(openeditor)

        ##init toolbar
        toolbar = self.addToolBar('Exit')
        toolbar.addAction(exAction)

        ##init check box
        checkbox=QtGui.QCheckBox('Enlarge Window', self)
        checkbox.move(100,50)
        #checkbox.toggle()
        checkbox.stateChanged.connect(self.enlargeWindow)

        ##init progress bar
        self.progress = QtGui.QProgressBar(self)
        self.progress.setGeometry(200, 80, 250, 20)

        ## init button
        self.btn = QtGui.QPushButton("Download",self)
        self.btn.move(200,120)
        self.btn.clicked.connect(self.download)

        ## init window style
        print(self.style().objectName())
        self.styleoption = QtGui.QLabel("Plastique", self)

        ## init combox
        combox = QtGui.QComboBox(self)
        combox.addItem("motif")
        combox.addItem("Windows")
        combox.addItem("cde")
        combox.addItem("Plastique")
        combox.addItem("CleanLook")
        combox.addItem("windowsvista")

        combox.move(50,250)
        self.styleoption.move(50,150)
        combox.activated[str].connect(self.styleSelect)

        ## init font picker
        fontpick = QtGui.QAction('Font', self)
        fontpick.triggered.connect(self.fontPick)
        self.toolBar = self.addToolBar('Font')
        self.toolBar.addAction(fontpick)

        ## color picker
        color = QtGui.QColor(0, 0, 0)
        fontcolor = QtGui.QAction('Font Bg Color',self)
        fontcolor.triggered.connect(self.colorPick)
        self.toolBar.addAction(fontcolor)

        ## init calender
        cal = QtGui.QCalendarWidget(self)
        cal.move(500,200)
        cal.resize(500,200)




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
        btn.clicked.connect(self.closeApp)
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

    ### def the customize call back method
    ## close app
    def closeApp(self):
        # print("Save the file before close!")
        option = QtGui.QMessageBox.question(self, 'Message', "Do you want to close app without save?",
                                            QtGui.QMessageBox.Yes | QtGui.QMessageBox.No, QtGui.QMessageBox.No)
        if option == QtGui.QMessageBox.Yes:
            print ("Exit the app without save!!!")
            sys.exit()
        else:
            pass
    ## enlarge windows
    def enlargeWindow(self,state):
        if state == QtCore.Qt.Checked:
            self.setGeometry(50,50,1000,600)
        else:
            self.setGeometry(50,50,500,300)
    ## download progress bar
    def download(self):
        self.completed = 0
        while self.completed <100:
            self.completed += 0.0001
            self.progress.setValue(self.completed)

    ## style selection
    def styleSelect(self,text):
        self.styleoption.setText(text)
        QtGui.QApplication.setStyle(QtGui.QStyleFactory.create(text))

    ## font pick
    def fontPick(self):
        font, valid = QtGui.QFontDialog.getFont()
        if valid :
            self.styleoption.setFont(font)
        else:
            pass
    ## color pick
    def colorPick(self):
        color = QtGui.QColorDialog.getColor()
        self.styleoption.setStyleSheet("QWidget { background-color: %s}" % color.name())

    ## editor open
    def editorOpen(self):
        self.textedit = QtGui.QTextEdit()
        self.setCentralWidget(self.textedit)

    ## file open
    def fileOpen(self):
        name = QtGui.QFileDialog.getOpenFileName(self,'Open File')
        file = open(name, 'r')
        self.editorOpen()
        with file:
            text =file.read()
            self.textedit.setText(text)

    ## file save
    def fileSave(self):
        name = QtGui.QFileDialog.getSaveFileName(self,'Save File')
        file = open(name, 'w')
        text = self.textedit.toPlainText()
        file.write(text)
        file.close()



def main():
    app = QtGui.QApplication(sys.argv)
    ArkGui= Window()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

