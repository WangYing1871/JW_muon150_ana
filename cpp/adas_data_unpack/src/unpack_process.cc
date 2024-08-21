#include <unordered_map>
#include <set>
#include <algorithm>
#include <numeric>
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include "unpack_process.hh"

#include "TPDF.h"
#include "TH1I.h"
#include "TF1.h"
#include "TFolder.h"
#include "TCanvas.h"
#include "TGraph.h"

using namespace std;
namespace util{

template <class _iter_t>
int to_pdf(_iter_t first, _iter_t last, std::string const& pdf_name){
  return 0;
}

template <class _iter_t>
int to_root(_iter_t first, _iter_t last,TFile* rfin, std::string const& folder_name){
  //auto* rfile = new TFile(root_name.c_str(),"recreate");
 // rfile->cd();
  rfin->cd();
  std::set<std::string> names_table;
  auto* folder = new TFolder(folder_name.c_str(),folder_name.c_str());

  std::map<size_t,std::pair<float,float>> baseline_record;

  for(auto iter = first; iter != last; ++iter){
    std::string name = std::to_string(iter->first);
    name += folder_name;
    while(names_table.find(name) != names_table.end()) name+="_1";
    names_table.insert(name);
    auto& array_ref = iter->second;
    auto max_ele = std::max_element(std::begin(array_ref),std::end(array_ref));
    auto min_ele = std::min_element(std::begin(array_ref),std::end(array_ref));

    auto range = *max_ele-*min_ele;
    auto* his = new TH1I(name.c_str(),name.c_str(),range/16,*min_ele,*max_ele);
    for (auto&& x : array_ref) his->Fill(x);
    float mean = his->GetMean();
    float rms = his->GetRMS();
    auto* f_gaus = new TF1("function0","gaus",mean-3*rms,mean+3*rms);
    his->Fit(f_gaus,"RQ");
    baseline_record[iter->first].first = f_gaus->GetParameter(1);
    baseline_record[iter->first].second = f_gaus->GetParameter(2);

    folder->Add(his);
  }
  auto* graph_mean = new TGraph(); graph_mean->SetName((folder_name+std::string("-mean")).c_str());
  auto* graph_rms = new TGraph(); graph_rms->SetName((folder_name+std::string("-rms")).c_str());
  for (std::size_t index = 0; auto&& [x,y] : baseline_record){
    graph_mean->SetPoint(index,x,y.first);
    graph_rms->SetPoint(index,x,y.second);
    index++; }
  folder->Add(graph_mean); folder->Add(graph_rms);



  info_out("...............");
  folder->Write();
  
  //rfile->Write(); rfile->Close();
  return 0;
}

}

unpack_process::unpack_process(vector<uint32_t> initial_data, bool is_old_data, int asic_num, string root_filename, bool is_write_txt, bool is_base_data, double LSB, string save_path, string file_write) : ch(asic_num), amp(asic_num)
{
    this->initial_data = initial_data;
    this->asic_num = asic_num;
    this->LSB = LSB;
    this->save_path = save_path;
    this->file_write = file_write;
    if (is_base_data)
    {
        init(is_old_data, is_base_data);

        std::unordered_map<int,std::vector<int>> chs_amp_map;


        auto const& viewer = [&,this](){
          for (int i=0; i<trig.size(); ++i){
            size_t hits_no = adas_chl[i].size();
            for(int j=0; j<hits_no; ++j){
              //info_out(adas_idx[i][j]);
              //if (adas_idx[i][j]==1)
                chs_amp_map[adas_chl[i][j]].emplace_back(adas_amp[i][j]);
            }
          }
    //for (int i = 0; i < trig.size(); i++)
    //{
    //    nhits = adas_chl[i].size();
    //    triggerid = trig[i];
    //    for (int j = 0; j < nhits; j++)
    //    {
    //        adas_hit_idx[j] = adas_idx[i][j];
    //        adas_hit_chl[j] = adas_chl[i][j];
    //        adas_hit_amp[j] = adas_amp[i][j];

    //        int channel_index = adas_hit_chl[j];
    //        int adc_amp = adas_hit_amp[j];
    //        channel_record_map[channel_index].emplace_back(adc_amp);
    //    }
    //    if (adas_hit_tree != nullptr)
    //    {
    //        adas_hit_tree->Fill();





    //    }
    //    else
    //    {
    //        cout << "adas_hit_tree is not initialized!" << std::endl;
    //    }
    //}
        };
        viewer();
        //util::to_root(std::begin(chs_amp_map),std::end(chs_amp_map),"temp.root");


    }
    else
    {
        init_root(root_filename);
        init(is_old_data, is_base_data);

        write_root(is_old_data);
        complete_adding();
        if (is_write_txt)
        {
            string txt_filename = root_filename + ".txt";
            ofstream file(txt_filename, ios::out);
            write_txt(txt_filename);
            file.close();
        }
    }
}

