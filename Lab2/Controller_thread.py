from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import cv2
from PIL import Image
import numpy as np
import time

from Line import Point2D
from LinePair import LinePair


class DrawLabel(QLabel):

    def __init__(self):
        super(DrawLabel, self).__init__()
        self.painter = QPainter(self)
        self.List = {}
        self.x0 = 0
        self.y0 = 0
        self.x1 = 0
        self.y1 = 0
        self.flag = False
        self.switch = True
        self.i = 0

    # click event
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.flag = True
            self.x0 = event.x()
            self.y0 = event.y()
            self.i += 1

        if event.button() == Qt.RightButton:
            self.List.clear()
            self.x0 = 0
            self.y0 = 0
            self.x1 = 0
            self.y1 = 0
            self.i = 0
            self.update()

    # mouse release event
    def mouseReleaseEvent(self, event):
        self.flag = False
        self.switch = False
        # print(self.x0, self.y0, self.x1, self.y1)

    # movement event
    def mouseMoveEvent(self, event):
        if self.flag:
            self.x1 = event.x()
            self.y1 = event.y()
            self.update()

    # Control event
    def paintEvent(self, event):
        super(DrawLabel, self).paintEvent(event)
        self.painter.begin(self)
        self.painter.setPen(QPen(Qt.red, 1, Qt.SolidLine))

        if self.switch:
            # Set parameter
            self.List[self.i] = [self.x0, self.y0, self.x1, self.y1]

        # Draw Lines
        for data in self.List.values():
            if data != [0, 0, 0, 0]:
                self.painter.drawRect(QRect(QPoint(data[0] - 3, data[1] - 3), QSize(6, 6)))
                self.painter.drawLine(data[0], data[1], data[2], data[3])
                self.painter.drawRect(QRect(QPoint(data[2] - 3, data[3] - 3), QSize(6, 6)))
        self.painter.end()


class imageInfo:
    def __init__(self):
        self.imagePath = ''
        self.imageLine = []
        self.width = 0
        self.height = 0


