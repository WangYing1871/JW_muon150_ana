// calcu_and_draw.cpp: 定义应用程序的入口点。
//

#include "calcu_and_draw.hh"

using namespace std;

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		cout << cRED << "Parameter needed!" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	// -f
	string data_filename;
	// -x
	double theta_x = 0.;
	// -y
	double theta_y = 0.;
	// -o
	string save_filename_append = "calcu_and_draw";
	// -e
	double rmse_upper = 10; // unit mm
	// -N
	int possible_strip_num = 5;
	// -L
	int min_hit_layer = 4;
	// -D
	int det_layer_used = 6;
	// -r offset and rotate matrix
	string offset_filename;
	// -t
	long long timestamp_in = 0;
	// -j
	bool is_save_json = false;
	// -l
	string user_set_layer_used = "3F";
	// -T
	bool is_tomography_system = false;
	// -O
	int calc_target_layer = 0;
	// -F
	bool is_first = false;
	bool is_calc_target_layer = false;
	cout << "Begin process" << endl;
	int c = 0;
	while ((c = getopt(argc, argv, "f:o:e:N:L:D:r:t:jl:TO:F")) != EOF)
	{
		switch (c)
		{
		case 'f':
			data_filename = optarg;
			break;
		case 'o':
			save_filename_append = optarg;
			break;
		case 'e':
			rmse_upper = atof(optarg);
			break;
		case 'N':
			possible_strip_num = atoi(optarg);
			break;
		case 'L':
			min_hit_layer = atoi(optarg);
			break;
		case 'D':
			det_layer_used = atoi(optarg);
			break;
		case 'r':
			offset_filename = optarg;
			break;
		case 't':
			timestamp_in = atoll(optarg);
			break;
		case 'j':
			is_save_json = true;
			break;
		case 'l':
			user_set_layer_used = optarg;
			break;
		case 'T':
			is_tomography_system = true;
			break;
		case 'O':
			calc_target_layer = atoi(optarg);
			is_calc_target_layer = true;
			break;
		case 'F':
			is_first = true;
			break;
		case '?':
			cout << "<<----------\?\?---------->>" << endl;
			cout << cRED << "unknow parameters " << argv[optind] << " " << optarg << cRESET << endl;
			cout << "<<----------\?\?---------->>" << endl;
			cout << "Or forget break in the last case???" << endl;
			show_info(argv[0]);
			return -1;
			break;
		default:
			cout << cRED << "Some option is not include in the switch. Please check the main()" << cRESET << endl;
			show_info(argv[0]);
			break;
		}
	}
	auto now = std::chrono::system_clock::now();

	// 转换为自1970年1月1日以来的毫秒数
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

	// 获取13位的Unix时间戳
	long long timestamp = duration.count();
	cout << "timestamp: " << timestamp << endl;
	if (timestamp_in != 0)
	{
		timestamp = timestamp_in;
	}
	else
	{
		cout << "No time stamp input use default" << endl;
	}

	if (data_filename.empty())
	{
		cout << cRED << " -f parameter is needed" << cRESET << endl;
		show_info(argv[0]);
	}
	vector<string> filename_suffix = split_string(data_filename, ".root");
	if (filename_suffix.size() != 2)
	{
		cout << cRED << "File name ERROR: " << data_filename << cRESET << endl;
		return -1;
	}
	vector<string> filename_suffix2 = split_string(data_filename, "/");
	string save_filename = filename_suffix[1] + "_" + save_filename_append + "_rmse" + to_string((int)rmse_upper) + "mm" + to_string(possible_strip_num) + "strips" + to_string(min_hit_layer) + "min_layer_" + user_set_layer_used;
	//best_tracker tracker_find(data_filename, save_filename, det_layer_used);
	
	info_calc info;
	info.set_input_filename(data_filename);
        info.init();
	info.execute();
	info.finish();

	//delete info;
	//return 0;
