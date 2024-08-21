#include <iomanip>
#include "adas_data_unpack.hh"
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"

using namespace std;

int main(int argc, char **argv)
{
    //-f get file name
    string file_name;
    //-L
    int LSB = 0;
    //-s
    bool is_old_data = false;
    //-i
    int asic_num = 4;
    //-t
    bool is_write_txt = false;
    //-b
    bool is_base_data = false;
    //-O
    string save_path_set = "unpack_result";

    while ((c = getopt(argc, argv, "f:o:L:s:i:r:t:b:O:")) != EOF)
    {
        switch (c)
        {
        case 'f':
            file_name = optarg;
            break;
        case 'L':
            LSB = atoi(optarg);
            break;
        case 's':
            is_old_data = atoi(optarg);
            break;
        case 'i':
            asic_num = atoi(optarg);
            break;
        case 't':
            is_write_txt = atoi(optarg);
            break;
        case 'b':
            is_base_data = atoi(optarg);
            break;
        case 'O':
            save_path_set = optarg;
            break;
        case '?':
            cout << cRED;
            cout << "<<---------------------->>" << endl;
            cout << "[Error]: unknow parameters" << endl;
            cout << "<<---------------------->>" << endl;
            cout << "Or forget break in the last case?" << endl;
            show_info(argv[0]);
            return -1;
            break;
        default:
            break;
        }
    }

    if (file_name.empty() || LSB == 0)
    {
        cout << cRED;
        cout << "<<---------------------->>" << endl;
        cout << "[Error]: missing parameters" << endl;
        cout << "<<---------------------->>" << endl;
        show_info(argv[0]);
        return -1;
    }
    if (file_name.find(".dat") == string::npos)
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

    vector<string> file_name_path = split_string(file_name, "/");
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

    string relative_filename = file_name_path[0];
    vector<string> data_file_prefix = split_string(relative_filename, ".dat");

    string root_file_name = save_path + data_file_prefix[1];
    string initial_file_name = save_path + data_file_prefix[1] + "_" + "init.txt";
    string plot_file_name = save_path + data_file_prefix[1] + "_";

    string save_suffix;
    vector<string> file_name_suffix = split_string(data_file_prefix[1], "-");
    save_suffix = "link" + file_name_suffix[0];

    string file_write = save_path + "/" + "3sigma_vth_" + save_suffix + "_usb" + ".txt";

    double adc_LSB = LSB * 0.15492 * 2.048 * 0.001; //FIXME
    vector<uint32_t> initial_data;
    if (read_file(file_name, initial_data))
    {
      info_out("....");
        write_file(initial_file_name, initial_data);
     info_out("...");
        unpack_process unpack_process(initial_data, is_old_data, asic_num, root_file_name, is_write_txt, is_base_data, adc_LSB, plot_file_name, file_write);

        cout << endl;
        cout << cBLUE << "Done!" << cRESET << endl;
        cout << endl;
        return 0;
    }
    else
    {
        return -1;
    }
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
         << "    -f [First data filename. If single file, specific the filename] (required)" << endl
         << "    -L [LSB of the data. Default is 0](required)" << endl
         << "    -s [Is old data? 0 for new data, 1 for old data. Default is 0](required)" << endl
         << "    -i [Number of asic. Default is 4](required)" << endl
         << "    -t [Is write txt? 0 for no, 1 for yes. Default is 0](required)" << endl
         << "    -b [Is base data? 0 for no, 1 for yes. Default is 0](required)" << endl
         << "    -O [Save path. Default is result](required)" << endl
         << cRESET << endl;
}

bool read_file(string file_name, vector<uint32_t> &initial_data)
{
    ifstream file(file_name, ios::in | ios::binary);
    if (!file)
    {
        cout << cRED << "The file " << file_name << " is not exist!" << cRESET << endl;
        return false;
    }

    uint32_t value;
    uint16_t header;
    char buffer[4];
    while (file.read(reinterpret_cast<char *>(&header), sizeof(header))) // 读取16位二进制数
    {

        if (header == 0x0FAC) // 检查是否为AC0F
        {
            header = 0xAC0F;                  // 将AC0F存入容器
            file.read(buffer, 2);             // 读取下一个16位二进制数
            uint32_t value = (header << 16) | // 将AC0F和下一个16位二进制数拼接起来
                             (static_cast<unsigned char>(buffer[0]) << 8) |
                             static_cast<unsigned char>(buffer[1]);
            initial_data.push_back(value); // 将10进制数存入容器

            while (file.read(buffer, sizeof(buffer)))
            { // 读取32位二进制数
                value = (static_cast<unsigned char>(buffer[0]) << 24) |
                        (static_cast<unsigned char>(buffer[1]) << 16) |
                        (static_cast<unsigned char>(buffer[2]) << 8) |
                        static_cast<unsigned char>(buffer[3]); // 将二进制数转换为10进制数
                initial_data.push_back(value);                 // 将10进制数存入容器
            }
            break; // 读到文件末尾，跳出循环
        }
    }
    // cout << initial_data[0] << endl;

    // while (file.read(buffer, 4))
    // {
    //     value = (static_cast<unsigned char>(buffer[0]) << 24) |
    //             (static_cast<unsigned char>(buffer[1]) << 16) |
    //             (static_cast<unsigned char>(buffer[2]) << 8) |
    //             static_cast<unsigned char>(buffer[3]);
    //     initial_data.push_back(value);
    // }
    file.close();
    info_out(initial_data.size());

    std::ofstream fout("view0.txt");
    fout<<std::hex<<std::setw(8);
    for(size_t i=0; auto&& x : initial_data){
      if (i++%16==0) fout<<std::endl;
      fout<<x<<"\t";
    }
    fout.close();

    return true;
}

void write_file(string file_name, vector<uint32_t> &initial_data)
{
    ofstream file(file_name, ios::out);

    if (!file)
    {
        std::cerr << "Cannot open the file." << std::endl;
        return;
    }

    for (const auto &value : initial_data)
    {
        file << value << std::endl;
    }

    file.close();
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
