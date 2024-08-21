#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "constant.hh"

using namespace std;

class hit_dec
{
public:
	// Filling the class with a specific encoding file
	hit_dec(string encoding_lists_filename, int max_chn_num = 20, int encoded_det_chn_num = 512, bool is_dis_warning = false);
	~hit_dec();
	bool is_list_get = false;
	bool is_allow_discontinue = true; // Allow one discontinue channel by default

	// Decoding the inputing data ��Ҫ������ѧͨ�������0��ʼ��
	bool run_dec(vector<int> chn, vector<int> idx);

	// All the possible decoding results
	vector<int> hit_strip;
	// The corresponding hit index of the hit_strip
	vector<int> hit_idx;
	// The correspondong hit sequence of the input aget_chn
	vector<int> hit_seq;
	vector<int> cluster_num;
	vector<int> cluster_holed;
	// vector<int> aget_chn_num_use;
	vector<int> strip_all;

	string lists_filename;

	// Clear all the element before decoding new data
	void clear();

	// Encoding lists created with electronics at the first column and ordered. In the future version, it should be private.
	array<vector<int>, 384> encoded_lists;

private:
	// Filling the encoded_lists
	bool get_encoded_lists(string filename);
	int max_hit_chn; //
	int det_chn_num;

	int hit_dec_total(vector<int> chn, vector<int> idx);

	/**
	 * @param d_chn �����������
	 * @return
	 */
	vector<int> bubble_sort_chn(vector<int> &d_chn);

	vector<int> calc_continuation(vector<int> d_chn);

	template <typename T>
	void resort(vector<T> &a, vector<int> idx);
	bool is_display_warning = false;
};

/// <summary>
/// This function is used to sort the input array with the input index
/// </summary>
/// <typeparam name="T">Array type </typeparam>
/// <param name="a">Array need to resort </param>
/// <param name="idx"></param>
template <typename T>
inline void hit_dec::resort(vector<T> &a, vector<int> idx)
{
	if (a.size() != idx.size())
	{
		cout << cRED << "[Error]: In function resort, the length of a and idx are not equal" << cRESET << endl;
		return;
	}
	vector<T> a_tmp;
	for (int i = 0; i < idx.size(); i++)
	{
		if (idx[i] > idx.size())
		{
			cout << cRED << "[Error]: The contect of idx in function hit_dec::resort is error" << cRESET << endl;
			return;
		}
		a_tmp.push_back(a[idx[i]]);
	}
	a.clear();
	a.insert(a.end(), a_tmp.begin(), a_tmp.end());
}
