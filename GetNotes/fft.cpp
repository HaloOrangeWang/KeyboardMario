#include "total.h"
#include <vector>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

vector<int> LastNoteIndexes;

vector<int> get_order(vector<int>& ary)
{
	if (ary.size() <= 3) {
		abort();
	}
	else if (ary.size() == 4) {
		return vector<int>{ary[0], ary[2], ary[1], ary[3]};
	}
	else {
		vector<int> ary_even;
		vector<int> ary_odd;
		vector<int> ary_output;
		ary_even.reserve(ary.size() / 2);
		ary_odd.reserve(ary.size() / 2);
		ary_output.reserve(ary.size());
		for (int t = 0; t <= ary.size() - 1; t++) {
			if (t & 1) {
				ary_odd.push_back(ary[t]);
			}
			else {
				ary_even.push_back(ary[t]);
			}
		}
		vector<int> ary_even_output = get_order(ary_even);
		vector<int> ary_odd_output = get_order(ary_odd);
		ary_output.insert(ary_output.end(), ary_even_output.begin(), ary_even_output.end());
		ary_output.insert(ary_output.end(), ary_odd_output.begin(), ary_odd_output.end());
		return ary_output;
	}
}

vector<double> hamming(vector<int>& wave_ori, int wave_length)
{
	// �źżӴ�
	vector<double> wave_out(wave_length);
	for (int t = 0; t <= wave_length - 1; t++) {
		double w = 0.5 - 0.5 * cos(2 * M_PI * t / wave_length);
		wave_out[t] = w * wave_ori[t];
	}
	return wave_out;
}

vector<complex<double>> fft_1layer(vector<complex<double>>& wave_in, int unit, int wave_length)
{
	//����һ���FFT
	vector<complex<double>> wave_out(wave_length);
	for (int t = 0; t <= wave_length - 1; t++) {
		if (t & unit)
			continue;
		//����任����
		int n = unit << 1;
		int k = t & (unit - 1);
		complex<double> wk(cos(2 * M_PI * k / n), -sin(2 * M_PI * k / n));
		//����������
		wave_out[t] = wave_in[t] + wk * wave_in[t + unit];
		wave_out[t + unit] = wave_in[t] - wk * wave_in[t + unit];
	}
	return wave_out;
}

vector<double> fft(vector<int>& wave_ori, int wave_length)
{
	//����FFT���������ԭʼ����Ƶ�����ÿ��Ƶ�ʵ�ķ�ֵ��
	//��ʼ��
	vector<double> wave_with_window = hamming(wave_ori, wave_length);
	vector<int> order = InputOrder[wave_length];
	vector<complex<double>> wave_tmp(wave_length);
	for (int t = 0; t <= wave_length - 1; t++)
		wave_tmp[t] = complex<double>(wave_with_window[order[t]], 0);
	int unit = 1;
	//ִ��FFT����
	while (true) {
		wave_tmp = fft_1layer(wave_tmp, unit, wave_length);
		unit <<= 1;
		if (unit >= wave_length)
			break;
	}
	//�����ֵ
	vector<double> freq_data(wave_length / 2);
	for (int t = 0; t <= wave_length / 2 - 1; t++)
		freq_data[t] = sqrt(wave_tmp[t].real() * wave_tmp[t].real() + wave_tmp[t].imag() * wave_tmp[t].imag()) / wave_length;
	return freq_data;
}

vector<double> get_freq_data_by_note(vector<double>& freq_data, int wave_length)
{
	//����ÿ��������Ӧ��Ƶ�ʷ�ֵ
	//��ʼ��
	double sample_ratio = SAMPLE_RATE * 1.0 / wave_length; //�������������עfreq_data��ÿ���±��Ӧ�ĸ���ʵƵ��ֵ
	vector<double> freq_data_by_note(49);
	for (int t = 0; t <= 48; t++) {
		int real_freq_dx = int(NoteFreq[t] / sample_ratio + 0.5);
		freq_data_by_note[t] = freq_data[real_freq_dx];
	}
	return freq_data_by_note;
}

vector<int> get_note_indexes(vector<double>& freq_data, double sample_ratio)
{
	//������Ƶ��Ƶ�ʷ�ֵ��Ϣ���ж���������Щ����
	vector<int> note_indexes;
	//��ȡ�����Ƶ��Ƶ�׷�ֵ�㡣��ֵ��֮������Ҫ���һ��ȫ��
	map<double, double> peaks = get_train_data_peak_1note(freq_data, sample_ratio, PRESS * PRESS_RATIO_2);
	//�жϷ�ֵ��ķֲ�����Ƿ����ÿ���������ѵ�����ݡ��жϷ�ʽ�ǣ�ѵ�������е�ÿ����ֵ�㶼��������Ƶ������������
	for (int t = 0; t <= 5; t++) {
		int num_peak_match = 0;
		int num_peak_not_match = 0;
		double avr_peak_ratio = 0;
		double press_ratio_tmp = PRESS_RATIO;
		if (LastNoteIndexes.size() >= 1){
			for (int t0 = 0; t0 <= LastNoteIndexes.size() - 1; t0++) {
				if (LastNoteIndexes[t0] == t){
					press_ratio_tmp = PRESS_RATIO_2;
					break;
				}
			}
		}
		for (map<double, double>::iterator peak_it = NoteFreqPeaks[t].begin(); peak_it != NoteFreqPeaks[t].end(); peak_it++) {
			double freq = peak_it->first;
			double amp1 = peak_it->second;
			bool is_in_wave = false;
			for (map<double, double>::iterator peak_it2 = peaks.begin(); peak_it2 != peaks.end(); peak_it2++) {
				double freq2 = peak_it2->first;
				double amp2 = peak_it2->second;
				if (freq2 / freq < pow(2, 2 / 12.f) && freq / freq2 < pow(2, 2 / 12.f) && amp2 / amp1 > press_ratio_tmp) {
					is_in_wave = true;
					break;
				}
			}
			if (is_in_wave) {
				num_peak_match += 1;
			}
			else {
				num_peak_not_match += 1;
			}
		}
		double peak_match_ratio = (num_peak_match + num_peak_not_match == 0) ? 0 : num_peak_match * 1.0 / (num_peak_match + num_peak_not_match);
		if (peak_match_ratio >= PRESS_PEAKS_RATIO)
			note_indexes.push_back(t);
	}
	LastNoteIndexes = note_indexes;
	return note_indexes;
}