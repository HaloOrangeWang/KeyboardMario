from settings import *
from functools import partial
from PyQt5.QtWidgets import QMainWindow, QMessageBox, QLabel, QPushButton, QRadioButton
from PyQt5.QtGui import QPainter, QPen, QColor, QFont
from PyQt5.QtCore import QTimer, Qt, QRect
import traceback
import win32api, win32con


def run_with_exc(f):
    """游戏运行出现错误时，用messagebox把错误信息显示出来"""

    def call(window, *args, **kwargs):
        try:
            return f(window, *args, **kwargs)
        except Exception:
            exc_info = traceback.format_exc()
            QMessageBox.about(window, '错误信息', exc_info)
    return call


class Window(QMainWindow):

    def __init__(self, audio_parser):
        super().__init__()
        self.last_scene = 0  # 上一次游戏场景
        self.cur_note_pressed = set()  # 当前按键情况
        self.audio_parser = audio_parser

    def init_window(self):
        # 设置窗口大小，文字等
        self.setFixedSize(650, 500)
        self.setObjectName('MainWindow')
        self.setWindowTitle('琴控手柄')
        # 设置文字
        self.l_up = QLabel(self)
        self.l_up.move(200, 125)
        self.l_up.setFixedSize(50, 50)
        self.l_up.setAlignment(Qt.AlignCenter)
        self.l_up.setFont(QFont('songti', pointSize=16))
        self.l_up.setText(LevelNote[1])
        self.l_left = QLabel(self)
        self.l_left.move(150, 175)
        self.l_left.setFixedSize(50, 50)
        self.l_left.setAlignment(Qt.AlignCenter)
        self.l_left.setFont(QFont('songti', pointSize=16))
        self.l_left.setText(LevelNote[0])
        self.l_down = QLabel(self)
        self.l_down.move(200, 225)
        self.l_down.setFixedSize(50, 50)
        self.l_down.setAlignment(Qt.AlignCenter)
        self.l_down.setFont(QFont('songti', pointSize=16))
        self.l_down.setText(LevelNote[3])
        self.l_right = QLabel(self)
        self.l_right.move(250, 175)
        self.l_right.setFixedSize(50, 50)
        self.l_right.setAlignment(Qt.AlignCenter)
        self.l_right.setFont(QFont('songti', pointSize=16))
        self.l_right.setText(LevelNote[2])
        self.l_b = QLabel(self)
        self.l_b.move(390, 190)
        self.l_b.setFixedSize(60, 60)
        self.l_b.setAlignment(Qt.AlignCenter)
        self.l_b.setFont(QFont('songti', pointSize=16))
        self.l_b.setText(LevelNote[4])
        self.l_a = QLabel(self)
        self.l_a.move(480, 150)
        self.l_a.setFixedSize(60, 60)
        self.l_a.setAlignment(Qt.AlignCenter)
        self.l_a.setFont(QFont('songti', pointSize=16))
        self.l_a.setText(LevelNote[5])
        # 显示窗口
        self.show()

    @run_with_exc
    def paintEvent(self, e):
        """绘制手柄的形状"""
        qp = QPainter()
        qp.begin(self)
        qp.setPen(QPen(QColor(0, 0, 0), 3, Qt.SolidLine))
        # 绘制手柄外形
        r = QRect(80, 80, 500, 240)
        qp.drawRoundedRect(r, 40, 40)
        # 绘制十字键
        qp.drawLine(200, 125, 200, 175)
        qp.drawLine(200, 175, 150, 175)
        qp.drawLine(150, 175, 150, 225)
        qp.drawLine(150, 225, 200, 225)
        qp.drawLine(200, 225, 200, 275)
        qp.drawLine(200, 275, 250, 275)
        qp.drawLine(250, 275, 250, 225)
        qp.drawLine(250, 225, 300, 225)
        qp.drawLine(300, 225, 300, 175)
        qp.drawLine(300, 175, 250, 175)
        qp.drawLine(250, 175, 250, 125)
        qp.drawLine(250, 125, 200, 125)
        # 绘制AB键
        qp.drawEllipse(480, 150, 60, 60)
        qp.drawEllipse(390, 190, 60, 60)
        # 绘制按键情况
        qp.setPen(Qt.NoPen)
        qp.setBrush(QColor(192, 0, 0, 70))
        if 1 in self.cur_note_pressed:
            qp.drawRect(QRect(200, 125, 50, 50))
        if 3 in self.cur_note_pressed:
            qp.drawRect(QRect(200, 225, 50, 50))
        qp.setBrush(QColor(0, 0, 192, 70))
        if 0 in self.cur_note_pressed:
            qp.drawRect(QRect(150, 175, 50, 50))
        if 2 in self.cur_note_pressed:
            qp.drawRect(QRect(250, 175, 50, 50))
        if 5 in self.cur_note_pressed:
            qp.setBrush(QColor(32, 96, 32, 70))
            qp.drawEllipse(QRect(480, 150, 60, 60))
        if 4 in self.cur_note_pressed:
            qp.setBrush(QColor(192, 96, 16, 70))
            qp.drawEllipse(QRect(390, 190, 60, 60))
        qp.end()

    @run_with_exc
    def on_note(self, note_pressed: set):
        # 根据音符绘制按键标志
        self.cur_note_pressed = note_pressed
        self.repaint()
        # 根据音符确定按键信息，将其以电脑按键的形式传递给FC模拟器
        if SendKeypressSignal:
            for t in range(6):
                if t in note_pressed:
                    win32api.keybd_event(EmuKeys[t], 0, 0, 0)
                else:
                    win32api.keybd_event(EmuKeys[t], 0, win32con.KEYEVENTF_KEYUP, 0)
