#include "noise_calc.hh"

using namespace std;
noise_calc::noise_calc(string title)
{
	string th1_title;
	file_title = title;
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		for (int j = 0; j < cCHN_NUM; j++)
		{
			th1_title = title + "_CHIP" + to_string(i) + "_" + "Chn" + to_string(j);
			noise_data[i][j] = new TH1I(th1_title.c_str(), title.c_str(), 1000, 0, 1000);
		}
	}
}

noise_calc::~noise_calc()
{

	//delete c1;
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		for (int j = 0; j < cCHN_NUM; j++)
		{
			delete noise_data[i][j];
		}
	}
}

bool noise_calc::fill_fec_data(string fec_data_filename)
{
	char* filename_c = new char[fec_data_filename.length() + 1];
	strcpy(filename_c, fec_data_filename.c_str());
	return fill_fec_data(filename_c);
}

bool noise_calc::fill_fec_data(char* fec_data_filename)
{
	fec_data_file = new TFile(fec_data_filename);
	if (fec_data_file->IsZombie() || !fec_data_file->IsOpen())
	{
		cout << cRED << fec_data_filename << " does not exist!!!" << endl << cRESET;
		delete fec_data_file;
		return false;
	}
	fec_data_tree = (TTree*)fec_data_file->Get("fec_origin_data");
	if (fec_data_tree == nullptr)
	{
		cout << cRED << "Can not get fec_origin_data from " << fec_data_filename << endl << cRESET;
		delete fec_data_file;
		return false;
	}
	long entry_num = fec_data_tree->GetEntries();
	total_cnts += entry_num;
	cout << "Get the TTree fec_origin_data from " << fec_data_filename
		<< " Entry number: " << entry_num << endl;
	fec_data_tree->SetBranchAddress("nhits", &nhits, &b_nhit);
	fec_data_tree->SetBranchAddress("chip_id", chip_id, &b_chip_id);
	fec_data_tree->SetBranchAddress("chn_id", chn_id, &b_chn_id);
	fec_data_tree->SetBranchAddress("adc_data", adc_data, &b_adc_data);
	cout << "Set branch done" << endl;
	int chn;
	int chip;
	for (long i = 0; i < entry_num; i++)
	{
		fec_data_tree->GetEntry(i);
		for (int j = 0; j < nhits && j < cMAX_HITS; j++)
		{
			chn = chn_id[j];
			chip = chip_id[j];
			if (chn > 67 || chn < 0 || chip > 3 || chip < 0)
			{
				cout << "Chip: " << chip
					<< " Channel: " << chn
					<< " error!!!" << endl;
				continue;
			}
			for (int k = 0; k < cSCA_NUM; k++)
			{
				noise_data[chip][chn]->Fill(adc_data[j][k]);
			}
		}
	}
	//cout << "File :\"" << fec_data_filename << "\" get done. " << endl;
	delete fec_data_file;
	//delete fec_data_tree;
	return true;
}

void noise_calc::run_noise_calc()
{
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		for (int j = 0; j < cCHN_NUM; j++)
		{
			// Define the Gaussian fit function
			TF1* fitFunc = new TF1("fitFunc", "gaus");

			// Fit the histogram with the gaussian function
			noise_data[i][j]->Fit("fitFunc", "Q");

			mean[i][j] = fitFunc->GetParameter(1);
			rms[i][j] = fitFunc->GetParameter(2);
		}
	}
}

void noise_calc::show(int chip, int chn)
{
	if (chn > 67 || chn < 0 || chip > 3 || chip < 0)
	{
		cout << "Chip: " << chip
			<< " Channel: " << chn
			<< " error!!!" << endl;
	}
	cout << "Chip: " << chip << " Chn: " << chn
		<< " Mean: " << mean[chip][chn] << " StdDev: " << rms[chip][chn] << endl;
}

void noise_calc::write(string out_filename)
{
	char* filename_c = new char[out_filename.length() + 1];
	strcpy(filename_c, out_filename.c_str());
	write(filename_c);
}

void noise_calc::write(char* out_filename)
{
	out_tfile = new TFile(out_filename, "RECREATE");
	noise_tree = new TTree("noise_tree", "FEC noise");
	noise_tree->Branch("total_cnts", &total_cnts, "total_cnts/L");
	noise_tree->Branch("mean", mean, "mean[4][68]/D");
	noise_tree->Branch("rms", rms, "rms[4][68]/D");
	noise_tree->Fill();

	//noise_tree->Write();
	out_tfile->Flush();
	out_tfile->Write();
	out_tfile->Close();
	string txt_file_name = string(out_filename);
	txt_file_name.replace(txt_file_name.rfind(".root"), 5, ".txt");
	ofstream of_txt;
	of_txt.open(txt_file_name);
	of_txt.precision(5);
	//of_txt << "// Chip0~3 mean value (channel0~68)" << endl;
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		for (int j = 0; j < cCHN_NUM; j++)
		{
			of_txt << mean[i][j];
			if (j != cCHN_NUM - 1)
			{
				of_txt << " ";
			}
		}
		of_txt << endl;

	}
	//of_txt << "----------" << endl;
	//of_txt << "// Chip0~3 std dev value (channel0~68)" << endl;
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		for (int j = 0; j < cCHN_NUM; j++)
		{
			of_txt << rms[i][j];
			if (j != cCHN_NUM - 1)
			{
				of_txt << " ";
			}
		}
		if (i != cCHIP_NUM - 1)
		{
			of_txt << endl;
		}

	}
	cout << txt_file_name << " Write done!" << endl;
	//delete noise_tree;
	delete out_tfile;

}

