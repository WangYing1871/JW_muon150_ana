#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include <boost/type_index.hpp>
#define type_name(x) std::cout<<std::dec<<boost::typeindex::type_id<decltype(x)>().pretty_name() \
  <<" sizeof:"<<sizeof(decltype(x)) \
  <<" alignof:"<<alignof(decltype(x))<<std::endl;
#include "dec_pro.hh"

using namespace std;

dec_pro::dec_pro(
    string base_txt
    ,string rms_txt
    ,string input_file_name
    ,string root_file_name
    ,string save_path
    ,bool is_old_data
    ,bool is_write_txt
    ,int asic_num
    ,string encoding_list_filename
    ,bool is_allow_dec_discon, double LSB):
  base(asic_num), noise(asic_num), idx_dec(2), chl_dec(2), amp_dec(2) {
    this->asic_num = asic_num;
    this->save_path = save_path;
    this->root_filename = root_file_name;
    this->is_write_txt = is_write_txt;
    this->LSB = LSB;

    dec_task_cg = new hit_dec(encoding_list_filename, 40);
    dec_task_cg->is_allow_discontinue = is_allow_dec_discon;

    if (dec_task_cg->is_list_get)
    {
        log_file << "Encoding lists " << encoding_list_filename << " get successfully" << endl;
    }
    else
    {
        log_file << "[Error]: Encoding lists " << encoding_list_filename << " get failed!" << endl;
        return;
    }

    if (!read_base(base_txt))
    {
        return;
    }
    if (!read_noise(rms_txt))
    {
        return;
    }
    // write_base();
    if (read_root(input_file_name))
    {
        init_root();


        dec_base();
        data_dec_process(is_old_data);
        complete_adding();
    }
    else
    {
        return;
    }
}

dec_pro::~dec_pro()
{
    if (adas_dec_file != nullptr && adas_dec_file->IsOpen())
    {
        adas_dec_file->Close();
        delete adas_dec_tree;
        delete adas_dec_file;
    }
}

bool dec_pro::read_base(string base_txt)
{
    ifstream base_file(base_txt);
    string line;
    if (!base_file.is_open())
    {
        cout << cRED << "Failed to open file: " << base_txt << endl;
        return false;
    }
    while (getline(base_file, line))
    {
        istringstream iss(line);
        vector<string> line_split;
        string s;
        while (getline(iss, s, ' '))
        {
            line_split.push_back(s);
        }

        vector<double> line_split_double;
        for (int i = 0; i < line_split.size(); i++)
        {
            line_split_double.push_back(stod(line_split[i]));
        }

        for (int i = 0; i < line_split_double.size(); i++)
        {
            base[i].push_back(line_split_double[i]);
        }
    }
    base_file.close();
    cout << cBLUE << "Read base done!" << cRESET << endl;
    return true;
}

bool dec_pro::read_noise(string noise_txt)
{
    ifstream noise_file(noise_txt);
    string line;
    if (!noise_file.is_open())
    {
        cout << cRED << "Failed to open file: " << noise_txt << endl;
        return false;
    }
    while (getline(noise_file, line))
    {
        istringstream iss(line);
        vector<string> line_split;
        string s;
        while (getline(iss, s, ' '))
        {
            line_split.push_back(s);
        }

        vector<double> line_split_double;
        for (int i = 0; i < line_split.size(); i++)
        {
            line_split_double.push_back(stod(line_split[i]));
        }

        for (int i = 0; i < line_split_double.size(); i++)
        {
            noise[i].push_back(line_split_double[i]);
        }
    }
    noise_file.close();
    cout << cBLUE << "Read noise done!" << cRESET << endl;
    return true;
}

void dec_pro::write_base()
{
    string filename = save_path + "out.txt";
    ofstream base_file(filename, ios::out | ios::trunc);
    for (int i = 0; i < base.size(); i++)
    {
        for (int j = 0; j < base[i].size(); j++)
        {
            base_file << base[i][j] << " ";
        }
        base_file << endl;
    }
    base_file.close();
}