unpack_process::~unpack_process()
{
    if (adas_hit_file != nullptr && adas_hit_file->IsOpen())
    {
        adas_hit_file->Close();
        delete adas_hit_tree;
        delete adas_hit_file;
    }
}

void unpack_process::init(bool is_old_data, bool is_base_data)
{
    if (is_old_data)
    {
        int header = 4294945450; //0xFFFFAAAA
        int ender = 3722313727;   //DDDDFFFF

        // 第一步删除
        auto it = std::find(initial_data.begin(), initial_data.end(), header);

        if (it != initial_data.begin() && it != initial_data.end())
        {
            initial_data.erase(initial_data.begin(), it);
        }

        // 第二步
        for (auto it = initial_data.begin(); it != initial_data.end(); it++)
        {
            if (*it == header)
            {
                start_index.push_back(distance(initial_data.begin(), it));
            }
            if (*it == ender)
            {
                end_index.push_back(distance(initial_data.begin(), it));
            }
        }

        int N = end_index.size();
        if (is_base_data)
        {
            for (int i = 0; i < N; i++)
            {
                vector<uint32_t> temp;
                if (end_index[i] - start_index[i] == 539)
                {
                    for (int j = start_index[i]; j < end_index[i] + 1; j++)
                    {
                        temp.push_back(initial_data[j]);
                    }
                    adc_data.push_back(temp);
                }
                else
                {
                    cout << "lose read data" << endl;
                }
            }
        }
        else
        {
            for (int i = 0; i < N; i++)
            {
                vector<uint32_t> temp;
                if (end_index[i] - start_index[i] > 16)
                {
                    for (int j = start_index[i]; j < end_index[i] + 1; j++)
                    {
                        temp.push_back(initial_data[j]);
                    }
                    adc_data.push_back(temp);
                }
            }
        }

        for (int i = 0; i < adc_data.size(); i++)
        {
            trig.push_back(adc_data[i][1]);
        }

        vector<vector<vector<uint32_t>>> data(asic_num);

        for (int i = 0; i < asic_num; i++)
        {
            split_data(adc_data, h[i], e[i], data[i]);
        }

        for (int i = 0; i < asic_num; i++)
        {
            spilt_channel_amp(data[i], ch[i], amp[i], is_base_data, data[i]);
        }

        if (is_base_data)
        {
            vector<vector<vector<uint32_t>>> base(asic_num);
            vector<vector<double>> means(asic_num);
            vector<vector<double>> means128(asic_num);
            vector<vector<double>> rms(asic_num);
            vector<vector<double>> rms_bin(asic_num);
            vector<vector<double>> rms128(asic_num);
            vector<vector<double>> threshold(asic_num);

            for (int i = 0; i < asic_num; i++)
            {
                clac_base_rms(amp[i], base[i], means[i], rms[i], threshold[i], rms_bin[i], rms128[i], means128[i]);
            }

            vector<double> th;
            for (int i = 0; i < threshold.size(); i++)
            {
                for (int j = 0; j < threshold[i].size(); j++)
                {
                    th.push_back(threshold[i][j]);
                }
            }

            threshold_to_usb(th);
            draw(means, rms);
            write_base_txt(means128, rms128, threshold);
        }
    }
    else
    {
        find_start_end();

        // write_adc_data();
        cout << adc_data.size() << endl;

        for (int i = 0; i < adc_data.size(); i++)
        {

            vector<uint32_t> temp_data;
            vector<uint32_t> temp_idx;
            for (int j = 3; j < adc_data[i].size() - 1;)
            {
                if ((adc_data[i][j] & 4294901760) == 1717960704) // 0xFFFF0000 //66660000
                {
                    int cnt = adc_data[i][j] & 255;
                    if (is_base_data)
                    {
                        for (int k = 0; k < cnt; k++)
                        {
                            temp_idx.push_back((adc_data[i][j] & 65280) / 256);
                            temp_data.push_back(adc_data[i][j + 1 + k]);
                        }
                    }
                    else
                    {
                        for (int k = 1; k < cnt; k++)
                        {
                            temp_idx.push_back((adc_data[i][j] & 65280) / 256);
                            temp_data.push_back(adc_data[i][j + 1 + k]);
                        }
                    }

                    j = j + cnt + 1;
                }
            }
            if (temp_idx.size() == 0)
            {
                cout << cRED << "lose data ! the data idx is " << i << cRESET << endl;
                continue;
            }
            else
            {
                trig.push_back(adc_data[i][1]);
                dataidx.push_back(temp_idx);
                data.push_back(temp_data);
            }
        }

        spilt_channel_amp(data, adas_chl, adas_amp, is_base_data, adas_idx);

        cout << cBLUE << "channel amp spilt suceess" << cRESET << endl;
        cout << endl;

        if (is_base_data)
        {
          m_view_file = new TFile("view.root","recreate");
            vector<vector<vector<uint32_t>>> base(asic_num);
            vector<vector<double>> means(asic_num);
            vector<vector<double>> means128(asic_num);
            vector<vector<double>> rms(asic_num);
            vector<vector<double>> rms_bin(asic_num);
            vector<vector<double>> rms128(asic_num);
            vector<vector<double>> threshold(asic_num);

            for (int i = 0; i < adas_amp.size(); ++i)
            {
                for (int j = 0; j < adas_amp[i].size(); j += 132)
                {
                    vector<uint32_t> group;
                    double k = 0;
                    k = j / 132;
                    group.insert(group.end(), adas_amp[i].begin() + j, adas_amp[i].begin() + j + 132);
                    amp[k].push_back(group);
                }
            }

            for (int i = 0; i < asic_num; i++)
            {
              m_index = i;
                clac_base_rms(amp[i], base[i], means[i], rms[i], threshold[i], rms_bin[i], rms128[i], means128[i]);
            }


            vector<double> th;
            for (int i = 0; i < threshold.size(); i++)
            {
                for (int j = 0; j < threshold[i].size(); j++)
                {
                    th.push_back(threshold[i][j]);
                }
            }
            threshold_to_gbt(th);
            draw(means, rms);
            write_base_txt(means128, rms128, threshold);

            m_view_file->Write();
            m_view_file->Close();
        }
    }
}

