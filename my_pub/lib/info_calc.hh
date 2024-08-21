#ifndef INFO_CALC_HH
#define INFO_CALC_HH

#include <string>

#include "position_data.hh"
#include "TFile.h"
#include "TH2F.h"
#include "TTree.h"
#include "constant.hh"
//#include "global_function.hh"
//#include "global_variable.hh"
//#include "hit_correct.hh"

using namespace std;

class info_calc
{
public:
    info_calc();
    ~info_calc();

    bool set_input_filename(string name);
    //bool set_alignment_filename(string name)
    //{
    //    m_alignment_filename = name;
    //    return true;
    //}
    //bool set_correct_process(hit_correct *correct);

    //void set_run_amp_sum(bool b) { mb_run_amp_sum = b; }

private:
    // init parameters
    string hit_data_filename;
    //string m_alignment_filename;
    string m_png_path = "./result/";

    //hit_correct *m_correct;

    // info settings
    //bool mb_run_amp_sum = 1;
    //bool mb_run_eff_calc = 1;
    //bool mb_stats_track = 0;
    //bool mb_show_time_dist = 0;
    //bool mb_run_pos_res = 0;
    //bool mb_check_correct_process = 0;
    //bool mb_check_align_var = 0;
    //bool mb_stats_hitnums = 0;
    //bool mb_show_vars_corr = 1;
    //bool mb_check_micro_TPC = 0;
    //bool mb_tomography = 0;
    //bool mb_amp_eff_map = 0;

    // bool mb_run_amp_sum = 1;
    // bool mb_run_eff_calc = 1;
    // bool mb_stats_track = 0;
    // bool mb_show_time_dist = 0;
    // bool mb_run_pos_res = 0;
    // bool mb_check_correct_process = 0;
    // bool mb_check_align_var = 0;
    // bool mb_stats_hitnums = 0;
    // bool mb_show_vars_corr = 0;
    // bool mb_check_micro_TPC = 0;
    // bool mb_tomography = 0;
    // bool mb_amp_eff_map = 0;

    //bool mb_show_data = 0;

public:
    bool init();
    bool execute();
    bool finish();

private:
    bool init_input_file();

    bool run_amp_sum();
    bool run_eff_calc();
    bool run_pos_res();
    /*
    bool run_eff_calc_use_changed_correct();
    bool run_eff_calc_notuse_correct();
    bool run_eff_calc_notuse_correct_combine_xy();
    bool run_eff_calc_notuse_correct_combine_xy_split();
    bool run_eff_calc_use_alignment();
    bool run_eff_calc_use_alignment_single_dimension();
    bool stats_track();
    bool stats_track_map();
    bool show_time_dist();
    bool run_pos_res_position_binned();
    bool run_pos_res_angular_binned();
    bool run_pos_res_angular_binned_micro_TPC();
    bool check_correct_process();
    bool check_align_var();
    bool stats_hitnums();
    bool show_vars_corr();
    bool check_micro_TPC();
    bool tomography();
    bool tomography_2();
    bool amp_eff_map();

    bool show_data();

    bool run_amp_sum_subfunc(TH2F **);
    bool run_eff_calc_notuse_correct_combine_xy_subfunc(TH2F **);
*/
    // input data
    TFile *hit_data_file;
    TTree *hit_data_tree;
    int entries;
    int det_layer_used;

    double kx;
    double ky;
    double bx;
    double by;
    double rmse_x;
    double rmse_y;
    double r2_x;
    double r2_y;
    int is_x_hit[cMAX_DET_LAYER] ={0};
    int is_y_hit[cMAX_DET_LAYER] ={0};
    double x[cMAX_DET_LAYER] = {0};
    double y[cMAX_DET_LAYER] = {0};
    double z[cMAX_DET_LAYER] = {0};
    double x_amp[cMAX_DET_LAYER] ={0};
    double y_amp[cMAX_DET_LAYER] ={0};
    int x_strip_num[cMAX_DET_LAYER] = {0};
    int y_strip_num[cMAX_DET_LAYER] = {0};

    double cDET_SIZE[6]={400,400,400,400,400,400};
    double cDET_Z[6]={0,148.5,299.9,447.5,598.8,749.2};
    //long time_stamp;
    //int trigger_id;
    //int total_layer_num;
    //TBranch* b_trigger_id;
    //TBranch* b_total_layer_num;
    //int board_id[cTOTAL_CHN_NUM];
    //int chip_id[cTOTAL_CHN_NUM];
    //int chn_id[cTOTAL_CHN_NUM];
    //int adc_data[cTOTAL_CHN_NUM];
    //int adc_max_ceil[cTOTAL_CHN_NUM];
    //int adc_data_sca[cTOTAL_CHN_NUM][cSCA_NUM];
    //position_data detector_data[cMAX_DET_LAYER];
    //TBranch* b_detector_data[cMAX_DET_LAYER];

    //double alignment[cMAX_DET_LAYER][6];  // alignment test
};

#endif