/*
	string save_file_path = "./";
	if (filename_suffix2.size() == 1)
	{
		save_file_path = "./";
	}
	else
	{
		save_file_path = filename_suffix2[1] + "/" + to_string(timestamp) + "/";
	}

	// Check the save path is exist or not if not create
	if (is_save_json)
	{

		if (stat(save_file_path.c_str(), &info) != 0)
		{
			cout << cYELLOW << "Path " << save_file_path << " does not exist and create one" << endl
				 << cRESET;
			fs::create_directory(save_file_path);
		}
		else if (info.st_mode & S_IFDIR)
		{
			cout << cGREEN << "Save file at " << save_file_path << cRESET << endl;
		}
		else
		{
			cout << cYELLOW << "Path " << save_file_path << " does not exist and create one" << endl
				 << cRESET;
			fs::create_directory(save_file_path);
		}
		fs::create_directory(save_file_path + "status/");
		fs::create_directory(save_file_path + "event/");
		tracker_find.save_file_path = save_file_path;
	}
	tracker_find.set_rmse_vth(rmse_upper);
	tracker_find.set_possible_strip_vth(possible_strip_num);
	tracker_find.set_min_layer(min_hit_layer);
	// Check if the uset_set_layer_used is a hex number
	int user_set_layer_used_int = 0;
	stringstream ss;
	ss << hex << user_set_layer_used;
	ss >> user_set_layer_used_int;
	tracker_find.set_layer_used(user_set_layer_used_int);
	if (!offset_filename.empty())
	{
		tracker_find.load_offset_param(offset_filename);
	}
	if (is_calc_target_layer)
	{
		tracker_find.set_target_layer_calc(calc_target_layer, is_first);
	}
*/


	// Time start
	cout << "Start to run" << endl;
	cout << "File name: " << data_filename << endl;
	cout << "Save file name: " << save_filename << endl;
	cout << "RMSE upper limit: " << rmse_upper << " mm" << endl;
	cout << "Possible strip number: " << possible_strip_num << endl;
	cout << "Minimum hit layer: " << min_hit_layer << endl;
	cout << "Detector used: " << det_layer_used << endl;
	cout << "User set layer used: " << user_set_layer_used << endl;
	cout << "Offset file name: " << offset_filename << endl;
	cout << "Timestamp: " << timestamp << endl;
	// Show start time
	auto start = std::chrono::system_clock::now();
	auto start_time = std::chrono::system_clock::to_time_t(start);
	cout << "Start time: " << std::ctime(&start_time) << endl;
	
/*	
	tracker_find.set_tomography_system(is_tomography_system);
	if (!tracker_find.run())
	{
		cout << cRED << "Run ERROR" << cRESET << endl;
		return -1;
	}

	tracker_find.show_result();
*/


	auto end = std::chrono::system_clock::now();
	auto end_time = std::chrono::system_clock::to_time_t(end);
	cout << "End time: " << std::ctime(&end_time) << endl;
	cout << "Time cost: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << endl;
}

void show_info(char *name)
{
	for (int i = 0; i < 30; i++)
		cout << "-";
	cout << endl;
	cout << cGREEN << "Usage: " << name << " -[parameter]" << endl
		 << cBLUE << "Parameter lists:" << cGREEN << endl
		 << "    -f [Filename] (required)" << endl
		 << cCYAN
		 << "    -x [theta x]" << endl
		 << "    -y [theta_y]" << endl
		 << "    -e [Upper limit of the RMSE (Unit mm, default: 10mm)] " << endl
		 << "    -N [Maximum possible hit strip number, default 3]" << endl
		 << "    -L [Min hit layer, default 3]" << endl
		 << "    -D [Detetor number used (Default: 4)]" << endl
		 << "    -r [offset filename] (option)"
		 << "    -t [timestamp] (option)" << endl
		 << "    -l [layer used] (option)" << endl
		 << "    -T [Tomography system] (option)" << endl
		 << "    -O [Target layer to calculate] (option)" << endl
		 << "    -F [Is choose first hit to calculate offset] (option)" << endl

		 << cRESET << endl;
}

vector<string> split_string(const string &str, string delims, int inc)
{
	std::vector<std::string> result;
	size_t i = str.rfind(delims, str.length());
	if (i == str.length() - 1 || i == string::npos)
	{
		result.push_back(str);
	}
	else
	{
		result.push_back(str.substr(i + inc, str.length() - i));
		result.push_back(str.substr(0, i));
	}
	return result;
}