void unpack_process::split_data(vector<vector<uint32_t>> data, uint32_t header, uint32_t ender, vector<vector<uint32_t>> &adc_data)
{
    for (int i = 0; i < data.size(); i++)
    {
        // 找到 header 和 ender 的位置
        int oneIndex = -1, fiveIndex = -1;
        for (int j = 0; j < data[i].size(); ++j)
        {
            if (data[i][j] == header)
                oneIndex = j;
            if (data[i][j] == ender)
                fiveIndex = j;
        }

        // 如果找到 header 和 ender，将 header 和 ender 之间的数划入元胞数组
        if (oneIndex != -1 && fiveIndex != -1)
        {
            vector<uint32_t> temp;
            temp.insert(temp.end(), data[i].begin() + oneIndex + 1, data[i].begin() + fiveIndex);
            adc_data.push_back(temp);
        }
    }
}

void unpack_process::spilt_channel_amp(vector<vector<uint32_t>> data, vector<vector<uint32_t>> &channel, vector<vector<uint32_t>> &amp, bool is_base_data, vector<vector<uint32_t>> &idx)
{
    for (int i = 0; i < data.size(); i++)
    {
        vector<uint32_t> temp_channel;
        vector<uint32_t> temp_amp;
        vector<uint32_t> temp_idx;
        if (is_base_data)
        {
            for (int j = 1; j < data[i].size(); j++)
            {
                int ch = 0;
                int amp = 0;
                amp = data[i][j] & 16777215;
                ch = (data[i][j] & 4278190080) / 16777216;
                temp_channel.push_back(ch);
                temp_amp.push_back(amp);
            }
        }
        else
        {
            for (int j = 0; j < data[i].size(); j++)
            {
                int ch = 0;
                int ampp = 0;
                int idxx = 0;
                ampp = data[i][j] & 16777215;
                ch = (data[i][j] & 4278190080) / 16777216;
                idxx = dataidx[i][j];
                if (i==0) info_out(ch);
                if (ch <= 128)
                {
                    temp_channel.push_back(ch);
                    temp_amp.push_back(ampp);
                    temp_idx.push_back(idxx);
                }
            }
        }
        channel.push_back(temp_channel);
        amp.push_back(temp_amp);
        idx.push_back(temp_idx);
    }
}

