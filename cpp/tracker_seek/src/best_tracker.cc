#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include "best_tracker.hh"

best_tracker::best_tracker(string data_infilename, string save_filename, int layer, double angle_x, double angle_y)
{
	cout << "Begin initial data in and out file" << endl;
	det_layer_used = layer;
	cout << "Layer used: " << det_layer_used << endl;
	is_file_get = init(data_infilename, save_filename) > 0;
}

best_tracker::~best_tracker()
{
	if (hit_data_file->IsOpen())
	{
		cout << "Hit data in root strore at " << cCYAN << data_file_name << cRESET << endl;
		hit_data_file->Close();
	}
	if (dataout_file->IsOpen())
	{
		dataout_file->cd();
		dataout_tree->Write();
		dataout_file->Close();
	}
	if (txt_tracker_file.is_open())
	{
		txt_tracker_file.close();
		cout << "Tracker data in txt strore at " << cCYAN << txt_filename_tracker << cRESET << endl;
		cout << "Format: trigger_id kx bx ky by" << endl;
	}
	if (txt_hit_file.is_open())
	{
		txt_hit_file.close();
		cout << "Tracker hit in txt strore at " << cCYAN << txt_filename_hit << cRESET << endl;
		cout << "Format: trigger_id x[0:4] y[0:4] z[0:4] (-1 stands for not hit!)" << endl;
	}
	if (txt_hit_aln_file.is_open())
	{
		txt_hit_file.close();
		cout << "Tracker hit in txt strore at " << cCYAN << txt_filename_hit_aln << cRESET << endl;
		cout << "Format: trigger_id x[0:4] y[0:4] z[0:4] (-1 stands for not hit!)" << endl;
	}
	if (txt_file4aln.is_open())
	{
		txt_file4aln.close();
		cout << "Tracker hit in txt strore at " << cCYAN << txt_filename4aln << cRESET << endl;
		cout << "Format: trigger_id x[0] y[0] x[1] [1]... (-1 stands for not hit!)" << endl;
	}
}

int best_tracker::init(string data_infilename, string save_filename)
{
	int data_number = init_datain(data_infilename);
	init_dataout(save_filename);
	return data_number;
}

int best_tracker::init_datain(string data_infilename)
{
	hit_data_file = new TFile(data_infilename.c_str());
	if (hit_data_file->IsZombie() || !hit_data_file->IsOpen())
	{
		cout << cRED << "[ERROR]: File" << data_infilename << " open failed. Please check the file" << cRESET << endl;
		return -1;
	}
	hit_data_tree = (TTree *)hit_data_file->Get("fec_hit_data");
	if (hit_data_tree == nullptr)
	{
		cout << cRED << "[ERROR]: Cannot get TTree* fec_hit_data in file " << data_infilename << cRESET << endl;
		return -1;
	}
	hit_data_tree->SetBranchAddress("trigger_id_all", &trigger_id, &b_triggera_id);
	hit_data_tree->SetBranchAddress("total_layer_num", &total_layer_num, &b_total_layer_num);
	for (int i = 0; i < det_layer_used; i++)
	{

		string dec_name = "dector" + to_string(i);
		hit_data_tree->SetBranchAddress(dec_name.c_str(), &detector_data[i].x_nhits, &b_detector_data[i]);
	}
	cout << cGREEN << "TTree in TFile " << data_infilename << " get!" << cRESET << endl;
	total_entried_num = hit_data_tree->GetEntries();
	if (total_entried_num != 0)
	{
		hit_data_tree->GetEntry(0);
	}
	return total_entried_num;
}

