#include "hit_reconstruct.hh"

hit_reconstruct::hit_reconstruct(string filename2save
    , int link_used
    , int set_layer
    , string position_filename
    , string z_filename
    , bool is_wr_txt)
{
	is_write_txt_file = is_wr_txt;
	user_set_layer_number = set_layer;
	total_layer_num = set_layer;
	if (user_set_layer_number > cMAX_DET_LAYER)
	{
		cout << cRED << "Layer number is too large. Please check the layer number." << cRESET << endl;
		return;
	}
	if (!load_position(position_filename))
	{
		return;
	}
	if (!load_z(z_filename))
	{
		return;
	}

	for (int i = 0; i < cELINK_NUM; i++)
	{
		if (((link_used >> i) & 1) == 1)
		{
			link_enabled[i] = true;
		}
		else
		{
			link_enabled[i] = false;
		}
	}
	if (!init_dataout(filename2save))
	{
		cout << cRED << "Data are not processing" << cRESET << endl;
		return;
	}

	is_init_successed = true;
}

hit_reconstruct::~hit_reconstruct()
{
	/*if (hit_tfile->IsOpen())
	{
		hit_tfile->Close();
	}
	for (int i = 0; i < cELINK_NUM; i++)
	{
		if (link_enabled[i] && fec_data_file[i]->IsOpen())
		{
			fec_data_file[i]->Close();
		}
	}*/
}

bool hit_reconstruct::init_dataout(string filename)
{
	string root_filename = filename + ".root";
	hit_tfile = new TFile(root_filename.c_str(), "recreate");
	if (hit_tfile->IsZombie() || !hit_tfile->IsOpen())
	{
		cout << cRED << "[ERROR]: Cannot create file " << root_filename << cRESET << endl;
		return false;
	}
	hit_ttree = new TTree("fec_hit_data", "FEC HIT");
	hit_ttree->Branch("trigger_id_all", &trigger_id, "trigger_id/I");
	hit_ttree->Branch("total_layer_num", &total_layer_num, "total_layer_num/I");
	for (int i = 0; i < user_set_layer_number; i++)
	{
		string dec_name = "dector" + to_string(i);
		hit_ttree->Branch(dec_name.c_str(), &detector_data[i].x_nhits, "x_nhits/I:y_nhits/I:x[25]/D:y[25]/D:z/D:x_amp[25]/D:y_amp[25]/D:x_chn_num[25]/I:y_chn_num[25]/I:x_cluster_hole[25]/I:y_cluster_hole[25]/I");
	}
	cout << "TTree in TFile \"" << cCYAN << root_filename << cRESET << "\" init done" << endl;

	if (is_write_txt_file)
	{
		string txt_filename = filename + ".txt";
		txt_fileout.open(txt_filename);
		if (!txt_fileout.is_open())
		{
			cout << cRED << "[ERROR]: Cannot create file " << cRESET << txt_filename << endl;
			return false;
		}
		cout << "TXT file " << cCYAN << txt_filename << cRESET << " created." << endl;
		txt_filename = filename + "max_hit.txt";
		max_position_file.open(txt_filename);
		if (!max_position_file.is_open())
		{
			cout << cRED << "[ERROR]: Cannot create file " << cRESET << txt_filename << endl;
			return false;
		}
		cout << "TXT file " << cCYAN << txt_filename << cRESET << " created." << endl;
	}
	return true;
}