void unpack_process::write_root(bool is_old_data)
{

  std::unordered_map<int,std::vector<int>> channel_record_map;

    if (is_old_data)
    {
        for (int i = 0; i < trig.size(); i++)
        {
            vector<uint32_t> temp_ch;
            vector<uint32_t> temp_amp;
            vector<uint32_t> temp_idx;

            for (int j = 0; j < asic_num; j++)
            {
                if (ch[j][i].size() != 1)
                {
                    for (int k = 1; k < ch[j][i].size(); k++)
                    {
                        temp_amp.push_back(amp[j][i][k]);
                        temp_ch.push_back(ch[j][i][k]);
                        temp_idx.push_back(j);
                    }
                }
            }

            adas_chl.push_back(temp_ch);
            adas_amp.push_back(temp_amp);
            adas_idx.push_back(temp_idx);
        }
    }
    info_out(trig.size());
    info_out(adas_idx.size());

    if (trig.size() != adas_idx.size())
    {
        cout << "error read data" << endl;
    }

    for (int i = 0; i < trig.size(); i++)
    {
        nhits = adas_chl[i].size();
        if (i==0) info_out(nhits);
        triggerid = trig[i];
        for (int j = 0; j < nhits; j++)
        {
            adas_hit_idx[j] = adas_idx[i][j];
            adas_hit_chl[j] = adas_chl[i][j];
            adas_hit_amp[j] = adas_amp[i][j];

            int channel_index = adas_hit_chl[j];
            int adc_amp = adas_hit_amp[j];
            channel_record_map[channel_index].emplace_back(adc_amp);
        }
        if (adas_hit_tree != nullptr)
        {
            adas_hit_tree->Fill();





        }
        else
        {
            cout << "adas_hit_tree is not initialized!" << std::endl;
        }
    }

    std::ofstream fout("view1.txt");
    for (auto&& [x,y] : channel_record_map){
      fout<<x<<"\n";
      for (auto&& y0 : y) fout<<"  "<<y0<<"\n";
      fout<<std::flush;
    }

    fout.close();
}

bool unpack_process::init_root(string root_filename)
{
    string filename = root_filename + ".root";
    adas_hit_file = new TFile(filename.c_str(), "recreate");
    adas_hit_tree = new TTree("adas_initial_data", "ADAS Hit");

    adas_hit_tree->Branch("nhits", &nhits, "nhits/I");
    adas_hit_tree->Branch("triggerid", &triggerid, "triggerid/I");
    adas_hit_tree->Branch("adas_hit_idx", adas_hit_idx, "adas_hit_idx[nhits]/I");
    adas_hit_tree->Branch("adas_hit_chl", adas_hit_chl, "adas_hit_chl[nhits]/I");
    adas_hit_tree->Branch("adas_hit_amp", adas_hit_amp, "adas_hit_amp[nhits]/I");

    // cout << cCYAN << "TTree in TFile " << cMAGENTA << filename << cCYAN << " init done" << cRESET << endl;
    cout << cGREEN << filename << cRESET << endl;
    cout << endl;

    return true;
}

