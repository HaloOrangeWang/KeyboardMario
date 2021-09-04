#ifndef TOTAL_H
#define TOTAL_H

#include <vector>
#include <map>
using namespace std;

#define SAMPLE_RATE 48000 //音频采样率
#define PRESS 120 //超过这个点才认为是按键了
#define PRESS_PEAKS_RATIO 0.65 //需要有这个比例以上的峰值点对应的幅值超过PRESS_RATIO
#define PRESS_RATIO 0.5 //只有在基波和谐波的幅值都超过了训练数据的这个比例，才认为是按了这个键
extern double NoteFreq[49]; //每个音符对应的频率。49项分别对应A2（110Hz）-A6（1760Hz）
extern map<int, vector<int>> InputOrder; //对一个数组采取奇数下标在前，偶数下标在后的排序。如 0 2 4 6 -> 0 4 2 6
extern map<double, double> NoteFreqPeaks[6]; //对于每个音符，每个音符对应了哪几个频率峰值。六项分别对应左上右下BA。map的key是频率峰值的真实频率，value是这些频率的幅值。

extern void setup_variables();
extern void setup_train_data_peaks(vector<double> train_data[6], double sample_ratio);
map<double, double> get_train_data_peak_1note(vector<double>& freq_data, double sample_ratio, double press_ratio);
extern vector<int> get_order(vector<int>& ary);
extern vector<double> fft(vector<int>& wave_ori, int wave_length);
extern vector<double> get_freq_data_by_note(vector<double>& freq_data, int wave_length);
extern vector<int> get_note_indexes(vector<double>& freq_data, double sample_ratio);

#endif