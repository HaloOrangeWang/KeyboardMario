#ifndef TOTAL_H
#define TOTAL_H

#include <vector>
#include <map>
using namespace std;

#define SAMPLE_RATE 48000 //��Ƶ������
#define PRESS 120 //������������Ϊ�ǰ�����
#define PRESS_PEAKS_RATIO 0.65 //��Ҫ������������ϵķ�ֵ���Ӧ�ķ�ֵ����PRESS_RATIO
#define PRESS_RATIO 0.5 //ֻ���ڻ�����г���ķ�ֵ��������ѵ�����ݵ��������������Ϊ�ǰ��������
extern double NoteFreq[49]; //ÿ��������Ӧ��Ƶ�ʡ�49��ֱ��ӦA2��110Hz��-A6��1760Hz��
extern map<int, vector<int>> InputOrder; //��һ�������ȡ�����±���ǰ��ż���±��ں�������� 0 2 4 6 -> 0 4 2 6
extern map<double, double> NoteFreqPeaks[6]; //����ÿ��������ÿ��������Ӧ���ļ���Ƶ�ʷ�ֵ������ֱ��Ӧ��������BA��map��key��Ƶ�ʷ�ֵ����ʵƵ�ʣ�value����ЩƵ�ʵķ�ֵ��

extern void setup_variables();
extern void setup_train_data_peaks(vector<double> train_data[6], double sample_ratio);
map<double, double> get_train_data_peak_1note(vector<double>& freq_data, double sample_ratio, double press_ratio);
extern vector<int> get_order(vector<int>& ary);
extern vector<double> fft(vector<int>& wave_ori, int wave_length);
extern vector<double> get_freq_data_by_note(vector<double>& freq_data, int wave_length);
extern vector<int> get_note_indexes(vector<double>& freq_data, double sample_ratio);

#endif