void unpack_process::complete_adding()
{
    if (adas_hit_tree != nullptr)
    {
        adas_hit_file->cd();
        adas_hit_tree->Write();
        adas_hit_file->Flush();
        adas_hit_file->Close();
    }
    else
    {
        cout << "adas_hit_tree is not initialized!" << std::endl;
    }
}

void unpack_process::find_start_end()
{
    vector<uint32_t> data;
    data = initial_data;
    int N = data.size() - 1;

    int header = 44047; // 0xAC0F
    int ender = 1077438054; //40386666
    
    int i = 0;
    while (N > 0 || i > data.size() - 1)
    {
        int dec = data[i];
        bitset<32> bits(dec);
        bitset<16> hig16(bits.to_string().substr(0, 16));
        bitset<14> low14(bits.to_string().substr(32 - 14, 14));
        int msb = hig16.to_ulong();
        int lsb = low14.to_ulong();


        vector<uint32_t> temp;
        if (msb == header)
        {
          //if ((i+2)<data.size()
          //    && (i+data[i+2]-1)<=data.size()
          //    && data[i+2]==(lsb/4+2)
          //    && data[i+data[data+2]-1]==ender){
          //}else if(){
          //}else{
          //}
            if (((i + 2) < data.size()) && ((i + data[i + 2] - 1) <= data.size()) && (data[i + 2] == (lsb / 4 + 2)) && (data[i + data[i + 2] - 1] == ender))
            {
                if (data[i + 2] > 802 || data[i + 2] == 0)
                {
                    cout << cRED << "lose data ! the data idx is " << i << " " << N << cRESET << endl;
                    cout << endl;
                    i++;
                    N--;
                    continue;
                }
                start_index.push_back(i);
                end_index.push_back(i + data[i + 2] - 1);
                temp.insert(temp.end(), data.begin() + i, data.begin() + i + data[i + 2]);



                adc_data.push_back(temp);
                int dec = data[i + 2];
                i = i + dec;
                N = N - dec;
            }
            else if (((i + 2) >= data.size()) || ((i + data[i + 2] - 1) > data.size()))
            {

                break;
            }
            else
            {
                if (((i + 2) != data.size()) || ((i + data[i + 2] - 1) != data.size()) || (data[i + data[i + 2] - 1] != ender))
                {
                    i++;
                    N--;
                }
            }
        }
        else
        {
            i++;
            N--;
        }
    }
    cout << cBLUE << "fec data spilt suceess" << cRESET << endl;
    cout << endl;
}

void unpack_process::write_txt(string filename)
{
    ofstream file(filename, ios::out);
    for (int i = 0; i < adas_chl.size(); i++)
    {
        file << trig[i] << " ";
        for (int j = 0; j < adas_chl[i].size(); j++)
        {
            file << adas_idx[i][j] << " " << adas_chl[i][j] << " " << adas_amp[i][j] << " ";
        }
        file << endl;
    }
    file.close();
}