class Warp(QThread):
    sinOut = pyqtSignal(str)    # 訊號

    def __init__(self, image1Info, image2Info, linesPair, i):
        super(Warp, self).__init__()

        self.i = i

        self.image1Info = image1Info
        self.image2Info = image2Info

        self.image1 = cv2.cvtColor(cv2.imread(self.image1Info.imagePath), cv2.COLOR_BGR2RGB)
        self.image2 = cv2.cvtColor(cv2.imread(self.image2Info.imagePath), cv2.COLOR_BGR2RGB)

        self.image1Info.height = self.image1.shape[0]
        self.image1Info.width = self.image1.shape[1]
        self.image2Info.height = self.image2.shape[0]
        self.image2Info.width = self.image2.shape[1]

        self.width = self.image1Info.width
        self.height = self.image1Info.height

        self.resultImage = np.zeros((self.height, self.width, 3), dtype=np.uint8)  # result image
        self.linesPair = linesPair

        self.frame_count = 10

    def Bilinear(self, imArr, posX, posY):
        out = []

        # Get integer and fractional parts of numbers
        modXi = int(posX)  # FLOOR
        modYi = int(posY)
        modXf = posX - modXi  # A
        modYf = posY - modYi  # B
        modXiPlusOneLim = min(modXi + 1, imArr.shape[1] - 1)  # CEILING
        modYiPlusOneLim = min(modYi + 1, imArr.shape[0] - 1)

        # Get pixels in four corners
        for chan in range(imArr.shape[2]):
            bl = imArr[modYi, modXi, chan]
            br = imArr[modYi, modXiPlusOneLim, chan]
            tl = imArr[modYiPlusOneLim, modXi, chan]
            tr = imArr[modYiPlusOneLim, modXiPlusOneLim, chan]

            # Calculate interpolation
            b = modXf * br + (1. - modXf) * bl
            t = modXf * tr + (1. - modXf) * tl

            pxf = modYf * t + (1. - modYf) * b

            out.append(int(pxf + 0.5))

        return out

    def runWarp(self, frame_index):

        ratio = (frame_index + 1) / (self.frame_count + 1)

        for y in range(self.height):
            for x in range(self.width):

                dst_point = Point2D()
                dst_point.x = x
                dst_point.y = y
                leftXSum_x = 0.0
                leftXSum_y = 0.0
                leftWeightSum = 0.0
                rightXSum_x = 0.0
                rightXSum_y = 0.0
                rightWeightSum = 0.0

                size = len(self.linesPair)

                for i in range(size):
                    # 左圖為來源
                    src_line = self.linesPair[i].leftLine
                    dst_line = self.linesPair[i].warpLine[frame_index]

                    new_u = dst_line.Getu(dst_point)
                    new_v = dst_line.Getv(dst_point)

                    src_point = src_line.Get_Point(new_u, new_v)
                    src_weight = dst_line.Get_Weight(dst_point)

                    leftXSum_x = leftXSum_x + src_point.x * src_weight
                    leftXSum_y = leftXSum_y + src_point.y * src_weight
                    leftWeightSum = leftWeightSum + src_weight

                    # 右圖為來源
                    src_line = self.linesPair[i].rightLine

                    new_u = dst_line.Getu(dst_point)
                    new_v = dst_line.Getv(dst_point)

                    src_point = src_line.Get_Point(new_u, new_v)
                    src_weight = dst_line.Get_Weight(dst_point)

                    rightXSum_x = rightXSum_x + src_point.x * src_weight
                    rightXSum_y = rightXSum_y + src_point.y * src_weight
                    rightWeightSum = rightWeightSum + src_weight

                left_src_x = leftXSum_x / leftWeightSum
                left_src_y = leftXSum_y / leftWeightSum
                right_src_x = rightXSum_x / rightWeightSum
                right_src_y = rightXSum_y / rightWeightSum

                # 邊界測試
                if left_src_x < 0:
                    left_src_x = 0
                if left_src_y < 0:
                    left_src_y = 0
                if left_src_x >= self.width:
                    left_src_x = self.width - 1
                if left_src_y >= self.height:
                    left_src_y = self.height - 1
                if right_src_x < 0:
                    right_src_x = 0
                if right_src_y < 0:
                    right_src_y = 0
                if right_src_x >= self.width:
                    right_src_x = self.width - 1
                if right_src_y >= self.height:
                    right_src_y = self.height - 1

                left_scalar = self.Bilinear(self.image1, left_src_x, left_src_y)
                right_scalar = self.Bilinear(self.image2, right_src_x, right_src_y)

                for k in range(3):
                    self.resultImage[y][x][k] = (1 - ratio) * left_scalar[k] + ratio * right_scalar[k]

        new_image = Image.fromarray(self.resultImage, "RGB")
        new_image.save('./result/%d.png' % frame_index)

    def run(self):

        print("Start %d warp ~" % (self.i + 1))
        self.runWarp(self.i)
        self.sinOut.emit("")


class present_Window(QWidget):

    def __init__(self, widget, image1Info, image2Info, linesPair):
        super(present_Window, self).__init__()

        self.widget = widget
        self.cnt = 0

        self.setStyleSheet("font: 12pt Arial")
        self.image1Info = image1Info
        self.image2Info = image2Info
        self.linesPair = linesPair

        # multithreading
        self.threads = []
        for i in range(10):
            self.threads.append(Warp(image1Info, image2Info, linesPair, i))
            self.threads[i].sinOut.connect(self.doneWarp)
            self.threads[i].start()

        self.imageLabel = QLabel(self)
        self.imageLabel.setAlignment(Qt.AlignCenter)
        self.count = QLabel(self)
        self.count.setAlignment(Qt.AlignCenter)
        self.count.setText('0/10 done')

        self.movie = QMovie("just-a-second-please-eric-cartman.gif")
        self.imageLabel.setMovie(self.movie)
        self.movie.start()

        vbox = QVBoxLayout()
        vbox.addWidget(self.imageLabel)
        vbox.addWidget(self.count)

        self.setLayout(vbox)

    def doneWarp(self, file_inf):
        self.cnt = self.cnt + 1
        self.count.setText('%d/10 done' % self.cnt)

        if self.cnt == 10:
            self.thread_finished()

    def thread_finished(self):

        self.count.hide()

        self.widget.setMinimumSize(self.image1Info.width, self.image1Info.height)
        self.widget.setGeometry((1920 - self.image1Info.width) / 2, (1080 - self.image1Info.height) / 2,
                                self.image1Info.width, self.image1Info.height)

        while True:
            for i in range(10):
                self.imageLabel.setPixmap(QPixmap('result/%d.png' % i))
                QApplication.processEvents()
                time.sleep(0.05)