bool hit_reconstruct::load_position(string filename)
{
	ifstream position_file;
	position_file.open(filename);
	if (!position_file.is_open())
	{
		cout << cRED << "[ERROR]: Cannot open file " << filename << cRESET << endl;
		return false;
	}
	string line;
	string s;
	int k = 0;
	// Read the title
	getline(position_file, line);
	while (getline(position_file, line))
	{
		istringstream ss(line);
		// �ȶ�ǰ�������ݣ����Ҷ�������Ϊû��ʲô��
		if (!getline(ss, s, ','))
		{
			cout << cYELLOW << "===========" << endl;
			cout << cRED << "[ERROR]: Position file " << filename << " content error!" << cRESET << endl;
			return false;
		}
		if (!getline(ss, s, ','))
		{
			cout << cYELLOW << "===========" << endl;
			cout << cRED << "[ERROR]: Position file " << filename << " content error!" << cRESET << endl;
			return false;
		}

		if (!getline(ss, s, ','))
		{
			cout << cYELLOW << "===========" << endl;
			cout << cRED << "[ERROR]: Position file " << filename << " content error!" << cRESET << endl;
			return false;
		}
		if (isConvertibleToInt(s))
		{
      info_out(k);
			layer_idx[k][0] = stoi(s);
		}
		else
		{
			cout << cYELLOW << "===========" << endl;
			cout << cRED << "[ERROR]: Position file " << filename << " content error! at line " << k << " with content " << s << cRESET << endl;
			return false;
		}
		if (!getline(ss, s, ','))
		{
			cout << cYELLOW << "===========" << endl;
			cout << cRED << "[ERROR]: Position file " << filename << " content error!" << cRESET << endl;
			return false;
		}
		if (isConvertibleToInt(s))
		{
			layer_idx[k][1] = stoi(s);
			if (layer_num < layer_idx[k][0])
			{
				layer_num = layer_idx[k][0];
			}
		}
		else
		{
			cout << cYELLOW << "===========" << endl;
			cout << cRED << "[ERROR]: Position file " << filename << " content error! at line " << k << " with content " << s << cRESET << endl;
			return false;
		}
		if (getline(ss, s, ','))
		{
			cout << cYELLOW << "[Warning]: Position file " << filename << " has more element at line " << k << ". The redundant element is ignored." << cRESET << endl;
		}
		k++;
	}
	position_file.close();
	if (layer_num == 0)
	{
		cout << cYELLOW << "===========" << endl;
		cout << cRED << "[ERROR]: Position file " << filename << " has no layer." << cRESET << endl;
	}
	else
	{
		layer_num++;
		cout << "Layer number: " << layer_num << endl;
	}
	if (user_set_layer_number != layer_num)
	{
		cout << cYELLOW << "===========" << endl;
		cout << cRED << "[ERROR]: User set layer number is " << user_set_layer_number << ", but the position file has " << layer_num << " layers." << cRESET << endl;
		return false;
	}


  //info_out("layer_num");
  //for (auto&& x :layer_idx){
  //  for (auto&& y : x) std::cout<<y<<" ";
  //  std::cout<<std::endl;
  //}
  
    
	return true;
}

bool hit_reconstruct::load_z(string filename)
{
	ifstream z_file;
	z_file.open(filename);
	if (!z_file.is_open())
	{
		cout << cRED << "[ERROR]: Cannot open Z position file " << filename << cRESET << endl;
		return false;
	}
	string line;
	getline(z_file, line);
	// Check if line is empty or space
	// string check if line is empty
	string line_remove_space(line.begin(), remove_if(line.begin(), line.end(), ::isspace));
	if (line.empty() || line_remove_space.empty())
	{
		cout << cRED "[ERROR]: Z position file " << filename << " is empty." << cRESET << endl;
		return false;
	}
	istringstream ss(line);
	string s;
	int k = 0;
	while (ss)
	{
		if (!getline(ss, s, ',') || s == "")
			break;
		z_position[k] = stof(s);
		k++;
	}
	cout << "Z position: ";
	for (int i = 0; i < k; i++)
	{
		cout << z_position[i] << " cm ";
	}
	cout << endl;
	if (k != layer_num)
	{
		cout << cRED << "[ERROR]: Z position file " << filename << " has " << k << " layers, but the position file has " << layer_num << " layers." << cRESET << endl;
		return false;
	}
	cout << endl;
	z_file.close();
	return true;
}