void noise_calc::write_flx_cmd(string file_name, int sigma)
{
	ofstream cmd_file;
	cmd_file.open(file_name);
	if (cmd_file.is_open())
	{
		int vth_data;
		cmd_file << hex;
		int chip_vth_value;
		int chip_vth_cmd1;
		int chip_vth_cmd2;
		double single_rms;
		for (int chip = 0; chip < cCHIP_NUM; chip++)
		{
			for (int chn = 0; chn < cCHN_NUM; chn++)
			{
				single_rms = (rms[chip][chn] < max_noise_value) ? rms[chip][chn] : max_noise_value;
				vth_data = asic_mask_idx[chip] ? 0x3FF : ceil(mean[chip][chn] + sigma * single_rms);
				cmd_file << "0x" << cAGET_VTH_CMD_HEADER << " ";
				cmd_file << setfill('0') << setw(2) << "0x" << chn << " ";
				chip_vth_value = (chip << 14) + vth_data;
				chip_vth_cmd1 = (chip_vth_value >> 8) & 0xFF;
				chip_vth_cmd2 = chip_vth_value & 0xFF;
				cmd_file << "0x" << chip_vth_cmd1 << " 0x" << chip_vth_cmd2 << endl;
				cmd_file << "&100" << endl;
			}
		}
		cmd_file.close();
		cout << "Write \"" << file_name << "\" done!" << endl;
	}
	else
	{
		cout << "Write " << file_name << " fail!!!" << endl;
	}
}

void noise_calc::write_usb_cmd(string file_name, int sigma)
{
	ofstream cmd_file;
	cmd_file.open(file_name);
	if (cmd_file.is_open())
	{
		int vth_data;
		cmd_file << hex;
		int chip_vth_value;
		int chip_vth_cmd1;
		int chip_vth_cmd2;
		double single_rms;
		for (int chip = 0; chip < cCHIP_NUM; chip++)
		{
			for (int chn = 0; chn < cCHN_NUM; chn++)
			{
				single_rms = (rms[chip][chn] < max_noise_value) ? rms[chip][chn] : max_noise_value;
				vth_data = asic_mask_idx[chip] ? 0x3FF : ceil(mean[chip][chn] + sigma * single_rms);

				// Set the vth for noise channel to zero for reading back the data
				if (chn == 11 || chn == 22 || chn == 45 || chn == 56)
				{
					vth_data = 0;
				}
				string cmd;
				cmd_file << "0x0101" << cAGET_VTH_CMD_HEADER;
				cmd_file << setfill('0') << setw(2) << chn << endl;
				cmd_file << "0x01020001" << endl;
				chip_vth_value = (chip << 14) + vth_data;
				chip_vth_cmd1 = (chip_vth_value >> 8) & 0xFF;
				chip_vth_cmd2 = chip_vth_value & 0xFF;
				cmd_file << "0x0101" << setfill('0') << setw(2) << chip_vth_cmd1 << setfill('0') << setw(2) << chip_vth_cmd2 << endl;
				cmd_file << "0x01020001" << endl;
				cmd_file << "&1" << endl;
				cmd_file << endl;
			}
		}
		cmd_file.close();
		cout << cCYAN << "Write \"" << file_name << "\" done!" << cRESET << endl;
	}
	else
	{
		cout << cRED "Write " << file_name << " fail!!!" << cRESET << endl;
	}
}

void noise_calc::plot(int link_num)
{
	string name = "RMS Link" + to_string(link_num);
	plot(name);

}

void noise_calc::plot(string link_num)
{
	string name = link_num;
	gStyle->SetImageScaling(3.);
	c1 = new TCanvas(name.c_str(), name.c_str());
	c1->Divide(2, 2);
	double x[cCHN_NUM];
	double ex[cCHN_NUM];
	for (int i = 0; i < cCHN_NUM; i++)
	{
		x[i] = i;
		ex[i] = 0;
	}
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		c1->cd(i + 1);
		gPad->SetGrid();
		auto gr = new TGraphErrors(cCHN_NUM, x, mean[i], ex, rms[i]);
		string title = "Chip" + to_string(i);
		gr->SetTitle(title.c_str());
		gr->SetMarkerColor(4);
		gr->SetMarkerStyle(21);
		gr->DrawClone("ALP");

	}
	if (is_save_mean)
	{
		string save_filename = save_path + "mean_" + name + ".png";
		c1->SaveAs(save_filename.c_str());
	}
	name = "rms_" + name;
	c2 = new TCanvas(name.c_str(), name.c_str());
	c2->Divide(2, 2);
	for (int i = 0; i < cCHIP_NUM; i++)
	{
		c2->cd(i + 1);
		gPad->SetGrid();
		auto gr = new TGraph(cCHN_NUM, x, rms[i]);
		string title = "Chip" + to_string(i);
		gr->SetTitle(title.c_str());
		gr->SetMarkerColor(4);
		gr->SetMarkerStyle(21);
		gr->DrawClone("ALP");

	}
	if (is_save)
	{
		string save_filename = save_path + name + ".png";
		c2->SaveAs(save_filename.c_str());
	}
}

void noise_calc::set_max_noise(double _max)
{
	max_noise_value = _max;
}

