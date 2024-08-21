// usb_data_seperate.cpp : Defines the entry point for the application.
//

#include "usb_data_separate.hh"


using namespace std;

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		cout << cRED << "At least -f parameter is needed!" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	// -f
	string raw_data_file;
	// -o
	string save_prefix = "mt";
	// -O
	string save_path;
	// -M
	int max_fec_num = 16;
	// -r
	bool is_resort = false;
	// -D
	bool is_record_log = false;

	bool arg_hit = false;
	while ((c = getopt(argc, argv, "f:o:O:M:rD")) != EOF)
	{
		arg_hit = true;
		switch (c)
		{
		case 'f':
			raw_data_file = optarg;
			break;
		case 'o':
			save_prefix = optarg;
			break;
		case 'O':
			save_path = optarg;
			break;
		case 'M':
			max_fec_num = atoi(optarg);
			break;
		case 'r':
			is_resort = true;
			break;
		case 'D':
			is_record_log = true;
			break;
		case '?':
			cout << "<<----------\?\?---------->>" << endl;
			cout << "unknow parameters" << endl;
			cout << "<<----------\?\?---------->>" << endl;
			cout << "Or forget break in the last case???" << endl;
			show_info(argv[0]);
			return -1;
			break;
		default:
			break;
		}
	}
	if (!arg_hit)
	{
		cout << cRED << "No parameter" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	// cout << raw_data_file << endl;
	vector<string> data_filename_path = split_string(raw_data_file, '/');
	if (save_path.empty())
	{
		if (data_filename_path[1].empty())
		{
			save_path = "./dec_out/";
		}
		else
		{
			save_path = data_filename_path[1] + "/dec_out/";
		}
	}
	if (stat(save_path.c_str(), &info) != 0)
	{
		cout << cYELLOW << "Path " << save_path << " does not exist and create one" << endl
			 << cRESET;
		fs::create_directory(save_path);
	}
	else if (info.st_mode & S_IFDIR)
		cout << cGREEN << "Save file at " << save_path << cRESET << endl;
	else
	{
		cout << cYELLOW << "Path " << save_path << " does not exist and create one" << endl
			 << cRESET;
		fs::create_directory(save_path);
	}
	string app_name = argv[0];
	string log_filename = "./" + app_name + "_name.log";
	ofstream log_file;
	string data_filename = data_filename_path[0];
	vector<string> data_filename_suffix = split_string(data_filename, '.');
	vector<string> save_filename;
	ofstream save_file[16];
	bool is_data_get[16] = {false};

	if (is_record_log)
	{
		log_file.open(log_filename, ios_base::app);
		for (int i = 0; i < 20; i++)
		{
			log_file << "--";
		}
		log_file << endl;
		log_file << "Filename: " << raw_data_file << endl;
	}

	ifstream data_file;
	// unsigned short* usb_daq_data;
	// long usb_data_length = 0;
	unsigned char *data_buffer;
	long data_num_bytes;
	data_file.open(raw_data_file, ifstream::binary);
	if (data_file.is_open())
	{
		data_file.seekg(0, data_file.end);
		data_num_bytes = data_file.tellg();
		data_file.seekg(0, data_file.beg);
		data_buffer = new unsigned char[data_num_bytes];
		auto start = chrono::high_resolution_clock::now();
		data_file.read((char *)data_buffer, data_num_bytes);
		auto finish = chrono::high_resolution_clock::now();

		auto microseconds = chrono::duration_cast<chrono::microseconds>(finish - start);
		double read_time_s = microseconds.count() / 1000. / 1000.;

		data_file.close();
		cout << "----------" << endl;
		cout << "File " << raw_data_file << " read successfully (" << read_time_s * 1000. << "ms). "
			 << "File size: "
			 << data_num_bytes / 1024. / 1024. << "MB"
			 << ", Speed: " << data_num_bytes / 1024. / 1024. / read_time_s << "MB/s"
			 << endl;
		/*usb_data_length = floor(data_num_bytes / 2);
		usb_daq_data = new unsigned short[usb_data_length];
		long k = 0;
		for (long i = 0; i < data_num_bytes; i += 2)
		{
			usb_daq_data[k] = (unsigned char)data_buffer[i] * 0x100 + (unsigned char)data_buffer[i + 1];
			k++;
		}
		delete data_buffer;
		*/
		if (is_resort)
		{
			cout << "Data resort" << endl;
			cout << hex << data_num_bytes << dec << endl;
			unsigned char tmp;
			for (int i = 0; i < data_num_bytes; i += 4)
			{
				tmp = data_buffer[i + 3];
				data_buffer[i + 3] = data_buffer[i];
				data_buffer[i] = tmp;
				tmp = data_buffer[i + 2];
				data_buffer[i + 2] = data_buffer[i + 1];
				data_buffer[i + 1] = tmp;
			}
		}
	}
	else
	{
		cout << cRED << "File not exist!" << cRESET << endl;
		return -1;
	}

	for (int i = 0; i < 16; i++)
	{
		string filename_tmp = save_path + data_filename_suffix[1] + "_" + save_prefix + "-" + to_string(i) + ".dat";
		save_filename.push_back(filename_tmp);
		save_file[i].open(filename_tmp);
	}

	int chn_header_record = 0x54;

	long rd_ptr = 0;
	bool is_reach_start = false;
	bool is_err_clr = true;
	unsigned short pkg_cnt = 0;
	unsigned short last_pkt_cnt = 0;
	bool first_pkg = true;
	unsigned short frame_preheader1 = data_buffer[rd_ptr] * 0x100 + data_buffer[rd_ptr + 1];
	unsigned short frame_preheader2 = data_buffer[rd_ptr + 2] * 0x100; // + data_buffer[rd_ptr + 3];
	cout << frame_preheader1 << " " << frame_preheader2 << endl;
	while (rd_ptr < data_num_bytes - 4096)
	{

		unsigned short frame_preheader1 = data_buffer[rd_ptr] * 0x100 + data_buffer[rd_ptr + 1];
		unsigned short frame_preheader2 = data_buffer[rd_ptr + 2] * 0x100; // + data_buffer[rd_ptr + 3];

		rd_ptr += 4;
		if (frame_preheader1 != 0xFFAB || frame_preheader2 != 0x5300)
		{

			if ((frame_preheader1 != 0000 && frame_preheader2 != 0000) && (frame_preheader1 != 0xBBBB && frame_preheader2 != 0xBFBF) && is_reach_start && is_err_clr)
			{
				if (data_num_bytes - rd_ptr > 16384)
				{
					is_err_clr = false;
					cout << cYELLOW << "frame_prehaeder error. Read value: 0x" << hex << frame_preheader1 << " 0x" << frame_preheader2 << cRESET << ". At position: 0x" << rd_ptr << dec << endl;
					if (is_record_log)
					{
						log_file << "[Error] frame_prehaeder error. Read value: 0x" << hex << frame_preheader1 << " 0x" << frame_preheader2 << ". At position: 0x" << rd_ptr << dec << endl;
					}
				}
				else
				{

					break;
				}
			}

			continue;
		}

		is_err_clr = true;
		is_reach_start = true;
		pkg_cnt = data_buffer[rd_ptr] * 0x100 + data_buffer[rd_ptr + 1];
		rd_ptr += 2;

		if (pkg_cnt != (last_pkt_cnt + 1) && (last_pkt_cnt != 0xFFFF) && !first_pkg)
		{
			cout << cRED << "Pkg count is not continious. pkg_cnt: 0x" << hex << pkg_cnt
				 << " last_pkg_cnt: 0x" << last_pkt_cnt << " at position 0x" << rd_ptr
				 << " Lost pkg num: 0x" << (unsigned short)(pkg_cnt - last_pkt_cnt - 1) << cRESET << dec << endl;
			if (is_record_log)
			{
				log_file << "[Error] Pkg count is not continious. pkg_cnt: 0x" << hex << pkg_cnt
						 << " last_pkg_cnt: 0x" << last_pkt_cnt << " at position 0x" << rd_ptr
						 << " Lost pkg num: 0x" << (unsigned short)(pkg_cnt - last_pkt_cnt - 1) << dec << endl;
			}
		}
		last_pkt_cnt = pkg_cnt;
		first_pkg = false;
		// cout << "Frame header: " << hex << frame_header << dec << endl;
		/*if (frame_header != 0xFABB && frame_header != 0xFAFF)
		{
			cout << cRED << " package error at " << hex << rd_ptr << " value: 0x" << frame_header << " 0x" << frame_preheader2 << dec << cRESET << endl;
			break;
		}*/
		unsigned short chn_header = data_buffer[rd_ptr];

		int chn_info = data_buffer[rd_ptr + 1];
		// cout << "Current chn: " << chn_info << endl;
		rd_ptr += 2;
		if (chn_header != chn_header_record)
		{
			cout << cGREEN << "Channel header at 0x" << hex << rd_ptr << " data value is 0x" << chn_header << ". Please check this value with the designer." << dec << cRESET << endl;
			cout << "Set channel header to 0x" << hex << chn_header << dec << endl;
			chn_header_record = chn_header;
			if (is_record_log)
			{
				log_file << "[Error] Channel header at 0x" << hex << rd_ptr << " data value is 0x" << chn_header << dec << endl;
			}
		}
		int fec_num = chn_info & 0xff;

		if (!is_data_get[fec_num])
		{
			is_data_get[fec_num] = true;
		}
		// if (frame_header == 0xFABB)
		{
			unsigned short frame_tail = data_buffer[rd_ptr + 4096 - 8 - 8] * 0x100 +
										data_buffer[rd_ptr + 4097 - 8 - 8];
			unsigned short frame_full = data_buffer[rd_ptr + 4096 - 8 - 8 + 2] * 0x100 +
										data_buffer[rd_ptr + 4097 - 8 - 8 + 2];
			if (frame_tail != 0xFFCC)
			{
				cout << cRED << "Frame tail error at 0x" << hex << (rd_ptr + 4096 - 8) << dec << cYELLOW << hex << " Value: 0x" << frame_tail << dec << cRESET << endl;
				if (is_record_log)
				{
					log_file << "[Error] Frame tail error at 0x" << hex << (rd_ptr + 4096 - 8) << dec << cYELLOW << hex << " Value: 0x" << frame_tail << dec << endl;
				}
			}
			if (frame_full != 0x0000)
			{
				cout << cYELLOW << "Frame full at 0x" << hex << (rd_ptr + 4096 - 8 + 2) << dec << cYELLOW << hex << " Value: 0x" << frame_full << dec << cRESET << endl;
			}
			unsigned char *buffer;
			buffer = &data_buffer[rd_ptr];
			save_file[fec_num].write((char *)buffer, 4096 - 8 - 8);
			rd_ptr += 4096 - 8;
			// rd_ptr += 8;
		}
	}

	for (int i = 0; i < 16; i++)
	{
		save_file[i].close();
		if (!is_data_get[i])
		{
			remove(save_filename[i].c_str());
		}
		if (is_data_get[i])
		{
			cout << "Save file at :" << cGREEN << save_filename[i] << endl
				 << cRESET;
			if (is_record_log)
			{
				log_file << "[Info] Save file at :" << save_filename[i] << endl;
			}
		}
	}
	if (is_record_log)
	{
		log_file.flush();
		log_file.close();
	}
}

void show_info(char *name)
{
	cout << cGREEN << " Usage: " << name << endl
		 << "    -f [data filename]" << endl
		 << cCYAN << "    -o [output prefix](optional, default: \"mt\")" << endl
		 << "    -O [output path](optional, default: ./)" << endl
		 << "    -M [maximum FEC in use](optional, default: 16)" << endl
		 << cRESET;
}

vector<string> split_string(const string &str, char delims, string str2push)
{
	std::vector<std::string> result;
	size_t i = str.rfind(delims, str.length());

	if (i == str.length() - 1 || i == string::npos)
	{
		result.push_back(str);
		result.push_back(str2push);
	}
	else
	{
		result.push_back(str.substr(i + 1, str.length() - 1));
		result.push_back(str.substr(0, i));
	}
	return result;
}