bool dec_pro::read_root(string input_file_name)
{
    TFile *input_file = new TFile(input_file_name.c_str());
    if (input_file->IsZombie() || !input_file->IsOpen())
    {
        cout << cRED << "File" << input_file_name << " open failed. Please check the file" << cRESET << endl;
        return false;
    }
    TTree *input_tree = (TTree *)input_file->Get("adas_initial_data");
    input_tree->SetBranchAddress("nhits", &anhits);
    input_tree->SetBranchAddress("triggerid", &triggerid);
    input_tree->SetBranchAddress("adas_hit_idx", &adas_hit_idx);
    input_tree->SetBranchAddress("adas_hit_chl", &adas_hit_chl);
    input_tree->SetBranchAddress("adas_hit_amp", &adas_hit_amp);

    int nentries = input_tree->GetEntries();

    for (int i = 0; i < nentries; i++)
    {
        input_tree->GetEntry(i);
        vector<int> temp_idx;
        vector<int> temp_chl;
        vector<int> temp_amp;

        bool is_find_idx0 = false;
        bool is_find_idx1 = false;

        for (int j = 0; j < anhits; j++)
        {

            if (adas_hit_idx[j] < (asic_num / 2))
            {
                is_find_idx0 = true;
            }

            if (adas_hit_idx[j] >= (asic_num / 2))
            {
                is_find_idx1 = true;
            }
        }
        if (!(is_find_idx0 && is_find_idx1))
        {
            continue;
        }

        for (int j = 0; j < anhits; j++)
        {
            if (adas_hit_chl[j] > 128)
            {
                continue;
            }
            // if (abs((adas_hit_amp[j] - base[adas_hit_idx[j]][adas_hit_chl[j]-1]) * LSB) > (5 * noise[adas_hit_idx[j]][adas_hit_chl[j]-1]))
            // {
            //     temp_idx.push_back(adas_hit_idx[j]);
            //     temp_chl.push_back(adas_hit_chl[j] - 1);
            //     temp_amp.push_back(adas_hit_amp[j]);
            // }
            temp_idx.push_back(adas_hit_idx[j]);
            temp_chl.push_back(adas_hit_chl[j] - 1);
            temp_amp.push_back(adas_hit_amp[j]);
        }
        if (temp_idx.size() != 0)
        {
            trig.push_back(triggerid);
            idx.push_back(temp_idx);
            chl.push_back(temp_chl);
            amp.push_back(temp_amp);
        }
    }
    input_file->Close();
    cout << cBLUE << "Read root file done!" << cRESET << endl;
    cout << endl;
    return true;
}

void dec_pro::dec_base()
{
    for (int i = 0; i < idx.size(); i++)
    {
        for (int j = 0; j < idx[i].size(); j++)
        {
            amp[i][j] = amp[i][j] - base[idx[i][j]][chl[i][j]];
        }
    }

    cout << cBLUE << "Dec base done!" << cRESET << endl;
    cout << endl;
}

void dec_pro::data_dec_process(bool is_old_data)
{
    if (!is_encode_384)
    {
      info_out("!is_encode_384");
        direct_read();
        data_dec();
    }

    if (is_encode_384)
    {
      info_out("is_encode_384");
        int event_num = trig.size();

        for (int i = 0; i < event_num; i++)
        {
            nhits = 0;
            cluster_number = 0;
            for (int j = 0; j < asic_num; j++)
            {
                mux_read(j);
                hit_data_dec(j, i);
                v_clear();
            }

            bool is_qx = all_of(is_qx_dec.begin(), is_qx_dec.end(), [](bool i)
                                { return i == 1; });
            bool is_qy = all_of(is_qy_dec.begin(), is_qy_dec.end(), [](bool i)
                                { return i == 1; });

            if (nhits != 0 && cluster_number != 0 && is_qx && is_qy)
            {
                trigger_id = trig[i];

                adas_dec_tree->Fill();
                if (is_write_txt_file)
                {
                    write_txt();
                }
            }
            is_qx_dec.clear();
            is_qy_dec.clear();
        }

        cout << cBLUE << "Data dec done!" << cRESET << endl;
        cout << endl;
    }
}

