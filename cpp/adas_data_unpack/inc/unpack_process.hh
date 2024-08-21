#ifndef unpack_process_hh
#define unpack_process_hh 1

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

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <sstream>
#include "constant.hh"

using namespace std;

class unpack_process
{
public:
    unpack_process(vector<uint32_t> initial_data, bool is_old_data, int asic_num, string root_filename, bool is_write_txt, bool is_base_data, double LSB, string save_path, string file_write);
    ~unpack_process();
    // old
    int asic_num;
    double LSB;
    vector<vector<vector<uint32_t>>> ch;
    vector<vector<vector<uint32_t>>> amp;

    // new
    vector<vector<uint32_t>> data;
    vector<vector<uint32_t>> dataidx;
    vector<uint32_t> initial_data;

    string save_path;
    string file_write;

private:
    // 前期处理
    bool is_read_success_flag;
    vector<uint32_t> start_index;
    vector<uint32_t> end_index;
    vector<uint32_t> cnt_idx;
    vector<vector<uint32_t>> adc_data;
    uint32_t h[4] = {4294945450, 4294949819, 4294954188, 4294958557};
    uint32_t e[4] = {2863333375, 3149660159, 3435986943, 3722313727};

    vector<vector<uint32_t>> adas_idx;
    vector<vector<uint32_t>> adas_chl;
    vector<vector<uint32_t>> adas_amp;
    vector<uint32_t> trig;

    TFile *adas_hit_file;
    TTree *adas_hit_tree;
    int nhits;
    int triggerid;
    int adas_hit_idx[1280];
    int adas_hit_chl[1280];
    int adas_hit_amp[1280];

    std::vector<int> m_hit_index;

    TFile* m_view_file;
    int m_index;



    // 解包
    void init(bool, bool);
    // 计算基线mean、rms和threshold
    void clac_base_rms(vector<vector<uint32_t>> data, vector<vector<uint32_t>> &base, vector<double> &means, vector<double> &rms, vector<double> &threshold, vector<double> &rms_bin, vector<double> &rms128, vector<double> &means128);
    // 画图
    void draw(vector<vector<double>> means, vector<vector<double>> rms);

    // 将阈值转换为usb输入文件
    void threshold_to_usb(vector<double> threshold);
    void threshold_to_usb_new(vector<double> threshold);
    void threshold_to_gbt(vector<double> threshold);

    // v3.0解包 用
    void find_start_end();
    // v2.1解包 用
    void split_data(vector<vector<uint32_t>> data, uint32_t header, uint32_t ender, vector<vector<uint32_t>> &adc_data);
    // 拆开通道号和幅度
    void spilt_channel_amp(vector<vector<uint32_t>> data, vector<vector<uint32_t>> &channel, vector<vector<uint32_t>> &amp, bool is_base_data, vector<vector<uint32_t>> &adasidx);

    // 初始化root文件
    bool init_root(string);
    // 将解包的数据 写入root文件 待后续解码分析
    void write_root(bool);
    // 写入txt文件 没什么用
    void write_txt(string);

    // 将基线mean、rms和threshold写入txt文件
    void write_base_txt(vector<vector<double>> means, vector<vector<double>> rms, vector<vector<double>> threshold);

    // root文件的写入刷新
    void complete_adding();

    // 查看解包数据有没有问题
    void write_adc_data();
};

#endif
