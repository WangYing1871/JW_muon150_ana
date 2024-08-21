#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>

#include "constant.hh"
#include "MyTracker.hh"
#include "position_data.hh"
#include "linear_fit.hh"
#include "json.hh"


using namespace std;
using json = nlohmann::basic_json<nlohmann::ordered_map>;
class best_tracker
{
public:
	best_tracker(string data_infilename, string save_filename, int layer = 4, double angle_x = 0, double angle_y = 0);
	~best_tracker();
	bool is_file_get = false;
	bool run();
	void show_result();
	void set_rmse_vth(double);
	void set_possible_strip_vth(int);
	void set_min_layer(int);
	void set_layer_used(int);
	void load_offset_param(string filename);
	string save_file_path = "./";
	double acq_time = 5;
	void set_tomography_system(bool);
	void set_target_layer_calc(int layer, bool is_first = false);

private:
	bool is_tomography_system = false;
	int det_layer_used = 4;
	int init(string data_infilename, string save_filename);
	// Data in initial
	int init_datain(string data_infilename);
	bool init_dataout(string filename);
	int total_entried_num = 0;
	int valid_event_cnts = 0;
	
	TFile* hit_data_file;
	TTree* hit_data_tree;
	int trigger_id;
	int total_layer_num;
	position_data detector_data[cMAX_DET_LAYER];
	TBranch* b_triggera_id;
	TBranch* b_total_layer_num;
	TBranch* b_detector_data[cMAX_DET_LAYER];
	// Rotation angle of x and y axes
	double theta_x;
	double theta_y;

	bool user_set_layer_used[cMAX_DET_LAYER] = { false };
	

	double x_hit[cMAX_DET_LAYER];
	double y_hit[cMAX_DET_LAYER];
	MyTracker tracker_x;
	MyTracker tracker_y;

	int possible_strip_num = 3;
	double rms_vth = 10;
	int min_layer = 3;
	bool best_dec_fit(array<vector<double>, cMAX_DET_LAYER> possible_hit, bool* layer_used, double& best_k, double& best_b, int* best_idx, double& best_rmse, double& best_r2);
	bool least_square_fit(vector<double> x, vector<double> y,
		double& k, double& b,
		double& rmse, double& R2);
	bool least_square_fit(double* x, double* y, int n,
		double& k, double& b,
		double& rmse, double& R2);

	// Data out initial
	double kx;
	double ky;
	double bx;
	double by;
	double rmse_x;
	double rmse_y;
	double r2_x;
	double r2_y;
	int is_x_hit[cMAX_DET_LAYER] = {0};
	int is_y_hit[cMAX_DET_LAYER] = {0};
	double x[cMAX_DET_LAYER] = {0};
	double y[cMAX_DET_LAYER] = {0};
	double z[cMAX_DET_LAYER] = {0};
	double x_amp[cMAX_DET_LAYER] = {0};
	double y_amp[cMAX_DET_LAYER] = {0};
	int x_strip_num[cMAX_DET_LAYER] = {0};
	int y_strip_num[cMAX_DET_LAYER] = {0};



	string data_file_name;
	string dout_filename_prefix;
	TFile* dataout_file;
	TTree* dataout_tree;
	string txt_filename_tracker;
	ofstream txt_tracker_file;
	string txt_filename_hit;
	ofstream txt_hit_file;
	string txt_filename_hit_aln;
	ofstream txt_hit_aln_file;
	string txt_filename4aln;
	ofstream txt_file4aln;

	void offset_aln();
	double x_aln[cMAX_DET_LAYER] = { 0 };
	double y_aln[cMAX_DET_LAYER] = { 0 };
	double z_aln[cMAX_DET_LAYER] = { 0 };

	void write_txt_file();

	bool hough_trans(array<vector<double>, cMAX_DET_LAYER> possible_hit, bool* link_used, double& k, double& b);
	void hough_trans_back(array<vector<double>, cMAX_DET_LAYER> possible_hit, bool* link_used, double k, double b, double* x_h);
	int nbins = 401;
	double cPI = TMath::Pi();
	double c_theta_max = cPI / 2.;
	double c_theta_min = -cPI / 2.;
	double c_r_max = 400 + 600; // 400mm for detector size and 587mm for distance
	double c_r_min = -c_r_max;
	TH2D* hough_map;

	bool is_calc_target_offset = false;
	int target_layer = 0;
	TH1D* h_target8_x_offset;
	TH1D* h_target8_y_offset;
	TH1D* h_target9_x_offset;
	TH1D* h_target9_y_offset;
	string target_layer8_offset_x_filename;
	string target_layer8_offset_y_filename;
	string target_layer9_offset_x_filename;
	string target_layer9_offset_y_filename;
	bool is_choose_first = false;
	void target_layer_offset_calc();
	ofstream target_layer_hit_x_file[cMAX_DET_LAYER];
	ofstream target_layer_hit_y_file[cMAX_DET_LAYER];
	ofstream target_layer_amp_x_file[cMAX_DET_LAYER];
	ofstream target_layer_amp_y_file[cMAX_DET_LAYER];
	void write_current_target_layer_hit();
	void close_current_target_layer_hit();

	double offset_parameter[cMAX_DET_LAYER][6] = {0.0};
	json config_json;
	bool init_json(string filename);
	json event_json;
	void write_json_file(int);
	json status_json;
	void write_status_json_file();
	bool is_save_json = false;

	

};