bool best_tracker::init_dataout(string filename)
{
	data_file_name = filename + ".root";
	dataout_file = new TFile(data_file_name.c_str(), "recreate");
	dataout_tree = new TTree("hit_tracker", "hit_tracker");
	dataout_tree->SetAutoSave();
	dataout_tree->Branch("trigger_id", &trigger_id, "trigger_id/I");
	dataout_tree->Branch("kx", &kx, "kx/D");
	dataout_tree->Branch("bx", &bx, "bx/D");
	dataout_tree->Branch("ky", &ky, "ky/D");
	dataout_tree->Branch("by", &by, "by/D");
	dataout_tree->Branch("rmse_x", &rmse_x, "rmse_x/D");
	dataout_tree->Branch("rmse_y", &rmse_y, "rmse_y/D");
	dataout_tree->Branch("r2_x", &r2_x, "r2_x/D");
	dataout_tree->Branch("r2_y", &r2_y, "r2_y/D");
	dataout_tree->Branch("det_layer_used", &det_layer_used, "det_layer_used/I");
	dataout_tree->Branch("is_x_hit", is_x_hit, "is_x_hit[det_layer_used]/I");
	dataout_tree->Branch("is_y_hit", is_y_hit, "is_y_hit[det_layer_used]/I");
	dataout_tree->Branch("x", x, "x[det_layer_used]/D");
	dataout_tree->Branch("y", y, "y[det_layer_used]/D");
	dataout_tree->Branch("z", z, "z[det_layer_used]/D");
	dataout_tree->Branch("x_amp", x_amp, "x_amp[det_layer_used]/D");
	dataout_tree->Branch("y_amp", y_amp, "y_amp[det_layer_used]/D");
	dataout_tree->Branch("x_strip_num", x_strip_num, "x_strip_num[det_layer_used]/I");
	dataout_tree->Branch("y_strip_num", y_strip_num, "y_strip_num[det_layer_used]/I");
	cout << cCYAN << "ROOT file: " << data_file_name << " created successfully" << cRESET << endl;
	txt_filename_tracker = filename + "_fit.txt";
	txt_tracker_file.open(txt_filename_tracker);
	txt_filename_hit = filename + "_hit.txt";
	txt_hit_file.open(txt_filename_hit);
	txt_filename_hit_aln = filename + "_hit_aligned.txt";
	txt_hit_aln_file.open(txt_filename_hit_aln);
	txt_filename4aln = filename + "_wait2aln.txt";
	txt_file4aln.open(txt_filename4aln);
	dout_filename_prefix = filename;
	return true;
}

void best_tracker::show_result()
{
	cout << "Total events number: " << total_entried_num << ", hitted events number: " << valid_event_cnts << endl;
}

void best_tracker::set_rmse_vth(double t)
{
	rms_vth = t;
}

void best_tracker::set_possible_strip_vth(int t)
{
	possible_strip_num = t;
}

void best_tracker::set_min_layer(int t)
{
	min_layer = t;
}

void best_tracker::set_layer_used(int layer_used_idx)
{
	// layer_used_idx is a hex number, each bit represent a layer
	for (int i = 0; i < cMAX_DET_LAYER; i++)
	{
		user_set_layer_used[i] = (layer_used_idx >> i) & 1;
	}
	cout << "Layer used: " << endl;
	for (int i = 0; i < cMAX_DET_LAYER; i++)
	{
		if (user_set_layer_used[i])
		{
			cout << "Layre " << i << " ";
			cout << cGREEN << "used" << endl;
			cout << cRESET;
		}
	}
	cout << "========" << endl;
}

void best_tracker::load_offset_param(string filename)
{
	ifstream offset_file;
	offset_file.open(filename);
	if (offset_file.is_open())
	{
		string offset_s;
		int line_cnt = 0;
		while (getline(offset_file, offset_s))
		{
			if (line_cnt >= cMAX_DET_LAYER)
			{
				cout << cRED << "Offset file content line number exceed the det_layer_used: " << det_layer_used << cRESET << endl;
			}
			istringstream offset_ss(offset_s);
			string s;
			int cnt = 0;
			while (offset_ss && cnt < 6)
			{
				if (!getline(offset_ss, s, ' '))
					break;
				offset_parameter[line_cnt][cnt] = stod(s);
				cnt++;
			}
			line_cnt++;
		}
	}
	offset_file.close();
}

void best_tracker::set_tomography_system(bool t)
{
	is_tomography_system = t;
}

