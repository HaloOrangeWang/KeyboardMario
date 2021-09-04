from settings import *
import numpy as np
import get_notes
import array
import time


def read_train_data():
    """获取训练数据，提供给音频分析部分"""
    train_data = []
    for t in range(6):
        b = np.load(TrainDataSet + '/' + NoteTrainFiles[t])
        train_data.append(np.average(b, axis=0).tolist())
    get_notes.setup_vars(train_data[0], train_data[1], train_data[2], train_data[3], train_data[4], train_data[5], 1024, AudioConfig['rate'] / 2048)
    return train_data


def parse_pcm(raw_data: bytes) -> list:
    """将原始PCM字节流转换成的可以做FFT的格式"""
    short_array = array.array('h')
    short_array.frombytes(raw_data)
    channel0 = short_array[::2]
    channel1 = short_array[1::2]
    pcm_output = [channel0[t] + channel1[t] for t in range(2048)]
    return pcm_output


def main():
    train_data = read_train_data()
    p = pyaudio.PyAudio()
    # 获取音频信号
    stream = p.open(input=True, **AudioConfig)  # 打开麦克风输入
    raw_pcm_data = stream.read(2048)
    pcm_data = parse_pcm(raw_pcm_data)
    freq_data = get_notes.parse_note(pcm_data, 0)
    freq_dat2 = get_notes.parse_note(pcm_data, 2)
    p.close(stream)
    print('7')


if __name__ == '__main__':
    main()
