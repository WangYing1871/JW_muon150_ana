#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

// ROOT lib
#include "TFile.h"
#include "TTree.h"

// Personal lib
#include "constant.hh"
#include "position_data.hh"
#include <climits>
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"

using namespace std;
class hit_reconstruct
{
public:
	hit_reconstruct(string filename2save, int link_used, int set_layer, string position_filename, string z_filename, bool is_wr_txt = false);
	~hit_reconstruct();
	bool fill_data(string data_filename);
	void complete_adding();
	bool is_init_successed = false;
	bool is_datain_get = false;

private:
	bool position_judge(int link_num);
	void hit_result_sort(int layer);

	bool isConvertibleToInt(const string &str);

	int layer_num = 0;
	int user_set_layer_number = 4;
	// ��ʾÿ��link��Ӧ��������̽�����ĵڼ��㣬̽�����������֮�����һ���кü���link��
	// �����Ҫ�ڶ����������ж��ǵڼ���link��
	// ������ʵ��˵��bug�ģ�����������Ӷ�һ��̽������һ��ά�ȣ��ڽ����㷨��û�п�������
	// ������ڽ����Ĵ������ٽ����Ż�
	int layer_idx[cELINK_NUM * 4][2];
	double z_position[cMAX_DET_LAYER];
	bool load_z(string);
	bool load_position(string);

	// For data out file
	bool init_dataout(string filename);

	bool is_write_txt_file = false; // This function is only for debug;
	ofstream txt_fileout;
	ofstream max_position_file;
	void write_txt_data();
	TFile *hit_tfile;
	TTree *hit_ttree;
	int trigger_id = 0;
	int total_layer_num = 8;
	position_data detector_data[cMAX_DET_LAYER];

	void init_position_data();

	// For data in file
	int init_datain(string filename);
	bool link_enabled[cELINK_NUM] = {false};
	TFile *fec_data_file[cELINK_NUM];
	TTree *fec_data_tree[cELINK_NUM];
	int nhits_all[cELINK_NUM];
	int trigger_id_all[cELINK_NUM];
	int hit_strips[cELINK_NUM][cMAX_DEC_HITS];
	int hit_asic_chn[cELINK_NUM][cMAX_DEC_HITS];
	int hit_amp[cELINK_NUM][cMAX_DEC_HITS];
	//double hit_time[cELINK_NUM][cMAX_DEC_HITS];
	//double hit_max_position[cELINK_NUM][cMAX_DEC_HITS];
	int hit_time[cELINK_NUM][cMAX_DEC_HITS];
	int hit_max_position[cELINK_NUM][cMAX_DEC_HITS];

	int dimension_idx[cELINK_NUM][cMAX_DEC_HITS];
	int cluster_number[cELINK_NUM];
	int cluster_size[cELINK_NUM][cMAX_DEC_HITS];
	int cluster_hole[cELINK_NUM][cMAX_DEC_HITS];

	TBranch *b_nhits[cELINK_NUM];
	TBranch *b_trigger_id[cELINK_NUM];
	TBranch *b_hit_strips[cELINK_NUM];
	TBranch *b_hit_asic_chn[cELINK_NUM];
	TBranch *b_hit_amp[cELINK_NUM];
	TBranch *b_hit_time[cELINK_NUM];
	TBranch *b_hit_max_position[cELINK_NUM];
	TBranch *b_dimension_idx[cELINK_NUM];
	TBranch *b_cluster_number[cELINK_NUM];
	TBranch *b_cluster_size[cELINK_NUM];
	TBranch *b_cluster_hole[cELINK_NUM];
};
