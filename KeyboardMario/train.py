# 获取一个音符的训练数据

from settings import *
import pandas as pd
import numpy as np
import argparse
import array
import get_notes
import os


def parse_pcm(raw_data: bytes) -> list:
    """将原始PCM字节流转换成的可以做FFT的格式"""
    short_array = array.array('h')
    short_array.frombytes(raw_data)
    channel0 = short_array[::2]
    channel1 = short_array[1::2]
    pcm_output = [channel0[t] + channel1[t] for t in range(2048)]
    return pcm_output


def train_1note():
    freq_data_all = np.zeros((TrainSamples, 1024), dtype=np.float64)
    p = pyaudio.PyAudio()
    # 获取音频信号
    for t in range(TrainSamples):
        input('获取第%d个训练数据' % t)
        stream = p.open(input=True, **AudioConfig)  # 打开麦克风输入
        raw_pcm_data = stream.read(2048)
        pcm_data = parse_pcm(raw_pcm_data)
        freq_data = get_notes.parse_note(pcm_data, 0)
        freq_data_all[t, :] = freq_data
        p.close(stream)
    return freq_data_all


def main():
    # 获取训练的音符
    parser = argparse.ArgumentParser()
    parser.add_argument('note', type=str)
    args = parser.parse_args()
    note_name = args.note
    # 获取训练结果
    get_notes.setup_vars()
    freq_data = train_1note()
    # freq_data = pd.DataFrame(freq_data, columns=['A2', '#A2', 'B2', 'C3', '#C3', 'D3', '#D3', 'E3', 'F3', '#F3', 'G3', '#G3',
    #                                              'A3', '#A3', 'B3', 'C4', '#C4', 'D4', '#D4', 'E4', 'F4', '#F4', 'G4', '#G4',
    #                                              'A4', '#A4', 'B4', 'C5', '#C5', 'D5', '#D5', 'E5', 'F5', '#F5', 'G5', '#G5',
    #                                              'A5', '#A5', 'B5', 'C6', '#C6', 'D6', '#D6', 'E6', 'F6', '#F6', 'G6', '#G6', 'A6'])
    # 将训练结果保存成文件
    if not os.path.exists('%s' % TrainDataSet):
        os.makedirs('%s' % TrainDataSet)
    np.save('%s/%s.npy' % (TrainDataSet, note_name), freq_data)


if __name__ == '__main__':
    main()