void best_tracker::set_target_layer_calc(int layer, bool is_first)
{
	target_layer = layer;
	is_calc_target_offset = true;
	for (int i = 0; i < total_layer_num; i++)
	{
		if (user_set_layer_used[i])
		{
			continue;
		}
		string txt_filename = dout_filename_prefix + "_hit_layer_" + to_string(i) + "_x.txt";
		target_layer_hit_x_file[i].open(txt_filename);
		txt_filename = dout_filename_prefix + "_hit_layer_" + to_string(i) + "_y.txt";
		target_layer_hit_y_file[i].open(txt_filename);
		txt_filename = dout_filename_prefix + "_amp_layer_" + to_string(i) + "_x.txt";
		target_layer_amp_x_file[i].open(txt_filename);
		txt_filename = dout_filename_prefix + "_amp_layer_" + to_string(i) + "_y.txt";
		target_layer_amp_y_file[i].open(txt_filename);
		cout << cCYAN << "Layer " << i << " is set as target layer for data saving" << cRESET << endl;
	}
	is_choose_first = is_first;
	target_layer8_offset_x_filename = dout_filename_prefix + "_target_layer_" + to_string(8) + "_offset_x.png";
	target_layer8_offset_y_filename = dout_filename_prefix + "_target_layer_" + to_string(8) + "_offset_y.png";
	target_layer9_offset_x_filename = dout_filename_prefix + "_target_layer_" + to_string(9) + "_offset_x.png";
	target_layer9_offset_y_filename = dout_filename_prefix + "_target_layer_" + to_string(9) + "_offset_y.png";
	h_target8_x_offset = new TH1D("h_target_x8_offset", "h_target_x8_offset", 100, -50, 50);
	h_target8_y_offset = new TH1D("h_target_y8_offset", "h_target_y8_offset", 100, -50, 50);
	h_target9_x_offset = new TH1D("h_target_x9_offset", "h_target_x9_offset", 100, -50, 50);
	h_target9_y_offset = new TH1D("h_target_y9_offset", "h_target_y9_offset", 100, -50, 50);
	cout << cGREEN << "Calculate the offset of layer " << target_layer << ". Only choose first hit: " << (is_choose_first ? "True" : "False") << cRESET << endl;
}