void unpack_process::clac_base_rms(
    vector<vector<uint32_t>> data
    , vector<vector<uint32_t>> &base
    , vector<double> &means
    , vector<double> &rms
    , vector<double> &threshold
    , vector<double> &rms_bin
    , vector<double> &rms128
    , vector<double> &means128)
{

  //for (auto&& x : data) info_out(x.size());

    for (int i = 0; i < data.size(); i++)
    {
        vector<uint32_t> temp_base;
        for (int j = 0; j < 128; j++)
        {
            temp_base.push_back(data[i][j]);
        }
        base.push_back(temp_base);
    }

    int rows = base.size();
    int cols = base[0].size();

    std::unordered_map<uint32_t,std::vector<uint32_t>> map00;
    for(int i=0; i<128; ++i){
      for (int j=0; j<rows; ++j)
        map00[i].emplace_back(base[j][i]);
    }

    std::string folder_name = "folder-"+std::to_string(m_index);
    util::to_root(std::begin(map00),std::end(map00),m_view_file,folder_name);




    for (int i = 0; i < cols; i++)
    {
        double sum = 0;
        for (int j = 0; j < rows; j++)
        {
            sum += base[j][i];
        }
        means.push_back(sum / rows);
    }

    vector<double> temp;

    for (int i = 0; i < cols; i++)
    {
        double sum = 0;
        for (int j = 0; j < rows; j++)
        {
            sum += pow(base[j][i] - means[i], 2);
        }
        temp.push_back(sqrt(sum / rows));
    }

    for (int i = 0; i < cols; i++)
    {
        rms.push_back(temp[i] * LSB);
    }

    means128.resize(128);
    rms128.resize(128);

    for (int i = 0; i < cols; i++)
    {
        means128[i] = means[i];
        rms128[i] = rms[i];
    }

    for (int i = 0; i < means.size(); i++)
    {
        int th = 0;
        th = 3 * temp[i];
        threshold.push_back(th);
    }

    for (int i = 0; i < cols; i++)
    {
        rms_bin.push_back(temp[i]);
    }

    // 需要注意的是adas1128输出数据后64个通道是从128-65，并不是65-128，所以在画图的时候需要将64个通道的数据反过来；但是填入阈值的时候是不需要翻转的
    // mean和rms为了画图，写入txt的为mean128，rms128，threshold
    int n = 0;
    for (int i = 0; i < means.size(); i++)
    {
        if (i > 63 && n <= 32)
        {
            int t = 0;
            double r = 0;
            t = means[i];
            means[i] = means[191 - i];
            means[191 - i] = t;
            r = rms[i];
            rms[i] = rms[191 - i];
            rms[191 - i] = r;
            n++;
        }
    }
}

void unpack_process::threshold_to_usb(vector<double> threshold)
{
    vector<double> th;

    th = threshold;

    for (int i = 128; i < th.size() + (asic_num * 3); i += 128 + 3)
    {

        th.insert(th.begin() + i, 3, 0);
    }

    ostringstream datamsb;
    ostringstream datalsb;

    long DataMSB = 0x0208;
    long DataLSB = 0x0209;
    string datamsb_str;
    string datalsb_str;
    string wr_str;

    string bin_str;
    string bin_res;
    string bin_addr;
    string m_str;
    string l_str;

    string filename = save_path + "th.txt";
    ofstream output_file(filename, ios::out | ios::trunc);

    for (int i = 0; i < asic_num; i++)
    {
        for (int j = 0; j < 131; j++)
        {

            bitset<24> bits((int)(th[j + i * 131]));
            bitset<8> bits3(j);
            bin_str = bits.to_string();
            bin_addr = bits3.to_string();
            bin_res = bin_addr + bin_str;

            m_str = bin_res.substr(0, 16);
            l_str = bin_res.substr(16, 16);

            bitset<16> bits1(m_str);
            int msb = bits1.to_ulong();

            bitset<16> bits2(l_str);
            int lsb = bits2.to_ulong();

            datamsb << "0x" << setfill('0') << setw(4) << hex << DataMSB << setw(4) << hex << msb;
            datamsb_str = datamsb.str();

            datalsb << "0x" << setfill('0') << setw(4) << hex << DataLSB << setw(4) << hex << lsb;
            datalsb_str = datalsb.str();

            wr_str = "0x020" + to_string(i + 4) + "0001";
            output_file << datamsb_str << endl;
            output_file << datalsb_str << endl;
            output_file << wr_str << endl;
            datamsb.str("");
            datalsb.str("");
        }
    }
    output_file.close();

    cout << cBLUE << "sucess output " << filename << endl;
    cout << endl;
}

