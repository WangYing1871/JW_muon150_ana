// fec_noise_calc.cc : Defines the entry point for the application.
//

#include "fec_noise_calc.hh"

using namespace std;

int main(int argc, char **argv)
{
	int t_argc = 1;
	char *t_argv = new char[10];
	// TApplication app("ROOT Application", &t_argc, &t_argv);
	if (argc == 1)
	{
		show_info(argv[0]);
		return -1;
	}
	cout << "Begin process" << endl;
	int c = 0;
	// -f
	string data_filename;
	// -l
	int link_number = -1;
	// -F for FELIX
	string loop_file_suffix;
	// -L
	int loop_num = 1;
	// -s
	double sigma = 5;
	// -o
	string save_path;
	// -O
	bool is_save_plot = false;
	bool is_arg_get = false;
	// -U
	bool is_usb = true;
	// -Z
	bool is_write_zero_cmd = false;
	// -M
	bool is_set_max_noise = false;
	double max_noise_value = 0;
	// -p
	bool is_save_pedestal = false;
	// -m Mask asic idx
	string mask_asic_idx; // Default no mask;
	while ((c = getopt(argc, argv, "f:l:F:L:s:ouUZM:m:p")) != EOF)
	{
		is_arg_get = true;
		switch (c)
		{
		case 'f':
			data_filename = optarg;
			break;
		case 'l':
			link_number = atoi(optarg);
			break;
		case 'F':
			loop_file_suffix = optarg;
			break;
		case 'L':
			loop_num = atoi(optarg);
		case 's':
			sigma = atof(optarg);
			break;
		case 'o':
			is_save_plot = true;
			break;
		case 'u':
		case 'U':
			is_usb = true;
			break;
		case 'z':
		case 'Z':
			is_write_zero_cmd = true;
			break;
		case 'M':
			is_set_max_noise = true;
			max_noise_value = atof(optarg);
			break;
		case 'p':
			is_save_pedestal = true;
			cout << cCYAN << "Save pedestal mean value" << cRESET << endl;
			break;
		case 'm':
			mask_asic_idx = optarg;
			break;
		case '?':
			cout << cRED << "Unknown parameter " << endl;
			show_info(argv[0]);
			return -1;
		default:
			break;
		}
	}

	if (is_arg_get && is_write_zero_cmd)
	{
		noise_calc noise2preocess("empty");
		if (is_usb)
		{
			string file2write = save_path + "zero_vth_usb.txt";
			noise2preocess.write_usb_cmd(file2write);
		}
		else
		{
			string file2write = save_path + "zero_vth_felix.txt";
			noise2preocess.write_flx_cmd(file2write);
		}
		cout << cGREEN << "Done" << cRESET << endl;
		return 0;
	}

	if (!is_arg_get || data_filename.empty())
	{
		cout << "-f or -S must be specified" << endl;
		show_info(argv[0]);
		return -1;
	}
	bool is_single_file = false;
	if (data_filename.find(".root") != string::npos)
	{
		is_single_file = true;
	}
	vector<string> data_filename_path = split_string(data_filename, "/", 1);
	cout << cCYAN << "Data file path: " << data_filename_path[1] << cRESET << endl;
	cout << cCYAN << "Data file name: " << data_filename_path[0] << cRESET << endl;
	if (save_path.empty())
	{
		if (data_filename_path[1].empty())
		{
			save_path = "./";
		}
		else
		{
			save_path = data_filename_path[1] + "/";
		}
	}
	string filename = data_filename_path[0];

	noise_calc noise2preocess(filename);
	cout << cCYAN << "Calculate file: " << data_filename << cRESET << endl;
	bool asic_mask[4] = {false};
	if (!mask_asic_idx.empty())
	{
		int asic_idx = 0;
		stringstream ss;
		ss << hex << mask_asic_idx;
		ss >> asic_idx;
		for (int i = 0; i < 4; i++)
		{
			if (((asic_idx >> i) & 1) == 1)
			{
				noise2preocess.asic_mask_idx[i] = true;
			}
			else
			{
				noise2preocess.asic_mask_idx[i] = false;
			}
		}
	}
	if (is_set_max_noise)
	{
		noise2preocess.set_max_noise(max_noise_value);
	}
	noise2preocess.is_save_mean = is_save_pedestal;

	if (is_single_file)
	{
		if (!noise2preocess.fill_fec_data(data_filename))
		{
			// cout << cRED << "Error: Can not open file " << data_filename << cRESET << endl;
			return -1;
		}
	}
	else
	{
		if (loop_num == 1)
		{
			cout << cYELLOW
				 << "Warning: Select multiple files mode but loop number is "
				 << cRED << loop_num << cRESET << endl;
		}
		string file2read;
		for (int i = 0; i < loop_num; i++)
		{
			file2read = data_filename + to_string(i) + loop_file_suffix + ".root";
			if (!noise2preocess.fill_fec_data(data_filename))
			{
				cout << cRED << "Error: Can not open file " << file2read << cRESET << endl;
				return -1;
			}
		}
	}
	string save_suffix;
	vector<string> file_name_suffix = split_string(filename, ".");
	cout << cCYAN << file_name_suffix[1] << cRESET << endl;
	cout << cCYAN << file_name_suffix[0] << cRESET << endl;
	if (link_number == -1)
	{

		save_suffix = file_name_suffix[1];
	}
	else
	{
		save_suffix = "link-" + to_string(link_number);
	}

	noise2preocess.run_noise_calc();
	string file2write;
	file2write = save_path + "result_" + save_suffix + ".root";
	noise2preocess.write(file2write);
	if (is_usb)
	{
		file2write = save_path + "3sigma_vth_" + save_suffix + "_usb" + ".txt";
		noise2preocess.write_usb_cmd(file2write);
	}
	else
	{
		file2write = save_path + "3sigma_vth_" + save_suffix + "_felix" + ".txt";
		noise2preocess.write_flx_cmd(file2write);
	}

	if (is_save_plot)
	{
		noise2preocess.is_save = true;
		noise2preocess.save_path = save_path;
		noise2preocess.plot(file_name_suffix[1]);
	}

	return 0;
}

void show_info(const char *info)
{
	cout << cGREEN << "Usage:" << info << endl
		 << cCYAN
		 << "    -f [Noise file path]" << endl
		 << "    -l [Link number](If not set, all the file will save with the input filename)" << endl
		 << "    -F [Loop file suffix]" << endl
		 << "    -L [Loop number]" << endl
		 << "    -s [Sigma level]" << endl
		 << "    -o [is_save_plot]" << endl
		 << "    -u or -U (Is USB)" << endl
		 << "    -M [Max RMS value]" << endl
		 << cYELLOW << "    -Z (Write only zero command mode)"

		 << cRESET << endl;
}
