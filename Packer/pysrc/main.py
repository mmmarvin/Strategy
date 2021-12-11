#######################
## MIT License
##
## Copyright 2019-2020 Marvin Manese
##
## Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
## associated documentation files (the "Software"), to deal in the Software without restriction, including 
## without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
## copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the 
## following conditions:
##
## The above copyright notice and this permission notice shall be included in all copies or substantial portions 
## of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
## TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
## THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
## CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
## IN THE SOFTWARE. 
#######################
import os
import sys
import subprocess
from sys import platform
from pathlib import Path
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import QT_VERSION_STR
from PyQt5 import QtCore

class MainWindow(QMainWindow):
	def __init__(self, dirPath, packerFilename):
		super().__init__()
		
		# Open the packer program and read the title from it
		proc = subprocess.Popen([ dirPath + "/" + packerFilename], stdout = subprocess.PIPE)    
		self.__mTitle = proc.stdout.readline().decode("utf-8")[:-1]
		self.setWindowTitle(self.__mTitle + " GUI")

		self.__mToolbar = QToolBar(self)
		self.addToolBar(self.__mToolbar)
		self.__mOpenArchiveAction = QAction(QIcon("res/open.png"), self.tr("Open Archive"), self)
		self.__mOpenArchiveAction.setEnabled(True)
		self.__mSaveArchiveAction = QAction(QIcon("res/save.png"), self.tr("Save Archive"), self)
		self.__mSaveArchiveAction.setEnabled(False)
		self.__mAddFileAction = QAction(QIcon("res/add_file.png"), self.tr("Add File to Archive..."), self)
		self.__mAddFileAction.setEnabled(True)
		self.__mExtractAllFileAction = QAction(QIcon("res/extract_all.png"), self.tr("Extract All..."), self)
		self.__mExtractAllFileAction.setEnabled(False)
		self.__mAboutAction = QAction(QIcon("res/about.png"), self.tr("About"), self)
		self.__mAboutAction.setEnabled(True)
		
		self.__mToolbar.addAction(self.__mOpenArchiveAction)
		self.__mToolbar.addAction(self.__mSaveArchiveAction)
		self.__mToolbar.addAction(self.__mAddFileAction)
		self.__mToolbar.addAction(self.__mExtractAllFileAction)
		self.__mToolbar.addAction(self.__mAboutAction)
		
		self.__mFileListWidget = QListWidget(self)
		self.__mFileList = []
		self.__mCurrentFilename = ""
		self.setCentralWidget(self.__mFileListWidget)
		self.__mFileListWidget.itemDoubleClicked.connect(self.__openFile)

		self.__mProcFilename = dirPath + "/" + packerFilename
		self.__mProc = None # This will hold the Packer process

		self.__mOpenArchiveAction.triggered.connect(self.__openArchive)
		self.__mSaveArchiveAction.triggered.connect(self.__saveArchive)
		self.__mAddFileAction.triggered.connect(self.__addToArchive)
		self.__mExtractAllFileAction.triggered.connect(self.__extractAllFiles)
		self.__mAboutAction.triggered.connect(self.__showAbout)
		
	def __tryOpenSubProcess(self, archiveFilename):
		if self.__mProc != None:
			self.__mProc.terminate()
		self.__mProc = subprocess.Popen([self.__mProcFilename, archiveFilename], stdout = subprocess.PIPE, stdin = subprocess.PIPE)
		self.__mSaveArchiveAction.setEnabled(True)
		self.__mExtractAllFileAction.setEnabled(True)

	def __tryNewSubProcess(self):
		if self.__mProc != None:
			print("Terminating gpack subprocess...")
			self.__mProc.terminate()
		self.__mProc = subprocess.Popen([self.__mProcFilename, "--new"], stdout = subprocess.PIPE, stdin = subprocess.PIPE)
		self.__mSaveArchiveAction.setEnabled(True)
		
	def __cleanupStdOutMsg(self, msg):
		while len(msg) > 0 and msg[0] == '>':
			msg = msg[1:]
		if msg[len(msg) - 1] == '\n':
			msg = msg[:-1]
		return msg

	def __openArchive(self):
		archiveFilename = QFileDialog.getOpenFileName(self, "Open...", "", "Pack Files(*.spak)")[0]
		if len(archiveFilename) > 0:
			self.__mCurrentFilename = archiveFilename
			self.__tryOpenSubProcess(archiveFilename)
			if self.__mProc.poll() is None:
				self.__listFiles()
				
	def __closeArchive(self):
		if self.__mProc != None:
			self.__mProc.terminate()
		
		self.__mFileListWidget.clear()
		self.__mFileList.clear()
		self.__mCurrentFilename = ""
		self.__mOpenArchiveAction.setEnabled(True)
		self.__mSaveArchiveAction.setEnabled(False)
		self.__mExtractAllFileAction.setEnabled(False)
		self.__mAboutAction.setEnabled(True)
		
	def __saveArchive(self):
		if len(self.__mCurrentFilename) > 0:
			self.__saveArchiveImpl(self.__mCurrentFilename)
		else:
			filename = QFileDialog.getSaveFileName(self, "Save As...", "", "Pack Files (*.spak)")[0]
			if len(filename) > 0:
				self.__saveArchiveImpl(filename)
				
	def __saveArchiveImpl(self, filename):
		self.__mProc.stdin.write(("save " + filename + "\n").encode("utf-8"))
		self.__mProc.stdin.flush()
		msg = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
		if msg == "":
			self.__mCurrentFilename = filename
			self.__mExtractAllFileAction.setEnabled(True)
		else:
			QMessageBox.critical(self, "Error", msg, QMessageBox.StandardButton.Ok)
					
	def __addToArchive(self):
		filename = QFileDialog.getOpenFileName(self, "Open...", "", "All Files(*.*)")[0]
		if len(filename) > 0:
			if self.__mProc == None:
				self.__tryNewSubProcess()
			if self.__mProc.poll() is None:
				self.__mProc.stdin.write(("add " + filename + "\n").encode("utf-8"))
				self.__mProc.stdin.flush()
				msg = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
				if msg == "":
					self.__listFiles()
				else:
					QMessageBox.critical(self, "Error", "Cannot add file \"" + filename + "\"!", QMessageBox.StandardButton.Ok)
					
	def __extractAllFiles(self):
		outputDirectory = QFileDialog.getExistingDirectory(self, "Extract to...", "")
		if len(outputDirectory) > 0:
			self.__mProc.stdin.write(("extractall " + outputDirectory + "\n").encode("utf-8"))
			self.__mProc.stdin.flush()
			msg = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
			if msg == "":
				QMessageBox.information(self, "Successfull", "Extracted files to \"" + outputDirectory + "\"", QMessageBox.StandardButton.Ok)
			else:
				QMessageBox.critical(self, "Error", "Cannot extract files to \"" + outputDirectory + "\"", QMessageBox.StandardButton.Ok)
				
	def __showAbout(self):
		aboutStr = "GUI Interface for " + self.__mTitle + "\n"
		aboutStr = aboutStr + "Copyright (c) 2019-2020\n"
		aboutStr = aboutStr + "Written by Marvin Manese\n\n"
		aboutStr = aboutStr + "Based on PyQt5 v." + QT_VERSION_STR
		QMessageBox.about(self, "About",  aboutStr)
				
	def __openFile(self, item):
		filename = item.text()
		fileType = item.data(QtCore.Qt.UserRole)
				
		if fileType == 1:
			self.__mProc.stdin.write(("cd " + filename + "\n").encode("utf-8"))
			self.__mProc.stdin.flush()
			msg = self.__mProc.stdout.readline()
			msg = self.__cleanupStdOutMsg(msg.decode("utf-8"))
			if msg == "":
				self.__listFiles()
			else:
				QMessageBox.critical(self, "Error", msg, QMessageBox.StandardButton.Ok)
		# TODO: Add open options for files
		
	def __listFiles(self):
		self.__mFileList.clear()
		self.__mFileListWidget.clear()
		
		self.__mProc.stdin.write(b"ls -dc\n")
		self.__mProc.stdin.flush()
		dcount = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
		dirCount = int(dcount)
				
		self.__mProc.stdin.write(b"ls -dn\n")
		self.__mProc.stdin.flush()
		for i in range(dirCount):
			filename = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
			listItem = QListWidgetItem(QIcon("res/folder.png"), filename)
			listItem.setData(QtCore.Qt.UserRole, 1)
			self.__mFileList.append(listItem)
			self.__mFileListWidget.addItem(listItem)

		self.__mProc.stdin.write(b"ls -fc\n")
		self.__mProc.stdin.flush()
		count = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
		fileCount = int(count)
		
		self.__mProc.stdin.write(b"ls -f\n")
		self.__mProc.stdin.flush()
		for i in range(fileCount):
			filename = self.__cleanupStdOutMsg(self.__mProc.stdout.readline().decode("utf-8"))
			listItem = QListWidgetItem(QIcon("res/file.png"), filename)
			listItem.setData(QtCore.Qt.UserRole, 2)
			self.__mFileList.append(listItem)
			self.__mFileListWidget.addItem(listItem)

def main():
	app = QApplication([])

	packerFilename = "Packer" #TODO: Change this to gpack
	if platform == "win32":
		packerFilename = packerFilename + ".exe"

	dirPath = os.path.dirname(os.path.realpath(__file__))
	if not Path(dirPath + "/" + packerFilename).is_file():
		QMessageBox.critical(None, "Error", "Cannot find Packer \"" + packerFilename + "\"", QMessageBox.StandardButton.Ok)
		return
	  
	window = MainWindow(dirPath, packerFilename)
	window.show()
	app.exec()

if __name__ == "__main__":
	main()