void unpack_process::threshold_to_usb_new(vector<double> threshold)
{
    vector<double> th;

    th = threshold;

    for (int i = 128; i < th.size() + (asic_num * 3); i += 128 + 3)
    {

        th.insert(th.begin() + i, 3, 0);
    }

    ostringstream datamsb;
    ostringstream datalsb;

    long DataMSB = 0x0200;
    long DataLSB = 0x0201;
    string datamsb_str;
    string datalsb_str;
    string wr_str;

    string bin_str;
    string bin_res;
    string bin_addr;
    string m_str;
    string l_str;

    string filename = file_write;
    ofstream output_file(filename, ios::out | ios::trunc);

    for (int i = 0; i < asic_num; i++)
    {
        for (int j = 0; j < 131; j++)
        {

            bitset<24> bits((int)(th[j + i * 131]));
            bitset<8> bits3(j + 1);
            bin_str = bits.to_string();
            bin_addr = bits3.to_string();
            bin_res = bin_addr + bin_str;

            m_str = bin_res.substr(0, 16);
            l_str = bin_res.substr(16, 16);

            bitset<16> bits1(m_str);
            int msb = bits1.to_ulong();

            bitset<16> bits2(l_str);
            int lsb = bits2.to_ulong();

            datamsb << "0x" << setfill('0') << setw(4) << hex << DataMSB << setw(4) << hex << msb;
            datamsb_str = datamsb.str();

            datalsb << "0x" << setfill('0') << setw(4) << hex << DataLSB << setw(4) << hex << lsb;
            datalsb_str = datalsb.str();

            int wren = i + 10;
            stringstream ss;
            ss << hex << wren;
            string idx = ss.str();

            wr_str = "0x020" + idx + "0001";
            output_file << datamsb_str << endl;
            output_file << datalsb_str << endl;
            output_file << wr_str << endl;
            datamsb.str("");
            datalsb.str("");
        }
    }
    output_file.close();

    cout << cYELLOW << "threshold txt sucess output " << filename << endl;
    cout << endl;
}

void unpack_process::threshold_to_gbt(vector<double> threshold)
{
    vector<double> th;

    th = threshold;

    for (int i = 128; i < th.size() + (asic_num * 3); i += 128 + 3)
    {

        th.insert(th.begin() + i, 3, 0);
    }

    long DataMSB = 0x0101;
    long DataLSB = 0x0102;
    string enable = "0x01020001";

    string wait = "&1";

    ostringstream datamsb;
    ostringstream datalsb;

    string bin_str;
    string bin_res;
    string bin_addr;
    string m_str;
    string l_str;
    string datamsb_str;
    string datalsb_str;

    string filename = file_write;
    ofstream output_file(filename, ios::out | ios::trunc);

    for (int i = 0; i < asic_num; i++)
    {

        for (int j = 0; j < 131; j++)
        {

            for (int k = 0; k < 2; k++)
            {

                string wr_msb_en = "0x0101400" + to_string(k);

                output_file << wr_msb_en << endl;
                output_file << enable << endl;

                bitset<24> bits((int)(th[j + i * 131]));
                bitset<8> bits3(j + 1);
                bin_str = bits.to_string();
                bin_addr = bits3.to_string();
                bin_res = bin_addr + bin_str;

                m_str = bin_res.substr(0, 16);
                l_str = bin_res.substr(16, 16);

                bitset<16> bits1(m_str);
                int msb = bits1.to_ulong();

                bitset<16> bits2(l_str);
                int lsb = bits2.to_ulong();

                datamsb << "0x" << setfill('0') << setw(4) << hex << DataMSB << setw(4) << hex << msb;
                datamsb_str = datamsb.str();

                datalsb << "0x" << setfill('0') << setw(4) << hex << DataMSB << setw(4) << hex << lsb;
                datalsb_str = datalsb.str();

                if (k == 0)
                {
                    output_file << datamsb_str << endl;
                }
                else
                {
                    output_file << datalsb_str << endl;
                }

                output_file << enable << endl;
                output_file << wait << endl;
                datamsb.str("");
                datalsb.str("");
            }
            output_file << "0x01014012" << endl;
            output_file << enable << endl;
            output_file << "0x01010000" << endl;
            output_file << enable << endl;
            output_file << wait << endl;

            // WREN EN
            output_file << "0x01015000" << endl;
            output_file << enable << endl;

            int en = 1;
            int wren = 1 << i;
            wren = wren & 0xffff;
            stringstream ss;
            ss << setw(4) << setfill('0') << hex << wren;
            string idx = ss.str();
            string ram_wren = "0x0101" + idx;
            output_file << ram_wren << endl;

            output_file << enable << endl;
            if (!(i == asic_num - 1 && j == 130))
            {
                output_file << wait << endl;
            }
        }
    }
    output_file.close();

    cout << cYELLOW << "threshold txt sucess output " << filename << endl;
    cout << endl;
}