bool hit_reconstruct::fill_data(string data_filename)
{
	int data_len = init_datain(data_filename);
	if (data_len == 0)
	{
		return false;
	}
	is_datain_get = true;
	int one_percent = data_len / 100;
	one_percent = (one_percent == 0) ? 1 : one_percent; // In case data-len < 100
	setbuf(stdout, NULL);
	int trigger_id_start = INT_MAX;
	int trigger_id_end = 0;
	for (int j = 0; j < cELINK_NUM; j++)
	{
		if (!link_enabled[j])
		{
			continue;
		}
		fec_data_tree[j]->GetEntry(0);
		if (trigger_id_all[j] < trigger_id_start)
		{
			trigger_id_start = trigger_id_all[j];
		}
		fec_data_tree[j]->GetEntry(data_len - 1);
		if (trigger_id_all[j] > trigger_id_end)
		{
			trigger_id_end = trigger_id_all[j];
		}
	}

	cout << "Set trigger start to " << trigger_id_start << " and set trigger end to " << trigger_id_end << endl;
	cout << "Total event number is " << data_len << endl;
	int rd_idx[cELINK_NUM] = {0}; // This parameter is used to locate the Entry index of each tree
	bool is_idx_beyond = false;
	bool trigger_aligned[cELINK_NUM] = {false};
	bool is_trigger_aligned = false;
	for (int i = trigger_id_start; i < trigger_id_end && !is_idx_beyond; i++) {
		is_trigger_aligned = false;
		if (i % one_percent == 0 && i != 0)
		{
			cout << cCLR_LINE;
			cout << "Processing "
				 << "\t" << double(i - trigger_id_start) / (trigger_id_end - trigger_id_start) * 100 << "% ";
		}

		for (int j = 0; j < cMAX_DET_LAYER; j++)
		{
			detector_data[j] = position_data();
		}

		for (int j = 0; j < cELINK_NUM; j++)
		{
			if (!link_enabled[j])
			{
				continue;
			}
			trigger_aligned[j] = false;

			fec_data_tree[j]->GetEntry(rd_idx[j]);
      //for (auto&& x : trigger_id_all) info_out(x);
      //info_out(i);
			while (trigger_id_all[j] < i && !is_idx_beyond)
			{
				if (rd_idx[j] >= data_len)
				{
					is_idx_beyond = true;
				}
				rd_idx[j]++;
				fec_data_tree[j]->GetEntry(rd_idx[j]);
				cout << cYELLOW << "[Warning]: Trigger id " << i << "is not aligned at link " << j << cRESET << endl;
			}
			if (trigger_id_all[j] == i)
			{
				rd_idx[j]++;
				trigger_aligned[j] = true;
				is_trigger_aligned = true;
			}
		}
		if (!is_trigger_aligned)
		{
			continue;
		}

		bool is_hit = false;
		trigger_id = i;
		for (int j = 0; j < cELINK_NUM; j++)
		{
			if (!link_enabled[j] || !trigger_aligned[j])
			{
				continue;
			}
			if (position_judge(j))
			{
				is_hit = true;
			}
		}
		if (is_hit)
		{
			for (int k = 0; k < user_set_layer_number; k++)
			{
				hit_result_sort(k);
				detector_data[k].z = z_position[k];
			}
			if (is_write_txt_file)
			{
				write_txt_data();
			}
		}
		hit_ttree->Fill();
	}
	cout << endl;
	return true;
}

void hit_reconstruct::complete_adding()
{
	hit_tfile->cd();
	hit_ttree->Write();
	hit_tfile->Close();
	txt_fileout.close();
	for (int i = 0; i < cELINK_NUM; i++)
	{
		if (!link_enabled[i])
		{
			continue;
		}
		fec_data_file[i]->Close();
	}
}