bool best_tracker::run()
{
	if (!is_file_get)
	{
		cout << cRED << "Data in file initial failed!" << endl;
		return false;
	}
	int x_hit_num = 0;
	int y_hit_num = 0;
	int x_nhit[cMAX_DET_LAYER] = {0};
	int y_nhit[cMAX_DET_LAYER] = {0};
	valid_event_cnts = 0;

	array<vector<double>, cMAX_DET_LAYER> possible_hit_x;
	array<vector<int>, cMAX_DET_LAYER> possible_x_amp;
	array<vector<int>, cMAX_DET_LAYER> possible_x_strip_num;
	array<vector<double>, cMAX_DET_LAYER> possible_hit_y;
	array<vector<int>, cMAX_DET_LAYER> possible_y_amp;
	array<vector<int>, cMAX_DET_LAYER> possible_y_strip_num;

	bool layer_used_x[cMAX_DET_LAYER] = {false};
	bool layer_used_y[cMAX_DET_LAYER] = {false};
	// Generate a random number between 80to 10

	int one_percent = total_entried_num / rand() % 20 + 80;
	one_percent = (one_percent == 0) ? 1 : one_percent; // In case of total_entried_num < 100
	setbuf(stdout, NULL);
  info_out(total_entried_num);
	for (int hit_idx = 0; hit_idx < total_entried_num; hit_idx++)
	{
		x_hit_num = 0;
		y_hit_num = 0;
		int x_upper_hit_num = 0;
		int y_upper_hit_num = 0;
		int x_down_hit_num = 0;
		int y_down_hit_num = 0;
		for (int i = 0; i < cMAX_DET_LAYER; i++)
		{
			layer_used_x[i] = false;
			layer_used_y[i] = false;
		}
		hit_data_tree->GetEntry(hit_idx);
		for (int layer = 0; layer < cMAX_DET_LAYER; layer++)
		{
			if (!user_set_layer_used[layer])
			{
				continue;
			}
			if (detector_data[layer].x_nhits > 0)
			{
				x_hit_num++;
				layer_used_x[layer] = true;
				if (layer < 4)
				{
					x_upper_hit_num++;
				}
				else
				{
					x_down_hit_num++;
				}
			}
			if (detector_data[layer].y_nhits > 0)
			{
				y_hit_num++;
				layer_used_y[layer] = true;
				if (layer < 4)
				{
					y_upper_hit_num++;
				}
				else
				{
					y_down_hit_num++;
				}
			}
			x_nhit[layer] = detector_data[layer].x_nhits;
			y_nhit[layer] = detector_data[layer].y_nhits;
			//z[layer] = detector_data[layer].z + offset_parameter[layer][2]; // Unit mm
			z[layer] = detector_data[layer].z*10 + offset_parameter[layer][2]; // Unit mm
		}
    std::cout
      <<"trigger_id: "<<trigger_id
      <<" x_hit_num: "<<x_hit_num
      <<" y_hit_num: "<<y_hit_num
      <<" min_layer: "<<min_layer<<std::endl;
    //info_out(x_hit_num);
    //info_out(y_hit_num);
		// Remove unvalid
		if (x_hit_num < min_layer || y_hit_num < min_layer)
		{
			continue;
		}
		if (is_tomography_system && (x_upper_hit_num < 2 || x_down_hit_num < 2 || y_upper_hit_num < 2 || y_down_hit_num < 2))
		{
			continue;
		}

		for (int layer = 0; layer < cMAX_DET_LAYER; layer++)
		{
			if (!user_set_layer_used[layer])
			{
				continue;
			}
			// Initial a vector to store the possible hit of single layer
			vector<double> _x;
			vector<int> _xamp;
			vector<int> _xstrips;
			for (int i = 0; i < x_nhit[layer] && i < possible_strip_num; i++)
			{

				_x.push_back(detector_data[layer].x[i] * 0.4 + offset_parameter[layer][0]); // The pitch is 0.4mm
				_xamp.push_back(detector_data[layer].x_amp[i]);
				_xstrips.push_back(detector_data[layer].x_chn_num[i] - detector_data[layer].x_cluster_hole[i]);
			}
			possible_hit_x[layer] = _x;
			possible_x_amp[layer] = _xamp;
			possible_x_strip_num[layer] = _xstrips;

			vector<double> _y;
			vector<int> _yamp;
			vector<int> _ystrips;
			for (int i = 0; i < y_nhit[layer] && i < possible_strip_num; i++)
			{

				_y.push_back(detector_data[layer].y[i] * 0.4 + offset_parameter[layer][1]); // The pitch is 0.4mm
				_yamp.push_back(detector_data[layer].y_amp[i]);
				_ystrips.push_back(detector_data[layer].y_chn_num[i] - detector_data[layer].y_cluster_hole[i]);
			}
			possible_hit_y[layer] = _y;
			possible_y_amp[layer] = _yamp;
			possible_y_strip_num[layer] = _ystrips;
		}
		int best_idx_x[cMAX_DET_LAYER] = {0};
		int best_idx_y[cMAX_DET_LAYER] = {0};
		if (best_dec_fit(possible_hit_x, layer_used_x, kx, bx, best_idx_x, rmse_x, r2_x) 
        && best_dec_fit(possible_hit_y, layer_used_y, ky, by, best_idx_y, rmse_y, r2_y))
		{
			for (int i = 0; i < det_layer_used; i++)
			{
				if (layer_used_x[i])
				{
					x[i] = possible_hit_x[i][best_idx_x[i]] - offset_parameter[i][0];
					x_amp[i] = possible_x_amp[i][best_idx_x[i]];
					is_x_hit[i] = 1;
					x_strip_num[i] = possible_x_strip_num[i][best_idx_x[i]];
				}
				else
				{
					x[i] = -1;
					x_amp[i] = -1;
					is_x_hit[i] = 0;
					x_strip_num[i] = 0;
				}
				if (layer_used_y[i])
				{
					y[i] = possible_hit_y[i][best_idx_y[i]] - offset_parameter[i][1];
					y_amp[i] = possible_y_amp[i][best_idx_y[i]];
					is_y_hit[i] = 1;
					y_strip_num[i] = possible_y_strip_num[i][best_idx_y[i]];
				}
				else
				{
					y[i] = -1;
					y_amp[i] = -1;
					is_y_hit[i] = 0;
					y_strip_num[i] = 0;
				}
				z[i] = detector_data[i].z * 10;
			}

			dataout_tree->Fill();
			offset_aln();
			write_txt_file();
			//if (is_save_json)
			//{
			//	write_json_file(valid_event_cnts);
			//}
				write_json_file(valid_event_cnts);
			valid_event_cnts++;

			if (is_calc_target_offset)
			{
				target_layer_offset_calc();
				write_current_target_layer_hit();
			}
		}
	}
  write_status_json_file();
	//if (is_save_json)
	//{

	//	write_status_json_file();
	//}
	if (is_calc_target_offset)
	{
		close_current_target_layer_hit();
	}
	if (is_calc_target_offset)
	{
		TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
		h_target8_x_offset->DrawClone();
		TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
		h_target8_y_offset->DrawClone();
		c1->SaveAs(target_layer8_offset_x_filename.c_str());
		c2->SaveAs(target_layer8_offset_y_filename.c_str());
		TCanvas *c3 = new TCanvas("c3", "c3", 800, 600);
		h_target9_x_offset->DrawClone();
		TCanvas *c4 = new TCanvas("c4", "c4", 800, 600);
		h_target9_y_offset->DrawClone();
		c3->SaveAs(target_layer9_offset_x_filename.c_str());
		c4->SaveAs(target_layer9_offset_y_filename.c_str());
	}
	return true;
}

