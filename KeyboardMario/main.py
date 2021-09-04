from settings import *
from PyQt5.QtWidgets import QApplication
from audio_parser import AudioParser
from window import Window
import threading
import numpy as np
import time
import get_notes
import sys


def read_train_data():
    """获取训练数据，提供给音频分析部分"""
    train_data = []
    for t in range(6):
        b = np.load(TrainDataSet + '/' + NoteTrainFiles[t])
        train_data.append(np.average(b, axis=0).tolist())
    get_notes.setup_vars(train_data[0], train_data[1], train_data[2], train_data[3], train_data[4], train_data[5], 1024, AudioConfig['rate'] / 2048)


def main():
    # 根据训练数据初始化算法参数
    read_train_data()
    # 初始化主界面
    audio_in = AudioParser()
    app = QApplication(sys.argv)
    main_window = Window(audio_in)
    main_window.init_window()
    # 初始化音频输入，开多线程读取音频信息
    audio_in.note_signal.connect(main_window.on_note)
    audio_thread = threading.Thread(target=audio_in.read_from_stream)
    audio_thread.start()

    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