bool hit_reconstruct::position_judge(int link_num)
{
	int cluster_num = (cluster_number[link_num] > cMAX_CLUSTER) ? cMAX_CLUSTER : cluster_number[link_num];
	if (cluster_num == 0)
	{
		return false;
	}

	double total_amp;
	double sum_position_amp = 0.0;
	double hit_position;
	int idx;
	int base = 0;
	for (int k = 0; k < cluster_num; k++)
	{

		sum_position_amp = 0;
		total_amp = 0;
		for (int j = 0; j < cluster_size[link_num][k]; j++)
		{
			total_amp += hit_amp[link_num][j + base];
			//sum_position_amp += hit_amp[link_num][j + base] * hit_strips[link_num][j + base];
			sum_position_amp += hit_amp[link_num][j + base] * hit_strips[link_num][j + base];
		}
		hit_position = sum_position_amp / total_amp;
		idx = dimension_idx[link_num][base];

		//int det_idx = link_num * 4 + idx;
    //
		int det_idx = link_num * 2 + idx; //changed by wangying FIXME 2024-08-07 11:53:43

    //info_out(link_num);
    //info_out(det_idx);
    //for (int i=0; i<128; ++i){
    //  for (int j=0; j<2; ++j) std::cout<<layer_idx[i][j]<<" ";
    //  std::cout<<std::endl;
    //}
    //exit(0);
    
		int layer = layer_idx[det_idx][0];
		bool is_x = layer_idx[det_idx][1];
		if (layer == -1 || is_x == -1)
		{
			continue;
		}

    //hit_position *= 0.04;



		if (is_x)
		{
			detector_data[layer].x[detector_data[layer].x_nhits] = hit_position;
			detector_data[layer].x_amp[detector_data[layer].x_nhits] = total_amp;
			detector_data[layer].x_chn_num[detector_data[layer].x_nhits] = cluster_size[link_num][k];
			detector_data[layer].x_cluster_hole[detector_data[layer].x_nhits] = cluster_hole[link_num][k];
			detector_data[layer].x_nhits++;
			if (detector_data[layer].x_nhits > 25)
			{
				cout << cYELLOW << "Layer " << layer << " X dimension has nhits larger than 25. This will break out the process" << cRESET << endl;
			}
		}
		else
		{
			detector_data[layer].y[detector_data[layer].y_nhits] = hit_position;
			detector_data[layer].y_amp[detector_data[layer].y_nhits] = total_amp;
			detector_data[layer].y_chn_num[detector_data[layer].y_nhits] = cluster_size[link_num][k];
			detector_data[layer].y_cluster_hole[detector_data[layer].y_nhits] = cluster_hole[link_num][k];
			detector_data[layer].y_nhits++;
			if (detector_data[layer].y_nhits > 25)
			{
				cout << cYELLOW << "Layer " << layer << " Y dimension has nhits larger than 25. This will break out the process" << cRESET << endl;
			}
		}
		base += cluster_size[link_num][k];
	}

	return true;
}

void hit_reconstruct::hit_result_sort(int layer)
{

	int valid_strips_x_num[25] = {0};
	int valid_strips_y_num[25] = {0};
	int hit_amp_x[25] = {0};
	int hit_amp_y[25] = {0};
	for (int k = 0; k < detector_data[layer].x_nhits; k++)
	{
		valid_strips_x_num[k] = detector_data[layer].x_chn_num[k] - detector_data[layer].x_cluster_hole[k];
	}
	for (int k = 0; k < detector_data[layer].y_nhits; k++)
	{
		valid_strips_y_num[k] = detector_data[layer].y_chn_num[k] - detector_data[layer].y_cluster_hole[k];
	}
	double tmp_d;
	int tmp_i;

	/* X */
	int nhits = detector_data[layer].x_nhits;
	for (int i = 0; i < nhits - 1; i++)
	{
		for (int j = 0; j < nhits - i - 1; j++)
		{
			if (valid_strips_x_num[j] < valid_strips_x_num[j + 1])
			{
				swap(valid_strips_x_num[j], valid_strips_x_num[j + 1]);
				swap(detector_data[layer].x[j], detector_data[layer].x[j + 1]);
				swap(detector_data[layer].x_amp[j], detector_data[layer].x_amp[j + 1]);
				swap(detector_data[layer].x_chn_num[j], detector_data[layer].x_chn_num[j + 1]);
				swap(detector_data[layer].x_cluster_hole[j], detector_data[layer].x_cluster_hole[j + 1]);
			}
		}
	}
	for (int k = 0; k < detector_data[layer].x_nhits; k++)
	{
		hit_amp_x[k] = detector_data[layer].x_amp[k];
	}
	for (int i = 0; i < nhits; i++)
	{
		for (int j = 0; j < nhits - i - 1; j++)
		{
			if (valid_strips_x_num[j] == valid_strips_x_num[j + 1] && (hit_amp_x[j] + 100) < hit_amp_x[j + 1])
			{
				swap(valid_strips_x_num[j], valid_strips_x_num[j + 1]);
				swap(hit_amp_x[j], hit_amp_x[j + 1]);
				swap(detector_data[layer].x[j], detector_data[layer].x[j + 1]);
				swap(detector_data[layer].x_amp[j], detector_data[layer].x_amp[j + 1]);
				swap(detector_data[layer].x_chn_num[j], detector_data[layer].x_chn_num[j + 1]);
				swap(detector_data[layer].x_cluster_hole[j], detector_data[layer].x_cluster_hole[j + 1]);
			}
		}
	}

	/* Y */
	nhits = detector_data[layer].y_nhits;
	for (int i = 0; i < nhits - 1; i++)
	{
		for (int j = 0; j < nhits - i - 1; j++)
		{
			if (valid_strips_y_num[j] < valid_strips_y_num[j + 1])
			{
				swap(valid_strips_y_num[j], valid_strips_y_num[j + 1]);
				swap(detector_data[layer].y[j], detector_data[layer].y[j + 1]);
				swap(detector_data[layer].y_amp[j], detector_data[layer].y_amp[j + 1]);
				swap(detector_data[layer].y_chn_num[j], detector_data[layer].y_chn_num[j + 1]);
				swap(detector_data[layer].y_cluster_hole[j], detector_data[layer].y_cluster_hole[j + 1]);
			}
		}
	}
	for (int k = 0; k < detector_data[layer].y_nhits; k++)
	{
		hit_amp_y[k] = detector_data[layer].y_amp[k];
	}
	for (int i = 0; i < nhits; i++)
	{
		for (int j = 0; j < nhits - i - 1; j++)
		{
			if (valid_strips_y_num[j] == valid_strips_y_num[j + 1] && (hit_amp_y[j] + 100) < hit_amp_y[j + 1])
			{
				swap(valid_strips_y_num[j], valid_strips_y_num[j + 1]);
				swap(hit_amp_y[j], hit_amp_y[j + 1]);
				swap(detector_data[layer].y[j], detector_data[layer].y[j + 1]);
				swap(detector_data[layer].y_amp[j], detector_data[layer].y_amp[j + 1]);
				swap(detector_data[layer].y_chn_num[j], detector_data[layer].y_chn_num[j + 1]);
				swap(detector_data[layer].y_cluster_hole[j], detector_data[layer].y_cluster_hole[j + 1]);
			}
		}
	}
}

