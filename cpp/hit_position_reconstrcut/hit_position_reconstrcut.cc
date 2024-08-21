#include <iostream>
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"

#include "hit_position_reconstrcut.hh"

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
	// -L
	string link_selected;
	// -l Layer number
	int user_set_layer_number = 0;
	// -p
	string position_filename;
	// -z
	string z_filename;
	// -o Save append name
	string save_appending = "";
	// -V Show run info before run
	bool is_show_info = true;
	// -T is write txt file for test
	bool is_write_txt_file = false;
	cout << "Begin process" << endl;
	int c = 0;
	while ((c = getopt(argc, argv, "f:L:l:p:z:Z:o:VT")) != EOF)
	{
		switch (c)
		{
		case 'f':
			data_filename = optarg;
			break;
		case 'L':
			link_selected = optarg;
			break;
		case 'l':
			user_set_layer_number = atoi(optarg);
			break;
		case 'p':
			position_filename = optarg;
			break;
		case 'Z':
		case 'z':
			z_filename = optarg;
			break;
		case 'o':
			save_appending = optarg;
			break;
		case 'V':
			is_show_info = false;
			break;
		case 'T':
			is_write_txt_file = true;
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
	if (data_filename.empty())
	{
		cout << cRED << "Data filename is needed" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	if (link_selected.empty())
	{
		cout << cRED << "Link is needed." << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	if (position_filename.empty())
	{
		cout << cRED << "Position filename is needed!" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	if (z_filename.empty())
	{
		cout << cRED << "Z position filename is neeeded!" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	if (user_set_layer_number == 0)
	{
		cout << cRED << "Layer number is needed!" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	int link_used = 0;
	stringstream ss;
	ss << hex << link_selected;
	ss >> link_used;
	vector<string> filename_suffix = split_string(data_filename, "-");
	if (is_show_info)
	{
		cout << "===========" << endl;
		cout << cCYAN;
		cout << "Data filename: " << data_filename << endl;
		cout << "Link used: " << hex << link_used << dec << endl;
		cout << "Used links: ";
		for (int i = 0; i < cELINK_NUM; i++)
		{
			if (link_used & (0x1 << i))
			{
				cout << i << " ";
			}
		}
		cout << endl;
		cout << "Position filename: " << position_filename << endl;
		cout << "Z filename: " << z_filename << endl;
		cout << "Saving filename: " << filename_suffix[1] + "_hit_data_link" + link_selected + save_appending << endl;
		cout << "User set layer number: " << user_set_layer_number << endl;
		cout << cRESET;
		cout << "===========" << endl;
		cout << "To disable this info, use -V with running" << endl;
		cout << "Confirm? (y/n)" << endl;
		char confirm;
		cin >> confirm;
		if (confirm != 'y')
		{
			cout << cRED << "Exit!" << cRESET << endl;
			cout << "===========" << endl;
			return -1;
		}
	}
  //info_out(filename_suffix[1]);
  //info_out(link_selected);
  //info_out(save_appending);
  //info_out(link_used);
  //info_out(user_set_layer_number);
  //info_out(position_filename);
  //info_out(z_filename);
  //info_out(is_write_txt_file);
	hit_reconstruct hit_rec(filename_suffix[1] + "_hit_data_link" + link_selected + save_appending, link_used, user_set_layer_number,position_filename, z_filename, is_write_txt_file);
	if (!hit_rec.is_init_successed)
	{
		cout << cRED << "Cannot init the hit_reconstruct. ";
		cout << "Please see the error before this line" << cRESET << endl;
		cout << cRED << "Exit!" << cRESET << endl;
		cout << cYELLOW << "===========" << endl;
		return -1;
	}
	if (!hit_rec.fill_data(filename_suffix[1] + "-"))
	{
		cout << cRED << "Cannot fill the data" << cRESET << endl;
		cout << cRED << "Exit!" << cRESET << endl;
		cout << "===========" << endl;
		return -1;
	}
	if (!hit_rec.is_datain_get)
	{
		cout << "===========" << endl;
		cout << cRED << "Cannot get the data in file" << cRESET << endl;
		cout << cRED << "Exit!" << cRESET << endl;
		cout << "===========" << endl;
		return -1;
	}
	hit_rec.complete_adding();
}

void show_info(char *name)
{
	for (int i = 0; i < 30; i++)
		cout << "-";
	cout << endl;
	cout << cBLUE << "Usage: " << name << " -[parameter]" << endl
		 << cGREEN << "Parameter lists:" << cBLUE << endl
		 << "    -f [Filename] (required)" << endl
		 << "    -L [Link used in HEX] (require).0x1 stands for link0, 0x3 stands for link0, 1 0x6 stands for link1 and 2,..." << endl
		 << "    -p [Position file] (require)" << endl
		 << "    -z [Z layer file. Unit: mm] (require)" << endl
		 << "    -l [Layer number] (require)" << cYELLOW << " Please check this when using in new system" << endl
		 << cCYAN
		 << "    -o [Save appending]" << endl
		 << "    -V [Disable run info]" << endl
		 << "    -T [Write txt file for test]" << endl
		 << cBLUE << "Example: " << name << " -f some_datafilename.dat -L 0xF -p data/position.txt -z data/z.txt -o _test" << endl

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
