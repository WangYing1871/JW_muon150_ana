#ifndef fit_pro_hh
#define fit_pro_hh 1

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TH1I.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <iomanip>
#include "TROOT.h"
#include <TStyle.h>
#include <TLatex.h>

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <numeric>

#include <eigen3/Eigen/Dense>
#include <cmath>
#include <fstream>
#include "constant.hh"
#include "position_data.hh"
#include "my_poca.hh"
#include "MyTracker.hh"

#include "json.hh"
#include "DataPoint.hh"
#include "DBSCAN.hh"

using namespace std;
using json = nlohmann::basic_json<nlohmann::ordered_map>;

class fit_pro
{
public:
    fit_pro(string input_file_name, string align_filename, string range_filename, int layer_num, float rmse_precision, bool idx_discontinuous, int idx_disc, double sigma, double LSB, string save_path, bool is_req_xy, bool is_write_txt, bool is_angle, double angle, bool is_muon_scatter);
    ~fit_pro();

    bool is_file_get = false;

    int det_layer_used;
    int len;
    int idx_disc;
    bool idx_discontinuous;
    float rmse_precision;
    float fit_location;
    double sigma;
    double LSB;
    string save_path;
    bool is_req_xy;
    bool is_write_txt;
    bool is_angle;
    double angle;
    bool is_muon_scatter;

    int setlayer = 0;
    int setlayer_used = 6;

    bool is_down = false;

    vector<vector<float>> fec_datax;
    vector<vector<float>> fec_datay;

    vector<pair<float, float>> xidx_data;
    vector<pair<float, float>> yidx_data;
    vector<vector<double>> xaim_data;
    vector<vector<double>> yaim_data;
    vector<vector<double>> xaim_data_amp;
    vector<vector<double>> yaim_data_amp;
    vector<vector<double>> xaim_data_cluster;
    vector<vector<double>> yaim_data_cluster;

private:
    vector<float> x_alignment;
    vector<float> y_alignment;
    vector<float> z_alignment;

    // 读取root文件
    int total_entried_num = 0;

    TFile *hit_data_file;
    TTree *hit_data_tree;
    int trigger_id;
    int total_layer_num;
    position_data detector_data[cMAX_DET_LAYER];
    TBranch *b_triggera_id;
    TBranch *b_total_layer_num;
    TBranch *b_detector_data[cMAX_DET_LAYER];

    vector<float> alignment;
    vector<float> dec_range;
    vector<int> dec_idx;

    vector<float> fit_range;

    vector<float> dec_construct;
    vector<int> trig_id;

    vector<vector<float>> t_channel_x;
    vector<vector<int>> t_amp_x;
    vector<vector<int>> t_dimension_x;
    vector<vector<float>> t_cluster_x;

    vector<vector<float>> t_channel_y;
    vector<vector<int>> t_amp_y;
    vector<vector<int>> t_dimension_y;
    vector<vector<float>> t_cluster_y;

    // data_fit
    int file_select = 0;
    vector<int> trig;

    vector<vector<float>> t_channel_2d;
    vector<vector<int>> t_amp_2d;
    vector<vector<int>> t_dimension_2d;
    vector<vector<float>> t_cluster_2d;

    vector<pair<float, float>> idx_data;
    vector<vector<double>> aim_data;
    vector<vector<double>> aim_data_amp;
    vector<vector<double>> aim_data_cluster;

    vector<int> all_assembly;

    vector<vector<float>> fec_data_combine;
    vector<vector<float>> fec_data_combine_cluster;

    vector<vector<float>> fec_data_fit;

    // result
    vector<tuple<float, float, float>> fit_data;
    vector<vector<float>> fit_data_all;

    vector<vector<float>> fit_data_all_up;
    vector<vector<float>> fit_data_all_down;

    vector<vector<float>> scater;
    ofstream poca_file;

    double tx;
    double ty;
    double tz;

    // spatial calulate
    vector<vector<float>> tempx;
    vector<vector<float>> tempy;
    string spatial_name;
    double vld_x;
    double vld_y;

    // draw
    TH1F *spatial;

    // out
    ofstream outall;

    // read detector index
    void idx_gen();
    // read alignment
    void read_dec_file(string filename, vector<float> &dec);
    // read root file
    int read_root_file(string data_infilename);
    // read alignment
    void read_alignment();
    // convert position data
    bool convert_position_data();

    //
    void run();

    void process_data(char axis);

    void clear_data();

    void clear_init();

    void poca_track();

    void write_poca(MyPocaResult poca_result);

    // 筛选数据，要求每层探测器都要有hit，如果存在某层没有hit，则舍弃该次缪子事件
    bool data_select();
    // 筛选出每次缪子事件中可能存在的径迹组合
    void data_combine(int level, int dec_idx, vector<int> &count, vector<vector<int>> &containers, vector<vector<float>> &t_channel_2d, vector<float> &temp, vector<float> &temp_current);
    // 挑选出每次缪子事件中最优的径迹，然后反推到待测探测器上
    void data_fit();

    // 寻找xy同时击中的径迹
    void filterAndCombine();

    void findTarget(vector<vector<double>> &aimData, vector<vector<double>> &ampData, vector<vector<double>> &clusterData, vector<vector<float>> &fecData, vector<vector<double>> &dataSecondary, vector<vector<double>> &dataAmpSecondary, vector<vector<double>> &dataClusterSecondary, double txOrTy);

    void cal_all_energy(vector<vector<double>> &xampData, vector<vector<double>> &yampData, vector<double> &Ampall);

    void calculateSpatial(vector<vector<double>> &dataSecondary, vector<vector<double>> &dataAmpSecondary, vector<vector<double>> &dataClusterSecondary, vector<vector<double>> &dataTertiary, vector<vector<double>> &dataAmpTertiary, vector<vector<double>> &dataClusterTertiary, const string &filename, bool xory);

    void writeSpatial();

    void draw_spatial(vector<vector<double>> rms, bool xory);

    void draw_energy(vector<vector<double>> energy, bool xory);

    void draw_all_energy(vector<double> energy);

    void draw_cluster(vector<vector<double>> cluster, bool xory);

    void out_result();

    // Data out init
    int trig_event = 0;
    double kx_up, ky_up, bx_up, by_up, rmse_x_up, rmse_y_up;
    double kx_down, ky_down, bx_down, by_down, rmse_x_down, rmse_y_down;
    double x_uper[cMAX_DET_LAYER] = {0};
    double y_uper[cMAX_DET_LAYER] = {0};
    double z_uper[cMAX_DET_LAYER] = {0};
    int x_strip_num_up[cMAX_DET_LAYER] = {0};
    int y_strip_num_up[cMAX_DET_LAYER] = {0};
    double x_downer[cMAX_DET_LAYER] = {0};
    double y_downer[cMAX_DET_LAYER] = {0};
    double z_downer[cMAX_DET_LAYER] = {0};
    int x_strip_num_down[cMAX_DET_LAYER] = {0};
    int y_strip_num_down[cMAX_DET_LAYER] = {0};

    void draw_scatter();

    void write_event_json_file(int, MyPocaResult);
    json event_json;
};
#endif