void dec_pro::data_dec()
{
    // 每次运行前清空nhits 和 cluser_number，以免后续程序判断错误
    nhits = 0;
    cluster_number = 0;
    // Clear the data stored in root for not confusing while in TBrowser()
    for (int i = 0; i < cMAX_DEC_HITS; i++)
    {
        hit_strips[i] = 0;
        hit_asic_chn[i] = 0;
        hit_amp[i] = 0;
        hit_time[i] = 0;
        hit_max_position[i] = 0;
        dimension_idx[i] = 0;
        cluster_size[i] = 0;
        cluster_holed_num[i] = 0;
    }

    int event_num = trig.size();
    info_out(event_num);

    for (int i = 0; i < event_num; i++)
    {
        nhits = 0;
        cluster_number = 0;
        bool is_qx = 0;
        bool is_qy = 0;
        int tem = 0;
        for (int j = 0; j < 2; j++)
        {
            int dec_strip_cnt = 0;
            if (j != 0)
            {
                for (auto &element : idx_dec[j][i])
                {
                    element = 0;
                }
            }
            
            if (dec_task_cg->run_dec(chl_dec[j][i], idx_dec[j][i]))
            {
                for (int k = 0; k < dec_task_cg->cluster_num.size(); k++)
                {
                    if (((dec_task_cg->cluster_num[k] - dec_task_cg->cluster_holed[k]) == 1))
                    {
                        dec_strip_cnt += dec_task_cg->cluster_num[k];
                        continue;
                    }
                    

                    info_out(dec_task_cg->cluster_num[k]);
                    info_out(j);
                    for (int z = 0; z < dec_task_cg->cluster_num[k]; z++)
                    {
                        hit_strips[nhits] = dec_task_cg->hit_strip[z + dec_strip_cnt];
                        hit_asic_chn[nhits] = chl_dec[j][i][dec_task_cg->hit_seq[z + dec_strip_cnt]];
                        hit_amp[nhits] = amp_dec[j][i][dec_task_cg->hit_seq[z + dec_strip_cnt]];
                        info_out(hit_amp[nhits]);
                        hit_time[nhits] = 0;
                        hit_max_position[nhits] = 0;
                        dimension_idx[nhits] = j;
                        nhits++;
                    }
                    cluster_size[cluster_number] = dec_task_cg->cluster_num[k];
                    cluster_holed_num[cluster_number] = dec_task_cg->cluster_holed[k];
                    cluster_number++;
                    dec_strip_cnt += dec_task_cg->cluster_num[k];
                }
            }
            if (j == 0)
            {
                if (nhits != 0)
                {
                    is_qx = 1;
                    tem = nhits;
                }
            }
            else
            {
                if (nhits != 0 && nhits > tem)
                {
                    is_qy = 1;
                }
            }
        }
        //info_out(trig[i]);
        //if (trig[i]==87){
        //  info_out(nhits);
        //  info_out(cluster_number);
        //}
        if (nhits != 0 && cluster_number != 0 && (is_qx || is_qy))
        {
          if (trig[i]<100) info_out(trig[i]);
            trigger_id = trig[i];
            adas_dec_tree->Fill();
            if (is_write_txt_file)
            {
                write_txt();
            }
        }
    }

    cout << cBLUE << "Data dec done!" << cRESET << endl;
    cout << endl;
}

