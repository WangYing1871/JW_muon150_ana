#include "adas_data_dec.hh"

using namespace std;

int main(int argc, char *argv[])
{
    //-f
    string input_file_name;
    //-O
    string save_path_set = "dec_result";
    //-s
    bool is_old_data = false;
    //-b
    string base_txt = "noise_base.txt";
    //-n
    string rms_txt;
    //-i
    int asic_num = 4;
    //-e
    string encoding_list_filename = "encoding_list.csv";
    //-c
    bool is_allow_dec_discon = true;
    //-w
    bool is_write_txt = false;
    //-L
    int lsb = 15;

    while ((c = getopt(argc, argv, "f:O:s:b:i:e:c:w:L:n:")) != -1)
    {
        switch (c)
        {
        case 'f':
            input_file_name = optarg;
            break;
        case 'O':
            save_path_set = optarg;
            break;
        case 's':
            is_old_data = atoi(optarg);
            break;
        case 'b':
            base_txt = optarg;
            break;
        case 'i':
            asic_num = atoi(optarg);
            break;
        case 'e':
            encoding_list_filename = optarg;
            break;
        case 'c':
            is_allow_dec_discon = atoi(optarg);
            break;
        case 'w':
            is_write_txt = atoi(optarg);
            break;
        case 'L':
            lsb = atoi(optarg);
            break;
        case 'n':
            rms_txt = optarg;
            break;
        case '?':
            cout << cRED;
            cout << "<<---------------------->>" << endl;
            cout << "[Error]: unknow parameters" << endl;
            cout << "<<---------------------->>" << endl;
            cout << "Or forget break in the last case?" << endl;
            show_info(argv[0]);
            return -1;
        default:
            break;
        }
    }

    if (input_file_name.empty())
    {
        cout << cRED;
        cout << "<<---------------------->>" << endl;
        cout << "[Error]: input file name is empty" << endl;
        cout << "<<---------------------->>" << endl;
        show_info(argv[0]);
        return -1;
    }

    if (input_file_name.find(".root") == string::npos)
    {
        cout << cRED;
        cout << "<<---------------------->>" << endl;
        cout << "[Error]: file name is not correct" << endl;
        cout << "<<---------------------->>" << endl;
        show_info(argv[0]);
        return -1;
    }

    for (int i = 0; i < 20; i++)
    {
        cout << "==";
    }
    cout << endl;
    cout << "Begin process:" << endl;
    cout << endl;

    double L = lsb * 2.048 * 0.15492 * 0.001;

    vector<string> file_name_path = split_string(input_file_name, "/");
    string save_path;
    if (file_name_path[1].empty())
    {
        save_path = "./" + save_path_set;
    }
    else
    {
        save_path = file_name_path[1] + "/" + save_path_set;
    }
    if (stat(save_path.c_str(), &info) == -1)
    {
        cout << cYELLOW << "Save path : " << save_path << " does not exist, create it." << cRESET << endl;
        fs::create_directories(save_path);
    }

    base_txt = file_name_path[1] + "/" + base_txt;
    rms_txt = file_name_path[1] + "/" + rms_txt;

    string relative_filename = file_name_path[0];
    vector<string> data_file_prefix = split_string(relative_filename, ".root");

    vector<string> name_tmp = split_string(data_file_prefix[1], "_mt");

    string root_file_name = save_path + name_tmp[1] + "_dec" + name_tmp[0];

    string save_path1 = save_path + "/" + data_file_prefix[1] + "_";

    dec_pro dec_pro(base_txt
        ,rms_txt
        ,input_file_name
        ,root_file_name
        ,save_path1
        ,is_old_data
        ,is_write_txt
        ,asic_num
        ,encoding_list_filename
        ,is_allow_dec_discon
        ,L);

    cout << cGREEN << "Done!" << cRESET << endl;
    return 0;
}

void show_info(char *name)
{
    cout << cCYAN;
    for (int i = 0; i < 15; i++)
    {
        cout << "==";
    }
    cout << endl
         << "Usage: " << name << " -[parameter] [options]" << endl
         << "    -f [input data filename. If single file, specific the filename] (required)" << endl
         << "    -O [output data path] (default: dec_result)" << endl
         << "    -s [is old data? 0: new data, 1: old data] (default: 0)" << endl
         << "    -b [noise base txt file] (default: noise_base.txt)" << endl
         << "    -i [asic number] (default: 4)" << endl
         << "    -e [encoding list filename] (default: encoding_list.csv)" << endl
         << "    -c [is allow decoding disconnection? 0: not allow, 1: allow] (default: 1)" << endl
         << "    -w [is write txt? 0: not write, 1: write] (default: 0)" << endl
         << "    -L [LSB] (default: 15)" << endl
         << "    -n [noise rms txt file] (default: none)" << endl
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