bool best_tracker::best_dec_fit(
    array<vector<double>, cMAX_DET_LAYER> possible_hit
    , bool *layer_used
    , double &best_k
    , double &best_b
    , int *best_idx
    , double &best_rmse
    , double &best_r2)
{
	vector<double> fit_x;
	vector<double> fit_y;
	int current_idx[cMAX_DET_LAYER] = {0};
	int total_loop = 1;
	for (int layer = 0; layer < det_layer_used; layer++)
	{
		if (!layer_used[layer])
		{
			continue;
		}
		total_loop *= possible_hit[layer].size();
	}
	int loop_cnt = 0;
	int loop_tmp;
	double min_rmse = 10000;
	double r2_rec = 0;
	while (loop_cnt < total_loop)
	{
		loop_tmp = loop_cnt;
		loop_cnt++;
		fit_x.clear();
		fit_y.clear();
		for (int layer = 0; layer < cMAX_DET_LAYER; layer++)
		{
			if (!layer_used[layer])
			{
				continue;
			}
			current_idx[layer] = loop_tmp % possible_hit[layer].size();
			loop_tmp = loop_tmp / possible_hit[layer].size();

			fit_x.push_back(z[layer]);
			fit_y.push_back(possible_hit[layer][current_idx[layer]]);
		}
		linear_fit my_fit(fit_x, fit_y);
		if (my_fit.run_best_fit(100, 0.9, min_layer, true, true) && min_rmse > my_fit.rmse)
		{
			min_rmse = my_fit.rmse;
			r2_rec = my_fit.r2;
			best_k = my_fit.slope;
			best_b = my_fit.intercept;
			for (int i = 0; i < det_layer_used; i++)
			{
				best_idx[i] = current_idx[i];
			}
		}
	}
	if (min_rmse > rms_vth)
	{
		return false;
	}

	best_rmse = min_rmse;
	best_r2 = r2_rec;
	return true;
}

bool best_tracker::least_square_fit(vector<double> x, vector<double> y, double &k, double &b, double &rmse, double &R2)
{
	if (x.size() == y.size() && x.size() >= 3)
	{
		double K = 0, B = 0, MSE = 0; // y = k*x+b
		double Mx = 0, My = 0;
		double Mxx = 0, Myy = 0, Mxy = 0;
		int n = x.size();
		for (int i = 0; i < n; i++)
		{
			Mx += x[i] / n;
			My += y[i] / n;
			Mxx += x[i] * x[i] / n;
			Myy += y[i] * y[i] / n;
			Mxy += x[i] * y[i] / n;
		}
		if (Mxx - Mx * Mx == 0)
		{
			return false;
		}
		K = (Mxy - Mx * My) / (Mxx - Mx * Mx);
		B = My - K * Mx;
		for (int i = 0; i < n; i++)
		{
			MSE += (y[i] - K * x[i] - B) * (y[i] - K * x[i] - B) / n;
		}
		k = K, b = B, rmse = sqrt(MSE);
		R2 = (Mxy - Mx * My) * (Mxy - Mx * My) / (Mxx - Mx * Mx) / (Myy - My * My); // correlation index R2
		return true;
	}
	else if (x.size() == y.size() && x.size() == 2)
	{
		k = (y[1] - y[0]) / (x[1] - x[0]);
		b = y[0] - k * x[0];
		rmse = 0;
		R2 = 1;
		return true;
	}
	else if (x.size() != y.size())
	{
		cout << "Error: vectors is not the same size!" << endl;
		return false;
	}
	else
	{
		// cout << "Error: vectors' size is too small!" << endl;
		return false;
	}
}

