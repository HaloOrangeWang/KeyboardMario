#include "total.h"
#include <math.h>

double NoteFreq[49];
map<int, vector<int>> InputOrder;
map<double, double> NoteFreqPeaks[6];

void setup_variables()
{
	//每个音符的真实频率
	for (int t = 0; t <= 48; t++)
		NoteFreq[t] = 110 * pow(2, t / 12.f);
	//计算2048点和4096点的FFT时，信号的计算顺序
	vector<int> seq_2048(2048);
	for (int t = 0; t <= 2047; t++)
		seq_2048[t] = t;
	vector<int> order_2048 = get_order(seq_2048);
	vector<int> seq_4096(4096);
	for (int t = 0; t <= 4095; t++)
		seq_4096[t] = t;
	vector<int> order_4096 = get_order(seq_4096);
	InputOrder.insert(make_pair(2048, order_2048));
	InputOrder.insert(make_pair(4096, order_4096));
}

map<double, double> get_train_data_peak_1note(vector<double>& freq_data, double sample_ratio, double press_ratio)
{
	map<double, double> freq_peaks;
	//根据一个训练的音符的频率数据，来计算这个音符的频率峰值点
	for (int t = 2; t <= freq_data.size() - 1; t++) {
		double freq = t * sample_ratio;
		//如果频率数值超过了最高音符对应的频率值（1760Hz），就不用继续了。如果频率数值低于最低音符对应的频率，不应认为是峰值点。
		if (freq < NoteFreq[0])
			continue;
		if (freq > MAX_FREQ)
			break;
		//判断这个频率点是不是一个峰值。峰值的频率幅值必须高于PRESS。相邻两个峰值的频率至少要相差1个全音
		if (freq_data[t] < press_ratio)
			continue;
		double freq_low2_index = freq / pow(2, 2 / 12.f) / sample_ratio;
		double freq_hi2_index = freq * pow(2, 2 / 12.f) / sample_ratio;
		bool is_peak = true;
		for (int t0 = int(freq_low2_index) + 1; t0 <= freq_hi2_index; t0++) {
			if (freq_data[t0] > freq_data[t]) {
				is_peak = false;
				break;
			}
		}
		if (!is_peak)
			continue;
		//记录峰值点
		freq_peaks.insert(make_pair(freq, freq_data[t]));
		// printf("peak as %.2f is %.2f\n", freq, freq_data[t]);
	}
	return freq_peaks;
}

void setup_train_data_peaks(vector<double> train_data[6], double sample_ratio)
{
	//对于每一个训练的音符，计算它的频率峰值
	for (int t = 0; t <= 5; t++)
		NoteFreqPeaks[t] = get_train_data_peak_1note(train_data[t], sample_ratio, PRESS);
}