bool dec_pro::init_root()
{
    string filename = root_filename + ".root";
    adas_dec_file = new TFile(filename.c_str(), "recreate");
    adas_dec_tree = new TTree("asic_hit_data", "FEC Hit");

    adas_dec_tree->Branch("nhits", &nhits, "nhits/I");
    adas_dec_tree->Branch("trigger_id", &trigger_id, "trigger_id/I");
    adas_dec_tree->Branch("dimension_idx", dimension_idx, "dimension_idx[nhits]/I");
    adas_dec_tree->Branch("hit_strips", hit_strips, "hit_strips[nhits]/I");
    adas_dec_tree->Branch("hit_asic_chn", hit_asic_chn, "hit_asic_chn[nhits]/I");
    adas_dec_tree->Branch("hit_time", hit_time, "hit_time[nhits]/I");
    adas_dec_tree->Branch("hit_max_position", hit_max_position, "hit_max_position[nhits]/I");
    adas_dec_tree->Branch("hit_amp", hit_amp, "hit_amp[nhits]/I");
    adas_dec_tree->Branch("cluster_number", &cluster_number, "cluster_number/I");
    adas_dec_tree->Branch("cluster_holed_num", cluster_holed_num, "cluster_holed_num[cluster_number]/I");
    adas_dec_tree->Branch("cluster_size", cluster_size, "cluster_size[cluster_number]/I");

    cout << cCYAN << "TTree in TFile " << cMAGENTA << filename << cCYAN << " init done" << cRESET << endl;
    cout << cGREEN << filename << cRESET << endl;
    cout << endl;

    if (is_write_txt)
    {
        txt_filename = root_filename + ".txt";
        txt_file.open(txt_filename, ios::out | ios::trunc);
        if (txt_file.is_open())
        {
            cout << cCYAN << "Save hit position txt file to " << cMAGENTA << txt_filename << cRESET << endl;
            is_write_txt_file = true;
        }
        else
        {
            cout << cRED << "Cannot create file: " << txt_filename << cRESET << endl;
            is_write_txt_file = false;
        }
    }
    return true;
}

void dec_pro::write_txt()
{
    txt_file.open(txt_filename, ios::app);
    txt_file << "{" << endl;
    txt_file << "trigger_id: " << trigger_id << endl;
    txt_file << "nhits: " << nhits << endl;
    int i = 0;
    for (int k = 0; k < cluster_number; k++)
    {
        txt_file << "-------- Cluster " << k << "--------" << endl;
        int base = i;
        txt_file << "    cluster size:\t" << cluster_size[k] << endl;
        txt_file << "    dimension_idx:\t" << dimension_idx[base] << endl;
        txt_file << "    hit_strips:\t\t";
        for (int j = 0; j < cluster_size[k]; j++)
        {
            txt_file << hit_strips[j + base];
            if (j != cluster_size[k] - 1)
            {
                txt_file << ","
                         << "\t";
            }
            i++;
        }
        txt_file << endl;
        txt_file << "    hit_amp:\t";
        for (int j = 0; j < cluster_size[k]; j++)
        {
            txt_file << hit_amp[j + base];
            if (j != cluster_size[k] - 1)
            {
                txt_file << ","
                         << "\t";
            }
        }
        txt_file << endl;
    }
    txt_file << "}" << endl;
    txt_file.close();
    // cout << cCYAN << "Write txt done!" << cRESET << endl;
    // cout << endl;
}

void dec_pro::complete_adding()
{
    if (adas_dec_tree != nullptr)
    {
        adas_dec_file->cd();
        adas_dec_tree->Write();
        adas_dec_file->Flush();
        adas_dec_file->Close();
    }
    else
    {
        cout << "adas_dec_tree is not initialized!" << std::endl;
    }
}

void dec_pro::direct_read()
{

    //cout << cBLUE << "success" << cRESET << endl;
    for (int i = 0; i < idx.size(); i++)
    {
        for (int j = 0; j < idx[i].size(); j++)
        {
            if (idx[i][j] < (asic_num / 2))
            {

                chl[i][j] = chl[i][j] + (idx[i][j] * 128);
                idx[i][j] = 0;
            }
            if (idx[i][j] >= (asic_num / 2))
            {

                chl[i][j] = chl[i][j] + ((idx[i][j] - (asic_num / 2)) * 128);
                idx[i][j] = 1;
            }
        }
    }

    for (int i = 0; i < idx.size(); i++)
    {
        vector<int> idx_0;
        vector<int> idx_1;
        vector<int> chl_0;
        vector<int> chl_1;
        vector<int> amp_0;
        vector<int> amp_1;

        for (int j = 0; j < idx[i].size(); j++)
        {
            if (idx[i][j] == 0)
            {
                idx_0.push_back(idx[i][j]);
                chl_0.push_back(chl[i][j]);
                amp_0.push_back(amp[i][j]);
            }
            else if (idx[i][j] == 1)
            {
                idx_1.push_back(idx[i][j]);
                chl_1.push_back(chl[i][j]);
                amp_1.push_back(amp[i][j]);
            }
        }
        idx_dec[0].push_back(idx_0);
        idx_dec[1].push_back(idx_1);
        chl_dec[0].push_back(chl_0);
        chl_dec[1].push_back(chl_1);
        amp_dec[0].push_back(amp_0);
        amp_dec[1].push_back(amp_1);
    }

    cout << cBLUE << "The no dec data filter done!" << cRESET << endl;
    cout << endl;
}

