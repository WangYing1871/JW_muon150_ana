#ifndef NOISE_CALC_HH
#define NOISE_CALC_HH
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TH1I.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <iomanip>
#include <TStyle.h>

#include"constant.hh"
class noise_calc
{
public:
	noise_calc(string);
	~noise_calc();

	double mean[cCHIP_NUM][cCHN_NUM];
	double rms[cCHIP_NUM][cCHN_NUM];
	long total_cnts = 0;
	bool is_save_mean = true;
	bool is_save = false;
	bool asic_mask_idx[4] = { false };
	string save_path = "./";

	bool fill_fec_data(string fec_data_filename);
	bool fill_fec_data(char* fec_data_filename);
	void run_noise_calc();
	void show(int chip = 0, int chn = 0);
	void write(string out_filename);
	void write(char* out_filename);
	void write_flx_cmd(string,int sigma=3);
	void write_usb_cmd(string, int sigma = 3);
	void plot(int link_num);
	void plot(string link_num);
	void set_max_noise(double _max);
private:
	string file_title;
	

	TFile* out_tfile;
	TTree* noise_tree;
	TH1I* noise_data[cCHIP_NUM][cCHN_NUM];

	TFile* fec_data_file;
	TTree* fec_data_tree;
	int nhits = 0;
	int chip_id[cMAX_HITS];
	int chn_id[cMAX_HITS];
	int adc_data[cMAX_HITS][cSCA_NUM];
	TBranch* b_nhit;
	TBranch* b_chip_id;
	TBranch* b_chn_id;
	TBranch* b_adc_data;

	TCanvas* c1;
	TCanvas* c2;

	double max_noise_value = 4096;
};
#endif // !NOISE_CALC_HH