bool best_tracker::least_square_fit(double *x, double *y, int n, double &k, double &b, double &rmse, double &R2)
{
	if (n >= 3)
	{
		double K = 0, B = 0, MSE = 0; // y = k*x+b
		double Mx = 0, My = 0;
		double Mxx = 0, Myy = 0, Mxy = 0;
		for (int i = 0; i < n; i++)
		{
			Mx += x[i] / n;
			My += y[i] / n;
			Mxx += x[i] * x[i] / n;
			Myy += y[i] * y[i] / n;
			Mxy += x[i] * y[i] / n;
		}
		if (Mxx - Mx * Mx == 0)
		{
			return false;
		}
		K = (Mxy - Mx * My) / (Mxx - Mx * Mx);
		B = My - K * Mx;
		for (int i = 0; i < n; i++)
		{
			MSE += (y[i] - K * x[i] - B) * (y[i] - K * x[i] - B) / n;
		}
		k = K, b = B, rmse = sqrt(MSE);
		R2 = (Mxy - Mx * My) * (Mxy - Mx * My) / (Mxx - Mx * Mx) / (Myy - My * My); // correlation index R2
		return true;
	}
	else if (n == 2)
	{
		k = (y[1] - y[0]) / (x[1] - x[0]);
		b = y[0] - k * x[0];
		rmse = 0;
		R2 = 1;
		return true;
	}
	else
	{
		cout << cRED << "Error: vectors' size is too small!" << cRESET << endl;
		return false;
	}
}

void best_tracker::offset_aln()
{
	for (int i = 0; i < det_layer_used; i++)
	{
		if (x[i] != -1)
		{
			x_aln[i] = x[i] + offset_parameter[i][0];
			if (y[i] != -1)
			{
				x_aln[i] -= y[i] * offset_parameter[i][4];
			}
		}
		else
		{
			x_aln[i] = -1;
		}
		if (y[i] != -1)
		{
			y_aln[i] = y[i] + offset_parameter[i][1];
			if (x[i] != -1)
			{
				y_aln[i] += x[i] * offset_parameter[i][5];
			}
		}
		else
		{
			y_aln[i] = -1;
		}
		z_aln[i] = z[i] + offset_parameter[i][2];
		/*if (x[i] != -1)
		{
			z_aln[i] -= x[i] * offset_parameter[i][4];
		}
		if (y[i] != -1)
		{
			z_aln[i] += y[i] * offset_parameter[i][3];
		}*/
	}
}

void best_tracker::write_txt_file()
{
	txt_tracker_file << trigger_id << " " << kx << " " << bx << " " << ky << " " << by << " " << rmse_x << " " << rmse_y << endl;
	txt_hit_file << trigger_id << " ";
	txt_hit_aln_file << trigger_id << " ";
	for (int i = 0; i < det_layer_used; i++)
	{
		txt_hit_file << x[i] << " ";
		txt_hit_aln_file << x_aln[i] << " ";
	}
	for (int i = 0; i < det_layer_used; i++)
	{
		txt_hit_file << y[i] << " ";
		txt_hit_aln_file << y_aln[i] << " ";
	}
	for (int i = 0; i < det_layer_used; i++)
	{
		txt_hit_file << z[i];
		txt_hit_aln_file << z_aln[i] << " ";
		if (i != det_layer_used - 1)
		{
			txt_hit_file << " ";
			txt_hit_aln_file << " ";
		}
		else
		{
			txt_hit_file << endl;
			txt_hit_aln_file << endl;
		}
	}
	txt_file4aln << trigger_id << " ";
	for (int i = 0; i < det_layer_used; i++)
	{
		if (!user_set_layer_used[i])
		{
			if (i == det_layer_used - 1)
			{
				txt_file4aln << endl;
			}
			continue;
		}
		txt_file4aln << x_aln[i] << " " << y_aln[i] << " " << z_aln[i];
		if (i != det_layer_used - 1)
		{
			txt_file4aln << " ";
		}
		else
		{
			txt_file4aln << endl;
		}
	}
}