void dec_pro::mux_read(int k)
{
    for (int i = 0; i < idx.size();)
    {
        bool find_0 = false;
        for (int j = 0; j < idx[i].size(); j++)
        {
            if (idx[i][j] == k)
            {
                find_0 = true;
            }
        }
        if (!find_0)
        {
            trig.erase(trig.begin() + i);
            chl.erase(chl.begin() + i);
            amp.erase(amp.begin() + i);
            idx.erase(idx.begin() + i);
        }
        else
        {
            i++;
        }
    }

    for (int i = 0; i < idx.size(); i++)
    {
        vector<int> temp_idx0;
        vector<int> temp_idx1;
        vector<int> temp_chl0;
        vector<int> temp_chl1;
        vector<int> temp_amp0;
        vector<int> temp_amp1;

        for (int j = 0; j < idx[i].size(); j++)
        {
            if (idx[i][j] == k)
            {
                if (chl[i][j] <= 63)
                {
                    temp_amp0.push_back(amp[i][j]);
                    temp_chl0.push_back(chl[i][j]);
                    temp_idx0.push_back(idx[i][j]);
                }
                else
                {
                    temp_amp1.push_back(amp[i][j]);
                    temp_chl1.push_back(chl[i][j] - 64);
                    temp_idx1.push_back(idx[i][j] + 1);
                }
            }
        }
        idx_dec[0].push_back(temp_idx0);
        idx_dec[1].push_back(temp_idx1);
        chl_dec[0].push_back(temp_chl0);
        chl_dec[1].push_back(temp_chl1);
        amp_dec[0].push_back(temp_amp0);
        amp_dec[1].push_back(temp_amp1);
    }
    cout << cBLUE << "The dec data filter done!" << cRESET << endl;
    cout << endl;
}

void dec_pro::hit_data_dec(int index, int i)
{
    is_qx_dec.push_back(0);
    is_qy_dec.push_back(0);

    int tem = nhits;
    for (int j = 0; j < 2; j++)
    {
        int dec_strip_cnt = 0;
        if (j != 0)
        {
            for (auto &element : idx_dec[j][i])
            {
                element = 0;
            }
        }
          
        if (dec_task_cg->run_dec(chl_dec[j][i], idx_dec[j][i]))
        {
            for (int k = 0; k < dec_task_cg->cluster_num.size(); k++)
            {
                if (((dec_task_cg->cluster_num[k] - dec_task_cg->cluster_holed[k]) == 1))
                {
                    dec_strip_cnt += dec_task_cg->cluster_num[k];
                    continue;
                }
                for (int z = 0; z < dec_task_cg->cluster_num[k]; z++)
                {
                    hit_strips[nhits] = dec_task_cg->hit_strip[z + dec_strip_cnt];
                    hit_amp[nhits] = amp_dec[j][i][dec_task_cg->hit_seq[z + dec_strip_cnt]];
                    dimension_idx[nhits] = 2 * index + j;
                    nhits++;
                }
                cluster_size[cluster_number] = dec_task_cg->cluster_num[k];
                cluster_number++;
                dec_strip_cnt += dec_task_cg->cluster_num[k];
            }
        }
        if (nhits > tem)
        {
            if (j == 0)
            {
                is_qx_dec[index] = 1;
                tem = nhits;
            }
            else
            {
                is_qy_dec[index] = 1;
            }
        }
    }
}

void dec_pro::v_clear()
{
    idx_dec.clear();
    chl_dec.clear();
    amp_dec.clear();
}
