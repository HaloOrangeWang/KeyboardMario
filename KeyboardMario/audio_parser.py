from settings import *
from PyQt5.QtCore import pyqtSignal, QObject
import get_notes
import array
import time


class AudioParser(QObject):
    note_signal = pyqtSignal(set)  # 按的音符发生了变化，将变化信息发给主界面

    def __init__(self):
        super().__init__()
        p = pyaudio.PyAudio()
        self.stream = p.open(input=True, **AudioConfig)  # 打开麦克风输入

    @staticmethod
    def parse_pcm(raw_data: bytes) -> list:
        """将原始PCM字节流转换成的可以做FFT的格式"""
        short_array = array.array('h')
        short_array.frombytes(raw_data)
        channel0 = short_array[::2]
        channel1 = short_array[1::2]
        pcm_output = [channel0[t] + channel1[t] for t in range(SampleLen)]
        return pcm_output

    def read_from_stream(self):
        last_note_pressed = set()  # 记录上一次按了哪些音符
        while True:
            raw_pcm_data = self.stream.read(SampleLen)
            # st = time.time()
            pcm_data = self.parse_pcm(raw_pcm_data)
            note_pressed = get_notes.parse_note(pcm_data, 2)
            note_pressed = set(note_pressed)
            # ed = time.time()
            # print('ed - st = ', ed - st, ', note_pressed = ', note_pressed)
            # print('各个音符的频谱最大值为: ', max_d)
            # draw(freq_data, 500)
            # time.sleep(400)
            if note_pressed != last_note_pressed:  # 如果音符发生了变化，则通知主界面
                last_note_pressed = note_pressed
                self.note_signal.emit(note_pressed)