bool hit_reconstruct::isConvertibleToInt(const string &str)
{
	try
	{
		stoi(str);
		return true;
	}
	catch (invalid_argument e)
	{
		return false;
	}
	catch (out_of_range e)
	{
		return false;
	}
}

void hit_reconstruct::write_txt_data()
{
	txt_fileout << "{" << endl;
	txt_fileout << "  trigger_id:" << trigger_id << endl;
	max_position_file << "trigger_id:" << trigger_id << endl;
	stringstream ss_x;
	stringstream ss_y;
	for (int i = 0; i < user_set_layer_number; i++)
	{
		int max_x = 0;
		int max_x_loc = 0;
		int max_y = 0;
		int max_y_loc = 0;

		/*if (detector_data[i].x_nhits == 0 && detector_data[i].y_nhits == 0)
		{
			continue;
		}*/
		txt_fileout << "[" << endl
					<< "    Layer-" << to_string(i) << endl;
		txt_fileout << "    x_nhits: " << detector_data[i].x_nhits << endl;
		txt_fileout << "    x_position : ";
		for (int j = 0; j < detector_data[i].x_nhits; j++)
		{
			txt_fileout << detector_data[i].x[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    x_strip_num: ";
		for (int j = 0; j < detector_data[i].x_nhits; j++)
		{
			txt_fileout << detector_data[i].x_chn_num[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    x_strip_hole: ";
		for (int j = 0; j < detector_data[i].x_nhits; j++)
		{
			txt_fileout << detector_data[i].x_cluster_hole[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    x_amp : ";
		for (int j = 0; j < detector_data[i].x_nhits; j++)
		{
			if (detector_data[i].x_amp[j] > max_x)
			{
				max_x = detector_data[i].x_amp[j];
				max_x_loc = j;
			}
			txt_fileout << detector_data[i].x_amp[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    y_nhits: " << detector_data[i].y_nhits << endl;
		txt_fileout << "    y_position : ";
		for (int j = 0; j < detector_data[i].y_nhits; j++)
		{
			txt_fileout << detector_data[i].y[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    y_strip_num: ";
		for (int j = 0; j < detector_data[i].y_nhits; j++)
		{
			txt_fileout << detector_data[i].y_chn_num[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    y_strip_hole: ";
		for (int j = 0; j < detector_data[i].y_nhits; j++)
		{
			txt_fileout << detector_data[i].y_cluster_hole[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    y_amp : ";
		for (int j = 0; j < detector_data[i].y_nhits; j++)
		{
			if (detector_data[i].y_amp[j] > max_y)
			{
				max_y = detector_data[i].y_amp[j];
				max_y_loc = j;
			}
			txt_fileout << detector_data[i].y_amp[j] << ",";
		}
		txt_fileout << endl;
		txt_fileout << "    max_x: " << detector_data[i].x[max_x_loc] << " value: " << max_x << endl;
		txt_fileout << "    max_y: " << detector_data[i].y[max_y_loc] << " value: " << max_y << endl;
		txt_fileout << "    z: " << detector_data[i].z;
		txt_fileout << endl
					<< "]" << endl;
		ss_x << detector_data[i].x[max_x_loc] << ":" << max_x;
		ss_y << detector_data[i].y[max_y_loc] << ":" << max_y;
		if (i != cELINK_NUM - 1)
		{
			ss_x << ", ";
			ss_y << ", ";
		}
	}

	txt_fileout << "}" << endl;
	max_position_file << ss_x.str() << endl;
	max_position_file << ss_y.str() << endl;
}

void hit_reconstruct::init_position_data()
{
	for (int i = 0; i < user_set_layer_number; i++)
	{
		detector_data[i] = position_data();
	}
}

int hit_reconstruct::init_datain(string filename)
{
	int min_length = INT_MAX;
	for (int i = 0; i < cELINK_NUM; i++)
	{
		if (!link_enabled[i])
		{
			continue;
		}
		string data_filename = filename + to_string(i) + ".root";
		fec_data_file[i] = new TFile(data_filename.c_str());
		if (fec_data_file[i]->IsZombie() || !fec_data_file[i]->IsOpen())
		{
			cout << cRED << "[ERROR]: File" << data_filename << " open failed. Please check the file" << cRESET << endl;
			return 0;
		}
		fec_data_tree[i] = (TTree *)fec_data_file[i]->Get("asic_hit_data");
		if (fec_data_tree[i] == nullptr)
		{
			cout << cRED << "[ERROR]: Cannot get TTree* asic_hit_data in file \r\n\tPlease check the if the files are correct!" << data_filename << cRESET << endl;
			return 0;
		}
		fec_data_tree[i]->SetBranchAddress("nhits", &nhits_all[i], &b_nhits[i]);
		fec_data_tree[i]->SetBranchAddress("trigger_id", &trigger_id_all[i], &b_trigger_id[i]);
		fec_data_tree[i]->SetBranchAddress("hit_strips", &hit_strips[i][0], &b_hit_strips[i]);
		fec_data_tree[i]->SetBranchAddress("hit_asic_chn", &hit_asic_chn[i][0], &b_hit_asic_chn[i]);
		fec_data_tree[i]->SetBranchAddress("hit_amp", &hit_amp[i][0], &b_hit_amp[i]);
		fec_data_tree[i]->SetBranchAddress("hit_time", &hit_time[i][0], &b_hit_time[i]);
		fec_data_tree[i]->SetBranchAddress("hit_max_position", &hit_max_position[i][0], &b_hit_max_position[i]);
		fec_data_tree[i]->SetBranchAddress("dimension_idx", &dimension_idx[i][0], &b_dimension_idx[i]);
		fec_data_tree[i]->SetBranchAddress("cluster_number", &cluster_number[i], &b_cluster_number[i]);
		fec_data_tree[i]->SetBranchAddress("cluster_size", &cluster_size[i][0], &b_cluster_size[i]);
		fec_data_tree[i]->SetBranchAddress("cluster_holed_num", &cluster_hole[i][0], &b_cluster_hole[i]);
		cout << cGREEN << "TFile " << data_filename << " and TTree initial done" << cRESET << endl;
		if (min_length > fec_data_tree[i]->GetEntries())
		{
			min_length = fec_data_tree[i]->GetEntries();
		}
	}
	return min_length;
}
