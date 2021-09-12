#include "total.h"
#include <math.h>

double NoteFreq[49];
map<int, vector<int>> InputOrder;
map<double, double> NoteFreqPeaks[6];

void setup_variables()
{
	//ÿ����������ʵƵ��
	for (int t = 0; t <= 48; t++)
		NoteFreq[t] = 110 * pow(2, t / 12.f);
	//����2048���4096���FFTʱ���źŵļ���˳��
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
	//����һ��ѵ����������Ƶ�����ݣ����������������Ƶ�ʷ�ֵ��
	for (int t = 2; t <= freq_data.size() - 1; t++) {
		double freq = t * sample_ratio;
		//���Ƶ����ֵ���������������Ӧ��Ƶ��ֵ��1760Hz�����Ͳ��ü����ˡ����Ƶ����ֵ�������������Ӧ��Ƶ�ʣ���Ӧ��Ϊ�Ƿ�ֵ�㡣
		if (freq < NoteFreq[0])
			continue;
		if (freq > MAX_FREQ)
			break;
		//�ж����Ƶ�ʵ��ǲ���һ����ֵ����ֵ��Ƶ�ʷ�ֵ�������PRESS������������ֵ��Ƶ������Ҫ���1��ȫ��
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
		//��¼��ֵ��
		freq_peaks.insert(make_pair(freq, freq_data[t]));
		// printf("peak as %.2f is %.2f\n", freq, freq_data[t]);
	}
	return freq_peaks;
}

void setup_train_data_peaks(vector<double> train_data[6], double sample_ratio)
{
	//����ÿһ��ѵ������������������Ƶ�ʷ�ֵ
	for (int t = 0; t <= 5; t++)
		NoteFreqPeaks[t] = get_train_data_peak_1note(train_data[t], sample_ratio, PRESS);
}