bool best_tracker::hough_trans(array<vector<double>, cMAX_DET_LAYER> possible_hit, bool *layer_used, double &k, double &b)
{
	hough_map = new TH2D("hough_map", "hough_map", 401, c_theta_min, c_theta_max, nbins, c_r_min, c_r_max);
	double theta;
	double r;
	double step = cPI / (double)nbins;
	for (int layer = 0; layer < det_layer_used; layer++)
	{
		if (!layer_used[layer])
		{
			continue;
		}
		for (int j = 0; j < possible_hit[layer].size(); j++)
		{
			for (int k = 0; k < nbins; k++)
			{
				if (k > nbins / 4 && k < nbins * 3 / 4)
				{
					continue;
				}
				theta = c_theta_min + k * step;
				r = z[layer] * cos(theta) + possible_hit[layer][j] * sin(theta) * 0.4;
				hough_map->Fill(theta, r);
			}
		}
	}
	int theta_max;
	int r_max;
	int z_max;
	int max_cnt = hough_map->GetBinContent(hough_map->GetMaximumBin(theta_max, r_max, z_max));
	if (max_cnt < 3)
	{
		delete hough_map;
		return false;
	}
	cout << "Max count " << max_cnt;
	theta = c_theta_min + theta_max * step;
	r = c_r_min + r_max * (c_r_max - c_r_min) / nbins;
	k = -1 / tan(theta);
	b = r / (sin(theta));
	cout << ". Located at (" << theta << ", " << r << "). k = " << k << ", b = " << b << endl;
	delete hough_map;
	return true;
}

void best_tracker::hough_trans_back(array<vector<double>, cMAX_DET_LAYER> possible_hit, bool *layer_used, double k, double b, double *x_h)
{
	double ideal_hit[cMAX_DET_LAYER] = {0};
	for (int i = 0; i < det_layer_used; i++)
	{
		if (!layer_used[i])
		{
			continue;
		}

		ideal_hit[i] = k * z[i] + b;
		x_h[i] = possible_hit[i][0];
		double diff_min = abs(ideal_hit[i] - possible_hit[i][0]);
		for (int j = 1; j < possible_hit[i].size(); j++)
		{
			if (abs(ideal_hit[i] - possible_hit[i][j]) < diff_min)
			{
				diff_min = abs(ideal_hit[i] - possible_hit[i][j]);
				x_h[i] = possible_hit[i][j];
			}
		}
	}
}

void best_tracker::target_layer_offset_calc()
{
	if (target_layer >= total_layer_num)
	{
		cout << cRED << "Target layer exceed the total layer number!" << cRESET << endl;
		return;
	}
	target_layer = 0;
	double target_x_fit = kx * detector_data[target_layer].z * 10 + bx;
	double target_y_fit = ky * detector_data[target_layer].z * 10 + by;
	double target_x = 0;
	double target_y = 0;
	for (int i = 0; i < detector_data[target_layer].x_nhits; i++)
	{
		target_x = detector_data[target_layer].x[i] * 0.4;
		h_target8_x_offset->Fill(target_x - target_x_fit);
		if (is_choose_first)
		{
			break;
		}
	}
	for (int i = 0; i < detector_data[target_layer].y_nhits; i++)
	{
		target_y = detector_data[target_layer].y[i] * 0.4;
		h_target8_y_offset->Fill(target_y - target_y_fit);
		if (is_choose_first)
		{
			break;
		}
	}
	target_layer = 1;
	target_x_fit = kx * detector_data[target_layer].z * 10 + bx;
	target_y_fit = ky * detector_data[target_layer].z * 10 + by;
	target_x = 0;
	target_y = 0;
	for (int i = 0; i < detector_data[target_layer].x_nhits; i++)
	{
		target_x = detector_data[target_layer].x[i] * 0.4;
		h_target9_x_offset->Fill(target_x - target_x_fit);
		if (is_choose_first)
		{
			break;
		}
	}
	for (int i = 0; i < detector_data[target_layer].y_nhits; i++)
	{
		target_y = detector_data[target_layer].y[i] * 0.4;
		h_target9_y_offset->Fill(target_y - target_y_fit);
		if (is_choose_first)
		{
			break;
		}
	}
}

