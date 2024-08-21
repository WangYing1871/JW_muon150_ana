#include "adas_muxbrd_fit.hh"

using namespace std;

int main(int argc, char *argv[])
{
    //-f
    string input_file_name;
    //-O
    string save_path_set = "fit_result";
    //-w
    bool is_write_txt = false;
    //-l
    int layer_num = 6;
    //-r
    float rmse_precision = 0.5;
    //-u
    bool idx_discontinuous = false;
    //-v
    int idx_disc = 0;
    //-a
    string alignment_filename = "alignment.txt";
    //-g
    double sigma = 4.6;
    //-L
    double LSB = 15;
    //-R
    string detector_hitrange_filename = "detector_hitrange.txt";
    //-s
    bool is_req_xy = false;
    //-A
    bool is_angle = false;
    //-G
    double angle = 5;
    //-t
    bool is_muon_scatter = false;
    //bool is_muon_scatter = true;

    while ((c = getopt(argc, argv, "f:O:w:l:r:uv:a:g:L:R:sAG:t")) != -1)
    {
        switch (c)
        {
        case 'f':
            input_file_name = optarg;
            break;
        case 'O':
            save_path_set = optarg;
            break;
        case 'w':
            is_write_txt = true;
            break;
        case 'l':
            layer_num = atoi(optarg);
            break;
        case 'r':
            rmse_precision = atof(optarg);
            break;
        case 'u':
            idx_discontinuous = true;
            break;
        case 'v':
            idx_disc = atoi(optarg);
            break;
        case 'a':
            alignment_filename = optarg;
            break;
        case 'g':
            sigma = atof(optarg);
            break;
        case 'L':
            LSB = atof(optarg);
            break;
        case 'R':
            detector_hitrange_filename = optarg;
            break;
        case 's':
            is_req_xy = true;
            break;
        case 'A':
            is_angle = true;
            break;
        case 'G':
            angle = atof(optarg);
            break;
        case 't':
            is_muon_scatter = true;
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

    LSB = LSB * 0.15492 * 2.048 * 0.001;

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

    vector<string> file_name_path = split_string(input_file_name, "/");
    string save_path;
    string save_path_event;
    if (file_name_path[1].empty())
    {
        save_path = "./" + save_path_set;
        save_path_event = "./" + save_path_set + "/event";
    }
    else
    {
        save_path = file_name_path[1] + "/" + save_path_set;
        save_path_event = file_name_path[1] + "/" + save_path_set + "/event";
    }
    if (stat(save_path.c_str(), &info) == -1)
    {
        cout << cYELLOW << "Save path : " << save_path << " does not exist, create it." << cRESET << endl;
        fs::create_directories(save_path);
    }
    if (stat(save_path_event.c_str(), &info) == -1)
    {
        cout << cYELLOW << "Save path : " << save_path_event << " does not exist, create it." << cRESET << endl;
        fs::create_directories(save_path_event);
    }

    string relative_filename = file_name_path[0];
    vector<string> data_file_prefix = split_string(relative_filename, ".root");

    string save_path_file = save_path + "/";

    // x
    std::cout<<"is_muon_scatter: "<<is_muon_scatter<<std::endl;
    fit_pro fit_pro(input_file_name, alignment_filename, detector_hitrange_filename, layer_num, rmse_precision, idx_discontinuous, idx_disc, sigma, LSB, save_path_file, is_req_xy, is_write_txt, is_angle, angle, is_muon_scatter);

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
         << "Parameters:" << endl
         << "  -f [file name] : input file name" << endl
         << "  -O [path] : save path, default: fit_result" << endl
         << "  -w [write txt] : write txt file, default: false " << endl
         << "  -l [layer num] : layer number, default: 6" << endl
         << "  -r [rmse precision] : rmse precision, default: 0.5mm" << endl
         << "  -u [idx discontinuous] : idx discontinuous, default: false" << endl
         << "  -v [idx disc] : idx disc, default: 0" << endl
         << "  -a [alignment filename] : alignment filename, default: alignment.txt" << endl
         << "  -g [sigma] : sigma, default: 4.6mm" << endl
         << "  -L [LSB] : LSB, default: 15" << endl
         << "  -R [detector hitrange filename] : detector hitrange filename, default: detector_hitrange.txt" << endl
         << "  -s [is req xy] : is req xy, default: false" << endl
         << "  -A [is angle] : is angle, default: false" << endl
         << "  -G [angle] : angle, default: 5" << endl
         << "  -t [is muon scatter] : is muon scatter, default: false" << endl
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