class Draw_Line_Window(QWidget):

    def __init__(self, widget, image1Info, image2Info):
        super(Draw_Line_Window, self).__init__()

        self.setStyleSheet("font: 12pt Arial")

        self.widget = widget

        self.image1Info = image1Info
        self.image2Info = image2Info

        self.lastPoint = QPoint()

        self.title = QLabel(self)
        self.title.setText('Draw the pairs of lines for two images')
        self.title.setAlignment(Qt.AlignCenter)
        self.title.setStyleSheet("font: 20pt Comic Sans MS")

        self.imageLabel1 = QLabel(self)
        self.imageLabel2 = QLabel(self)
        self.imageLabel1.setText("Image1")
        self.imageLabel2.setText("Image2")

        self.imageLabel1.setAlignment(Qt.AlignCenter)
        self.imageLabel2.setAlignment(Qt.AlignCenter)

        self.image1 = DrawLabel()
        self.image2 = DrawLabel()

        self.image1.setAlignment(Qt.AlignCenter)
        self.image2.setAlignment(Qt.AlignCenter)

        self.KeepDrawBtn = QPushButton()
        self.WarpBtn = QPushButton()

        self.KeepDrawBtn.setText("KeepDraw")
        self.WarpBtn.setText("NextStep")

        vbox = QVBoxLayout()
        vbox.addWidget(self.title)
        vbox.addStretch()

        vbox2 = QHBoxLayout()
        vbox.addLayout(vbox2)
        vbox3 = QVBoxLayout()
        vbox4 = QVBoxLayout()
        vbox2.addLayout(vbox3)
        vbox2.addLayout(vbox4)

        vbox3.addWidget(self.imageLabel1)
        vbox3.addStretch()
        vbox3.addWidget(self.image1)

        vbox4.addWidget(self.imageLabel2)
        vbox4.addStretch()
        vbox4.addWidget(self.image2)

        vbox.addStretch()
        vbox.addWidget(self.KeepDrawBtn)
        vbox.addStretch()
        vbox.addWidget(self.WarpBtn)

        self.setLayout(vbox)
        self.setWindowTitle('DrawLines')

        self.linesPair = []

        self.setup_control()

    def setup_control(self):

        self.KeepDrawBtn.clicked.connect(self.keepDraw)
        self.WarpBtn.clicked.connect(self.nextStep)

    def Preprocess(self):

        print("Start preprocess\n")

        length = len(self.image1Info.imageLine)

        print("left lines : ")
        print(self.image1Info.imageLine)
        print("right lines : ")
        print(self.image2Info.imageLine)

        for i in range(1, length):
            curLinePair = LinePair()

            curLinePair.leftLine.P.x = self.image1Info.imageLine[i][0]
            curLinePair.leftLine.P.y = self.image1Info.imageLine[i][1]
            curLinePair.leftLine.Q.x = self.image1Info.imageLine[i][2]
            curLinePair.leftLine.Q.y = self.image1Info.imageLine[i][3]

            curLinePair.leftLine.PQtoMLD()

            curLinePair.rightLine.P.x = self.image2Info.imageLine[i][0]
            curLinePair.rightLine.P.y = self.image2Info.imageLine[i][1]
            curLinePair.rightLine.Q.x = self.image2Info.imageLine[i][2]
            curLinePair.rightLine.Q.y = self.image2Info.imageLine[i][3]

            curLinePair.rightLine.PQtoMLD()

            curLinePair.genWarpLine()

            self.linesPair.append(curLinePair)

        print("\nPreprocess over\n")

    def keepDraw(self):
        self.image1.switch = True
        self.image2.switch = True

    def nextStep(self):
        self.image1Info.imageLine = self.image1.List
        self.image2Info.imageLine = self.image2.List

        self.Preprocess()

        window3 = present_Window(self.widget, self.image1Info, self.image2Info, self.linesPair)
        self.widget.addWidget(window3)
        self.widget.setCurrentIndex(self.widget.currentIndex() + 1)
        self.widget.setMinimumSize(500, 350)
        self.widget.setGeometry(710, 365, 480, 300)