void best_tracker::write_current_target_layer_hit()
{
	for (int i = 0; i < total_layer_num; i++)
	{
		if (user_set_layer_used[i])
		{
			continue;
		}
		target_layer_hit_x_file[i] << trigger_id << " ";
		target_layer_hit_x_file[i] << detector_data[i].z;
		target_layer_amp_x_file[i] << trigger_id;
		for (int j = 0; j < detector_data[i].x_nhits; j++)
		{
			target_layer_hit_x_file[i] << " ";
			target_layer_hit_x_file[i] << detector_data[i].x[j] * 0.4;
			target_layer_amp_x_file[i] << " ";
			target_layer_amp_x_file[i] << detector_data[i].x_amp[j];
			if (j != detector_data[i].x_nhits - 1)
			{
				target_layer_hit_x_file[i] << " ";
				target_layer_amp_x_file[i] << " ";
			}
		}
		target_layer_hit_x_file[i] << endl;
		target_layer_amp_x_file[i] << endl;

		target_layer_hit_y_file[i] << trigger_id << " ";
		target_layer_hit_y_file[i] << detector_data[i].z;
		target_layer_amp_y_file[i] << trigger_id;
		for (int j = 0; j < detector_data[i].y_nhits; j++)
		{
			target_layer_hit_y_file[i] << " ";
			target_layer_hit_y_file[i] << detector_data[i].y[j] * 0.4;
			target_layer_amp_y_file[i] << " ";
			target_layer_amp_y_file[i] << detector_data[i].y_amp[j];
			if (j != detector_data[i].y_nhits - 1)
			{
				target_layer_hit_y_file[i] << " ";
				target_layer_amp_y_file[i] << " ";
			}
		}
		target_layer_hit_y_file[i] << endl;
		target_layer_amp_y_file[i] << endl;
	}
}

void best_tracker::close_current_target_layer_hit()
{
	for (int i = 0; i < total_layer_num; i++)
	{
		if (user_set_layer_used[i])
		{
			continue;
		}
		target_layer_hit_x_file[i].close();
		target_layer_hit_y_file[i].close();
		target_layer_amp_x_file[i].close();
		target_layer_amp_y_file[i].close();
		cout << cBLUE << "Layer " << i << " used as target layer and txt are saved done" << endl;
		cout << "Format: trigger_id z x1 x2 x3 ... xN" << endl;
		cout << "Format: trigger_id z y1 y2 y3 ... yN" << endl;
		cout << "Format: trigger_id z x1_amp x2_amp x3_amp ... xN_amp" << endl;
		cout << "Format: trigger_id z y1_amp y2_amp y3_amp ... yN_amp" << cRESET << endl;
	}
}

bool best_tracker::init_json(string filename)
{
	config_json = json::parse(filename);
	return true;
}

void best_tracker::write_json_file(int outevt)
{
	event_json.clear();
	event_json["type"] = "event";
	event_json["id"] = 35;
	event_json["event_id"] = trigger_id;
	event_json["tracks"][0]["start_point"] = {
		kx * -1000 + bx, ky * -1000 + by, -1000};
	TVector3 vec(kx, ky, 1);
	vec.SetMag(1);
	event_json["tracks"][0]["direction"] = {
		vec.x(), vec.y(), vec.z()};
	event_json["tracks"][0]["angle"] = {
		round(vec.Theta() / 3.1415 * 180 * 100) / 100, round(vec.Phi() / 3.1415 * 180 * 100) / 100};
	for (int i = 0; i < det_layer_used; i++)
	{
		event_json["hits"][i] = {
			kx * z[i] + bx, ky * z[i] + by};
		for (int j = 0; j < x_strip_num[i]; j++)
		{
			event_json["x_cells"][i][j] = int(x[i] / 0.4) + j;
		}
		for (int j = 0; j < y_strip_num[i]; j++)
		{
			event_json["y_cells"][i][j] = int(y[i] / 0.4) + j;
		}
	}
	string js_filename = save_file_path + "event/" + "event_" + to_string(outevt) + ".json";
	ofstream js_file(js_filename);
	js_file << event_json << endl;
	js_file.close();
}

void best_tracker::write_status_json_file()
{
	status_json.clear();
	status_json["type"] = "status";
	status_json["id"] = 35;
	status_json["daq"] = "on";
	status_json["event_rate"] = (double)valid_event_cnts / acq_time;
	for (int i = 0; i < det_layer_used; i++)
	{
		status_json["hv_supply"][2 * i]["name"] = "hv" + to_string(i + 1) + "_cathode";
		status_json["hv_supply"][2 * i]["voltage"] = 720;
		status_json["hv_supply"][2 * i]["current"] = 0.001;
		status_json["hv_supply"][2 * i + 1]["name"] = "hv" + to_string(i + 1) + "_mesh";
		status_json["hv_supply"][2 * i + 1]["voltage"] = 560;
		status_json["hv_supply"][2 * i + 1]["current"] = 0.001;
	}
	string js_filename = save_file_path + "status/" + "status_0.json";
	cout << js_filename << endl;
	ofstream js_file(js_filename);
	js_file << status_json << endl;
	js_file.close();
}