void unpack_process::draw(vector<vector<double>> means, vector<vector<double>> rms)
{
    string filename = "base";
    gStyle->SetImageScaling(3.);
    TCanvas *c1 = new TCanvas(filename.c_str(), filename.c_str());
    // 将画布分割成1行2列
    c1->Divide(2, 3);

    for (int i = 0; i < asic_num; i++)
    {
        TGraph *gr = new TGraph(means[i].size());
        for (int j = 0; j < means[i].size(); j++)
        {
            gr->SetPoint(j, j, means[i][j]);
        }

        c1->cd(i + 1); // 切换到第一个子画布
        gPad->SetGrid();
        string title = "Chip" + to_string(i);
        gr->SetTitle(title.c_str());
        gr->SetMarkerColor(4);
        gr->SetMarkerStyle(21);
        gr->SetMarkerSize(0.5);
        gr->Draw("APL");
        gr->GetXaxis()->SetRangeUser(0, 127);
    }
    string save_filename = save_path + "base.png";
    c1->SaveAs(save_filename.c_str());

    // rms
    string filename1 = "rms";
    gStyle->SetImageScaling(3.);
    TCanvas *c2 = new TCanvas(filename1.c_str(), filename1.c_str());
    c2->Divide(2, 3);

    for (int i = 0; i < asic_num; i++)
    {
        TGraph *gr = new TGraph(rms[i].size());
        for (int j = 0; j < rms[i].size(); j++)
        {
            gr->SetPoint(j, j, rms[i][j]);
        }

        c2->cd(i + 1); // 切换到第一个子画布
        gPad->SetGrid();
        string title = "Chip" + to_string(i);
        gr->SetTitle(title.c_str());
        gr->SetMarkerColor(4);
        gr->SetMarkerStyle(21);
        gr->SetMarkerSize(0.5);
        gr->Draw("APL");
        gr->GetXaxis()->SetRangeUser(0, 127);
    }
    string save_filename1 = save_path + "rms.png";
    c2->SaveAs(save_filename1.c_str());
}

void unpack_process::write_base_txt(vector<vector<double>> means, vector<vector<double>> rms, vector<vector<double>> threshold)
{

    string filename = save_path + "base.txt";
    ofstream file(filename, ios::out | ios::trunc);

    for (int i = 0; i < means[0].size(); i++)
    {

        for (int j = 0; j < means.size(); j++)
        {
            file << means[j][i] << " ";
        }
        file << endl;
    }
    file.close();

    string filename1 = save_path + "rms.txt";
    ofstream file1(filename1, ios::out | ios::trunc);
    for (int i = 0; i < rms[0].size(); i++)
    {

        for (int j = 0; j < rms.size(); j++)
        {
            file1 << rms[j][i] << " ";
        }
        file1 << endl;
    }
}

void unpack_process::write_adc_data()
{
    string filename = save_path + "adcdata.txt";
    ofstream adcdata(filename, ios::out);
    for (int i = 0; i < adc_data.size(); i++)
    {
        for (int j = 0; j < adc_data[i].size(); j++)
        {
            adcdata << adc_data[i][j] << " ";
        }
        adcdata << endl;
    }
}