class MainWindow(QWidget):

    def __init__(self, widget):
        super(MainWindow, self).__init__()

        self.setStyleSheet("font: 12pt Arial")

        self.image1Info = imageInfo()
        self.image2Info = imageInfo()

        self.widget = widget

        self.title = QLabel(self)
        self.title.setText('Image morphing')
        self.title.setAlignment(Qt.AlignCenter)
        self.title.setStyleSheet("font: 24pt Comic Sans MS")

        self.imageLabel1 = QLabel(self)
        self.LineEdit1 = QLineEdit(self)
        self.BrowseBtn1 = QPushButton(self)

        self.imageLabel2 = QLabel(self)
        self.LineEdit2 = QLineEdit(self)
        self.BrowseBtn2 = QPushButton(self)

        self.submitBtn = QPushButton(self)

        self.imageLabel1.setText("Image1")
        self.imageLabel2.setText("Image2")

        self.BrowseBtn1.setText("Browse")
        self.BrowseBtn2.setText("Browse")

        self.submitBtn.setText("submit")

        vbox = QVBoxLayout()
        vbox.addWidget(self.title)
        vbox.addStretch()

        vbox2 = QHBoxLayout()
        vbox.addLayout(vbox2)
        vbox2.addWidget(self.imageLabel1)
        vbox2.addWidget(self.LineEdit1)
        vbox2.addWidget(self.BrowseBtn1)
        vbox.addStretch()

        vbox3 = QHBoxLayout()
        vbox.addLayout(vbox3)
        vbox3.addWidget(self.imageLabel2)
        vbox3.addWidget(self.LineEdit2)
        vbox3.addWidget(self.BrowseBtn2)
        vbox.addStretch()
        vbox.addWidget(self.submitBtn)

        self.setLayout(vbox)
        self.setWindowTitle('Main Window')

        self.setup_control()

    def setup_control(self):

        self.BrowseBtn1.clicked.connect(self.open_file)
        self.BrowseBtn2.clicked.connect(self.open_file_)
        self.submitBtn.clicked.connect(self.submit)

    def open_file(self):
        self.image1Info.imagePath, filetype = QFileDialog.getOpenFileName(self, "Open file", ".")  # start path
        self.LineEdit1.setText(self.image1Info.imagePath)

    def open_file_(self):
        self.image2Info.imagePath, filetype = QFileDialog.getOpenFileName(self, "Open file", ".")  # start path
        self.LineEdit2.setText(self.image2Info.imagePath)

    def submit(self):
        window2 = Draw_Line_Window(self.widget, self.image1Info, self.image2Info)

        window2.image1.setPixmap(QPixmap(window2.image1Info.imagePath))
        window2.image2.setPixmap(QPixmap(window2.image2Info.imagePath))

        self.widget.addWidget(window2)
        self.widget.setCurrentIndex(self.widget.currentIndex() + 1)
        self.widget.setGeometry((1920-self.widget.width())/2, (1080-self.widget.height())/2, self.widget.width(), self.widget.height())
