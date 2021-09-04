import pyaudio


AudioConfig = {  # 音频输入：16位，双通道，48000采样率
    'format': pyaudio.paInt16,
    'channels': 2,
    'rate': 48000
}

SampleLen = 2048  # 一个样本音频的长度

TrainDataSet = '../TrainData'  # 训练数据的存放位置
TrainSamples = 5  # 每个音符使用多少个样本来进行训练
SendKeypressSignal = False

LevelNote = ['C3', 'E3', 'G3', 'B3', 'C5', 'E5']  # 六个按键对应的音符，从左到右分别是 左 上 由 下 B A
EmuKeys = [ord('A'), ord('W'), ord('D'), ord('S'), ord('J'), ord('K')]  # 模拟器里的左上右下AB分别对应哪个PC按键
NoteTrainFiles = ['C3.npy', 'E3.npy', 'G3.npy', 'B3.npy', 'C5.npy', 'E5.npy']  # 六个按键对应的训练数据文件名
