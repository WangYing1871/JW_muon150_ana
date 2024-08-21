#ifndef dec_pro_hh
#define dec_pro_hh 1

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TH1I.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <iomanip>
#include <TStyle.h>

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <numeric>

#include "hit_dec.hh"
#include "constant.hh"

using namespace std;

class dec_pro
{
public:
    dec_pro(string base_txt, string rms_txt, string input_file_name, string root_file_name, string save_path, bool is_old_data, bool is_write_txt, int asic_num, string encoding_list_filename, bool is_allow_dec_discon, double LSB);
    ~dec_pro();

    double LSB;
    int asic_num;
    string save_path;
    string root_filename;
    bool is_write_txt;
    bool is_encode_384 = false;
    vector<vector<double>> base;
    vector<vector<double>> noise;

private:
    int anhits;
    int triggerid;
    int adas_hit_idx[cMAX_DEC_HITS];
    int adas_hit_chl[cMAX_DEC_HITS];
    int adas_hit_amp[cMAX_DEC_HITS];

    int cnt_tmp = 0;

    vector<vector<int>> idx;
    vector<vector<int>> chl;
    vector<vector<int>> amp;
    vector<int> trig;

    TFile *adas_dec_file;
    TTree *adas_dec_tree;
    int nhits;
    int trigger_id;
    int dimension_idx[cMAX_DEC_HITS];
    int hit_strips[cMAX_DEC_HITS];
    int hit_asic_chn[cMAX_DEC_HITS];
    int hit_time[cMAX_DEC_HITS];
    int hit_max_position[cMAX_DEC_HITS];
    int hit_amp[cMAX_DEC_HITS];
    int cluster_number;
    int cluster_holed_num[cMAX_DEC_HITS];
    int cluster_size[cMAX_DEC_HITS];

    ofstream txt_file;
    string txt_filename;
    bool is_write_txt_file;

    vector<vector<vector<int>>> idx_dec;
    vector<vector<vector<int>>> chl_dec;
    vector<vector<vector<int>>> amp_dec;
    vector<bool> is_qx_dec;
    vector<bool> is_qy_dec;

    hit_dec *dec_task;
    hit_dec *dec_task_cg; // Encoding with Complete graph

    ofstream log_file;

    // 读取基线数据
    bool read_base(string base_txt);
    // 读取噪声数据
    bool read_noise(string noise_txt);
    // 读取root文件
    bool read_root(string input_file_name);

    void write_base();

    // 扣除基线
    void dec_base();

    // 数据解码处理
    void data_dec_process(bool is_old_data);

    // 数据解码
    void data_dec();

    // 初始化root
    bool init_root();

    // 写入txt
    void write_txt();

    // 写入root
    void complete_adding();

    // 直接读出
    void direct_read();

    // 多路复用读出
    void mux_read(int k);

    //
    void hit_data_dec(int index, int event_id);
    void v_clear();
};
#endif
