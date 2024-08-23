#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include <array>
#include <map>
#include <unordered_map>

#include "fit_pro.hh"
#include "DBSCAN.hh"
#include "DataPoint.hh"

#include "TFolder.h"
#include "TFile.h"

using namespace std;

fit_pro::fit_pro(string input_filename, string align_filename, string range_filename, int layer_num, float rmse_precision, bool idx_discontinuous, int idx_disc, double sigma, double LSB, string save_path, bool is_req_xy, bool is_write_txt, bool is_angle, double angle, bool is_muon_scatter)
{
    this->det_layer_used = layer_num;
    this->rmse_precision = rmse_precision;
    this->idx_discontinuous = idx_discontinuous;
    this->idx_disc = idx_disc;
    this->sigma = sigma;
    this->LSB = LSB;
    this->save_path = save_path;
    this->is_req_xy = is_req_xy;
    this->is_write_txt = is_write_txt;
    this->is_angle = is_angle;
    this->angle = angle;
    this->is_muon_scatter = is_muon_scatter;
    // 前提条件
    read_dec_file(align_filename, alignment);
    read_dec_file(range_filename, dec_range);

    vector<float> x_hitrange(dec_range.begin(), dec_range.begin() + dec_range.size() / 2);
    vector<float> y_hitrange(dec_range.begin() + dec_range.size() / 2, dec_range.end());

    fit_range = x_hitrange;
    read_alignment();

    int data_number = read_root_file(input_filename);
    is_file_get = data_number > 0;

    if (is_muon_scatter)
    {
      info_out("is_muon_scatter");
        len = det_layer_used / 2;

        // 上层计算
        setlayer = 0;
        setlayer_used = det_layer_used / 2;
        idx_gen();
        convert_position_data();
        run();
        fit_data_all_up = fit_data_all;

        clear_init();

        // 下层计算
        is_down = true;
        setlayer = det_layer_used / 2;
        setlayer_used = det_layer_used;
        idx_gen();
        convert_position_data();
        run();
        fit_data_all_down = fit_data_all;

        poca_file.open(save_path + "poca.txt", ios::app);
        poca_track();
    }
    else
    {
        setlayer = 0;
        setlayer_used = det_layer_used;
        idx_gen();
        out_result();

        len = dec_idx.size();
        info_out(len);
        convert_position_data();
        run();
    }
}

fit_pro::~fit_pro()
{
}

int fit_pro::read_root_file(string data_infilename)
{
    hit_data_file = new TFile(data_infilename.c_str());
    if (hit_data_file->IsZombie() || !hit_data_file->IsOpen())
    {
        cout << cRED << "[ERROR]: File" << data_infilename << " open failed. Please check the file" << cRESET << endl;
        return -1;
    }
    hit_data_tree = (TTree *)hit_data_file->Get("fec_hit_data");
    if (hit_data_tree == nullptr)
    {
        cout << cRED << "[ERROR]: Cannot get TTree* fec_hit_data in file " << data_infilename << cRESET << endl;
        return -1;
    }
    hit_data_tree->SetBranchAddress("trigger_id_all", &trigger_id, &b_triggera_id);
    hit_data_tree->SetBranchAddress("total_layer_num", &total_layer_num, &b_total_layer_num);
    for (int i = 0; i < det_layer_used; i++)
    {

        string dec_name = "dector" + to_string(i);
        hit_data_tree->SetBranchAddress(dec_name.c_str(), &detector_data[i].x_nhits, &b_detector_data[i]);
    }
    cout << cGREEN << "TTree in TFile " << data_infilename << " get!" << cRESET << endl;
    total_entried_num = hit_data_tree->GetEntries();
    if (total_entried_num != 0)
    {
        hit_data_tree->GetEntry(0);
    }
    return total_entried_num;
}

bool fit_pro::convert_position_data()
{
    if (!is_file_get)
    {
        cout << cRED << "Data in file initial failed!" << endl;
        return false;
    }

    cout << total_entried_num << endl;
    info_out(total_entried_num); //1895879

    std::map<int,std::array<size_t,3>> array00;
    for (int hit_idx = 0; hit_idx < total_entried_num; hit_idx++)
    {
        hit_data_tree->GetEntry(hit_idx);

        vector<float> temp_chnx, temp_chny;
        vector<int> temp_ampx, temp_ampy;
        vector<int> temp_dimx, temp_dimy;
        vector<float> temp_clux, temp_cluy;

        int xnum = 0;
        int ynum = 0;

//        info_out(setlayer); info_out(setlayer_used); exit(0);
        //for (int i = setlayer; i<setlayer_used; ++i){
        //  info_out(detector_data[i].x_nhits);
        //  info_out(detector_data[i].y_nhits);
        //}
        //info_out("");
        //continue;




        info_out(setlayer_used);
        for (int i = setlayer; i < setlayer_used; i++)
        {
            if (hit_idx == 0)
            {
                dec_construct.push_back(detector_data[i].z * 10); //shouble 'z' unit as cm;
                z_uper[i] = detector_data[i].z * 10;
                if (i >= 3)
                {
                    z_downer[i - 3] = detector_data[i].z * 10; 
                }
            }

            if (detector_data[i].x_nhits > 0)
            {
                xnum++;
                array00[i][0]++;
                info_out(detector_data[i].x_nhits);
                for (int j = 0; j < detector_data[i].x_nhits; j++)
                {
                    temp_chnx.push_back(detector_data[i].x[j] * 0.4); //mm
                    info_out(detector_data[i].x[j]);
                    info_out(detector_data[i].x[j]*0.4);
                    temp_ampx.push_back(abs(detector_data[i].x_amp[j]) * LSB);
                    temp_clux.push_back(detector_data[i].x_chn_num[j]);
                    temp_dimx.push_back(i);
                }
            }

            if (detector_data[i].y_nhits > 0)
            {
                ynum++;
                array00[i][1]++;
                info_out(detector_data[i].y_nhits);
                for (int j = 0; j < detector_data[i].y_nhits; j++)
                {
                    temp_chny.push_back(detector_data[i].y[j] * 0.4);
                    info_out(detector_data[i].y[j]*0.4);
                    temp_ampy.push_back(abs(detector_data[i].y_amp[j]) * LSB);
                    temp_cluy.push_back(detector_data[i].y_chn_num[j]);
                    temp_dimy.push_back(i);
                }
            }
            if (detector_data[i].x_nhits > 0 && detector_data[i].y_nhits>0) array00[i][2]++;

        }

        if (
            (temp_clux.size() > 0 && xnum == (setlayer_used - setlayer)) || 
            (temp_cluy.size() > 0 && ynum == ((setlayer_used - setlayer))))
        {
          info_out("0000");
            trig_id.push_back(trigger_id);
            t_channel_x.push_back(temp_chnx);
            t_amp_x.push_back(temp_ampx);
            t_dimension_x.push_back(temp_dimx);
            t_cluster_x.push_back(temp_clux);

            t_channel_y.push_back(temp_chny);
            t_amp_y.push_back(temp_ampy);
            t_dimension_y.push_back(temp_dimy);
            t_cluster_y.push_back(temp_cluy);
        }
        else
        {
          info_out("1111");
            continue;
        }
    }


    cout << cBLUE << "Data in file convert success!" << cRESET << endl;
    cout << endl;

    //for (auto&& [x,y] : array00){
    //  info_out(x);
    //  for (auto&&  y0 : y) info_out(y0);
    //}
            for (auto&& [x,y] : array00){
              std::cout<<"\t"<<x
                <<"\t\t"<<y[0]
                <<"\t\t"<<y[1]
                <<"\t\t"<<y[2]<<std::endl;
            }

    //exit(0);
    return true;
}

void fit_pro::run()
{
    process_data('x');
    process_data('y');

    filterAndCombine();
    if (idx_discontinuous)
    {
        writeSpatial();
        outall.close();
    }
}

void fit_pro::process_data(char axis)
{
    if (axis == 'x')
    {
        t_channel_2d = t_channel_x;
        t_amp_2d = t_amp_x;
        t_dimension_2d = t_dimension_x;
        t_cluster_2d = t_cluster_x;
        file_select = 0;
    }
    else if (axis == 'y')
    {
        t_channel_2d = t_channel_y;
        t_amp_2d = t_amp_y;
        t_dimension_2d = t_dimension_y;
        t_cluster_2d = t_cluster_y;
        file_select = 1;
    }

    trig = trig_id;
    data_select();
    data_fit();

    if (axis == 'x')
    {
        fec_datax = fec_data_fit;
        info_out(fec_data_fit.size());
        xidx_data = idx_data;
        xaim_data = aim_data;
        xaim_data_amp = aim_data_amp;
        xaim_data_cluster = aim_data_cluster;
    }
    else if (axis == 'y')
    {
        fec_datay = fec_data_fit;
        info_out(fec_data_fit.size());
        yidx_data = idx_data;
        yaim_data = aim_data;
        yaim_data_amp = aim_data_amp;
        yaim_data_cluster = aim_data_cluster;
    }

    clear_data();
}

void fit_pro::clear_data()
{
    idx_data.clear();
    aim_data.clear();
    aim_data_amp.clear();
    aim_data_cluster.clear();
    fec_data_fit.clear();
    all_assembly.clear();
    fec_data_combine.clear();
    fec_data_combine_cluster.clear();
}

void fit_pro::clear_init()
{
    trig.clear();
    t_channel_2d.clear();
    t_amp_2d.clear();
    t_dimension_2d.clear();
    t_cluster_2d.clear();
    trig_id.clear();
    t_channel_x.clear();
    t_amp_x.clear();
    t_dimension_x.clear();
    t_cluster_x.clear();
    t_channel_y.clear();
    t_amp_y.clear();
    t_dimension_y.clear();
    t_cluster_y.clear();
    dec_construct.clear();
    dec_idx.clear();

    fit_data_all.clear();
    fit_data.clear();
}

void fit_pro::poca_track()
{

    // ofstream upfile(save_path + "up.txt", ios::app);
    // ofstream downfile(save_path + "down.txt", ios::app);
    // for (auto &row : fit_data_all_up)
    // {
    //     for (int i = 0; i < row.size(); i++)
    //     {
    //         upfile << row[i] << " ";
    //     }
    //     upfile << endl;
    // }
    // for (auto &row : fit_data_all_down)
    // {
    //     for (int i = 0; i < row.size(); i++)
    //     {
    //         downfile << row[i] << " ";
    //     }
    //     downfile << endl;
    // }

    int n = 0;
    std::cout<<"========>fit_data_all_down: "<<fit_data_all_down.size()<<std::endl;
    for (auto &row : fit_data_all_down)
    {
      std::cout<<"~~~~~~~~>row1: "<<fit_data_all_up.size()<<std::endl;
        for (const auto &row1 : fit_data_all_up)
        {

            if (row.front() == row1.front())
            {
                double x_up = row[7] * 280 + row[8];
                double y_up = row[16] * 280 + row[17];
                double x_down = row1[7] * 240 + row1[8];
                double y_down = row1[16] * 240 + row1[17];
                MyTracker *tracker_up = new MyTracker(TVector3(x_up, y_up, 280), TVector3(row[7], row[16], 1));
                MyTracker *tracker_down = new MyTracker(TVector3(x_down, y_down, 240), TVector3(row1[7], row1[16], 1));
                my_poca *poca_run = new my_poca(tracker_up, tracker_down);
                MyPocaResult poca_result = poca_run->poca();
                write_poca(poca_result);
                n++;

                trig_event = row[0];
                kx_up = row[7];
                ky_up = row[16];
                kx_down = row1[7];
                ky_down = row1[16];
                bx_up = row[8];
                by_up = row[17];
                bx_down = row1[8];
                by_down = row1[17];
                for (int i = 0; i < len; i++)
                {
                    x_uper[i] = row[1 + i];
                    y_uper[i] = row[10 + i];
                    x_strip_num_up[i] = row[4 + i];
                    y_strip_num_up[i] = row[13 + i];
                    x_downer[i] = row1[1 + i];
                    y_downer[i] = row1[10 + i];
                    x_strip_num_down[i] = row1[4 + i];
                    y_strip_num_down[i] = row1[13 + i];
                }
                info_out("write_event_json_file");
                write_event_json_file(n, poca_result);

                delete tracker_up;
                delete tracker_down;
                delete poca_run;
            }
        }
    }
    cout << n << endl;
    draw_scatter();

    poca_file.close();
}

void fit_pro::write_poca(MyPocaResult poca_result)
{
    poca_file << poca_result.ScatterPoint.X() << " " << poca_result.ScatterPoint.Y() << " " << poca_result.ScatterPoint.Z() << " "
              << poca_result.ScatterAngle << " " << poca_result.PlaneAngleX << " " << poca_result.PlaneAngleY << " " << poca_result.t << " " << poca_result.s << endl;
    vector<float> temp_scater;
    if (abs(poca_result.ScatterAngle) >= 16 && poca_result.ScatterPoint.Z() >= 200 && poca_result.ScatterPoint.Z() <= 400)
    {
        temp_scater.push_back(poca_result.ScatterPoint.X());
        temp_scater.push_back(poca_result.ScatterPoint.Y());
        temp_scater.push_back(poca_result.ScatterPoint.Z());
        temp_scater.push_back(poca_result.ScatterAngle);
    }
    if (temp_scater.size() > 0)
    {
        scater.push_back(temp_scater);
    }
}

bool fit_pro::data_select()
{
  info_out(trig.size());
    if (trig.size() == 0)
    {
        cout << cRED << "no valid data" << cRESET << endl;
        cout << endl;
        return false;
    }
    else
    {
        if (idx_discontinuous)
        {
            for (int i = 0; i < t_dimension_2d.size(); i++)
            {
                int max = 0;
                float temp = 0;
                vector<double> strip;
                vector<double> amp;
                vector<double> cluster;
                strip.push_back(trig[i]);
                amp.push_back(trig[i]);
                cluster.push_back(trig[i]);
                for (int j = t_dimension_2d[i].size() - 1; j >= 0; j--)
                {
                    if (t_dimension_2d[i][j] == idx_disc)
                    {
                        strip.push_back(t_channel_2d[i][j]);
                        amp.push_back(t_amp_2d[i][j]);
                        cluster.push_back(t_cluster_2d[i][j]);
                        if (t_amp_2d[i][j] > max)
                        {
                            max = t_amp_2d[i][j];
                            temp = t_channel_2d[i][j];
                        }
                        t_channel_2d[i].erase(t_channel_2d[i].begin() + j);
                        t_amp_2d[i].erase(t_amp_2d[i].begin() + j);
                        t_dimension_2d[i].erase(t_dimension_2d[i].begin() + j);
                    }
                }
                if (max != 0)
                {
                    idx_data.push_back(make_pair(trig[i], temp));
                    aim_data.push_back(strip);
                    aim_data_amp.push_back(amp);
                    aim_data_cluster.push_back(cluster);
                }
            }
        }

        vector<vector<int>> containers(len);
        for (int i = 0; i < t_dimension_2d.size(); i++)
        {
            for (int j = 0; j < len; j++)
            {
                containers[j].push_back(count(t_dimension_2d[i].begin(), t_dimension_2d[i].end(), dec_idx[j]));
            }
        }
        for (int i = 0; i < containers[0].size(); ++i)
        {
            int temp = 1;
            for (int j = 0; j < len; ++j)
            {
                temp *= containers[j][i];
            }
            all_assembly.push_back(temp);
        }

        for (int i = 0; i < t_channel_2d.size(); i++)
        {
            vector<float> temp;
            vector<float> temp_current;
            vector<int> count(len, 0);
            vector<float> temp_cluster;
            vector<float> temp_cluster_current;

            data_combine(0, i, count, containers, t_channel_2d, temp, temp_current);
            data_combine(0, i, count, containers, t_cluster_2d, temp_cluster, temp_cluster_current);
            fec_data_combine.push_back(temp);
            fec_data_combine_cluster.push_back(temp_cluster);
        }

        return true;
    }
}

void fit_pro::data_combine(int level, int dec_idx, vector<int> &count, vector<vector<int>> &containers, vector<vector<float>> &t_channel_2d, vector<float> &temp_x, vector<float> &temp_x_current)
{
    if (level == containers.size())
    {
        return;
    }

    for (int i = 0; i < containers[level][dec_idx]; i++)
    {
        int k = 0;
        for (int j = 0; j < level; j++)
        {
            k = k + containers[j][dec_idx];
        }
        vector<float> temp_x_next;
        temp_x_next.clear();

        if (level != 0)
        {
            temp_x.push_back(t_channel_2d[dec_idx][i + k]);
            temp_x_next = temp_x_current;
            temp_x_next.push_back(t_channel_2d[dec_idx][i + k]);
        }
        else
        {
            temp_x_next.push_back(t_channel_2d[dec_idx][i]);
            temp_x.push_back(t_channel_2d[dec_idx][i]);
        }
        data_combine(level + 1, dec_idx, count, containers, t_channel_2d, temp_x, temp_x_next);

        if ((i < containers[level][dec_idx] - 1) && (level != 0))
        {
            temp_x.insert(temp_x.end(), temp_x_current.begin(), temp_x_current.end());
        }
    }
}

void fit_pro::data_fit()
{
    if (idx_discontinuous && file_select == 0)
    {
        for (int i = 0; i < det_layer_used; i++)
        {
            if (i == idx_disc)
            {
                fit_location = dec_construct[i] + tz;
                dec_construct.erase(dec_construct.begin() + i);
            }
        }
    }

    // 拟合数据
    int data_size = len;
    for (int i = 0; i < all_assembly.size(); i++)
    {
        int k = 0;
        float slope = 0;
        float Intercept = 0;
        float rmse = 0;
        float fit_min = rmse_precision;
        float fit_hit = 0;
        double tan = 0;
        double fit_angle = 0;

        vector<float> y_data;
        vector<float> cluster_data;
        vector<float> out_data;
        vector<float> fit_data;
        vector<float> temp;

        out_data.clear();
        fit_data.clear();
        temp.clear();
        y_data.clear();

        for (int j = 0; j < all_assembly[i]; j++)
        {
            for (int m = 0; m < data_size; m++)
            {
                y_data.push_back(fec_data_combine[i][k + m]);
                cluster_data.push_back(fec_data_combine_cluster[i][k + m]);
            }
            k = k + data_size;

            Eigen::MatrixXd X(data_size, 2);
            Eigen::VectorXd Y(data_size);

            for (int n = 0; n < data_size; n++)
            {
                if (!is_down)
                {
                    X(n, 0) = 1.0;
                    X(n, 1) = dec_construct[n] + z_alignment[n];
                    if (file_select == 0)
                    {
                        Y(n) = y_data[n] + x_alignment[n];
                    }
                    else
                    {
                        Y(n) = y_data[n] + y_alignment[n];
                    }
                }
                else
                {
                    X(n, 0) = 1.0;
                    X(n, 1) = dec_construct[n] + z_alignment[n + 3];
                    if (file_select == 0)
                    {
                        Y(n) = y_data[n] + x_alignment[n + 3];
                    }
                    else
                    {
                        Y(n) = y_data[n] + y_alignment[n + 3];
                    }
                }
            }

            Eigen::VectorXd beta = X.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Y);

            for (int z = 0; z < data_size; z++)
            {
                fit_data.push_back(beta(0) + beta(1) * dec_construct[z]);
                rmse = rmse + (y_data[z] - fit_data[z]) * (y_data[z] - fit_data[z]);
            }
            rmse = sqrt(rmse / data_size);

            if (is_angle)
            {
                tan = beta(1);
                fit_angle = abs(atan(tan) * (180 / M_PI));
            }
            else
            {
                fit_angle = 0;
            }

            if (rmse < rmse_precision && rmse < fit_min && fit_angle < angle)
            {
                out_data.clear();
                fit_min = rmse;
                slope = beta(1);
                Intercept = beta(0);
                for (int z = 0; z < data_size; z++)
                {
                    out_data.push_back(fit_data[z]);
                }
                for (int z = 0; z < data_size; z++)
                {
                    out_data.push_back(cluster_data[z]);
                }
                if (idx_discontinuous)
                {
                    fit_hit = fit_location * slope + Intercept;
                }
            }
            y_data.clear();
            cluster_data.clear();
        }

        if (fit_hit != 0 && fit_min < rmse_precision && fit_hit >= fit_range[0] && fit_hit <= fit_range[1] && idx_discontinuous)
        {
            temp.push_back(trig[i]);
            temp.push_back(slope);
            temp.push_back(Intercept);
            temp.push_back(fit_min);
            temp.push_back(fit_hit);
            fec_data_fit.push_back(temp);
        }
        else if (fit_min < rmse_precision && !idx_discontinuous)
        {
            if (is_muon_scatter)
            {
                temp.push_back(trig[i]);
                for (int z = 0; z < out_data.size(); z++)
                {
                    temp.push_back(out_data[z]);
                }
                temp.push_back(slope);
                temp.push_back(Intercept);
                temp.push_back(fit_min);
            }
            else
            {
                temp.push_back(trig[i]);
                for (int z = 0; z < out_data.size(); z++)
                {
                    temp.push_back(out_data[z]);
                }
                temp.push_back(slope);
                temp.push_back(Intercept);
                temp.push_back(fit_min);
            }
            fec_data_fit.push_back(temp);
        }
    }
    cout << fec_data_fit.size() << endl;

    cout << cBLUE << "fec data fit success" << cRESET << endl;
    cout << endl;
}

void fit_pro::filterAndCombine()
{

    for (const auto &row1 : fec_datax)
    {
        for (const auto &row2 : fec_datay)
        {
            vector<float> temp;
            if (row1[0] == row2[0])
            {
                fit_data.push_back({row1[0], row1.back(), row2.back()});
                for (int i = 0; i < row1.size(); i++)
                {
                    temp.push_back(row1[i]);
                }
                for (int j = 1; j < row2.size(); j++)
                {
                    temp.push_back(row2[j]);
                }
                fit_data_all.push_back(temp);
            }
        }
    }
    cout << fit_data_all.size() << endl;
    if (is_write_txt)
    {
        string filename = save_path + to_string(idx_disc) + "_fit_data.txt";

        ofstream out(filename, ios::app);
        for (int i = 0; i < fit_data_all.size(); i++)
        {
            for (int j = 0; j < fit_data_all[i].size(); j++)
            {
                out << fit_data_all[i][j] << " ";
            }
            out << endl;
        }
    }
}

void fit_pro::findTarget(
    vector<vector<double>> &aimData
    , vector<vector<double>> &ampData
    , vector<vector<double>> &clusterData
    , vector<vector<float>> &fecData
    , vector<vector<double>> &dataSecondary
    , vector<vector<double>> &dataAmpSecondary
    , vector<vector<double>> &dataClusterSecondary
    , double txOrTy)
{
    for (auto &row : aimData)
    {
        for (const auto &row1 : fecData)
        {
            if (row.front() == row1.front())
            {
                double n = row1.back();
                for (int i = 1; i < row.size(); i++)
                {
                    row[i] = row[i] - n + txOrTy;
                }
                dataSecondary.push_back(row);
            }
        }
    }

    for (auto &row : ampData)
    {
        for (const auto &row1 : fecData)
        {
            if (row.front() == row1.front())
            {
                dataAmpSecondary.push_back(row);
            }
        }
    }

    for (auto &row : clusterData)
    {
        for (const auto &row1 : fecData)
        {
            if (row.front() == row1.front())
            {
                dataClusterSecondary.push_back(row);
            }
        }
    }
}

void fit_pro::calculateSpatial(
    vector<vector<double>> &dataSecondary
    , vector<vector<double>> &dataAmpSecondary
    , vector<vector<double>> &dataClusterSecondary
    , vector<vector<double>> &dataTertiary
    , vector<vector<double>> &dataAmpTertiary
    , vector<vector<double>> &dataClusterTertiary
    , const string &filename, bool xory)
{
    for (int i = 0; i < dataSecondary.size(); i++)
    {
        vector<double> temp, amp, cluster;
        double min = sigma, ampTemp = 0, clusterTemp = 0;
        temp.push_back(dataSecondary[i][0]);
        amp.push_back(dataAmpSecondary[i][0]);
        cluster.push_back(dataClusterSecondary[i][0]);
        for (int j = 1; j < dataSecondary[i].size(); j++)
        {
            if (abs(dataSecondary[i][j]) < abs(min))
            {
                min = dataSecondary[i][j];
                ampTemp = dataAmpSecondary[i][j];
                clusterTemp = dataClusterSecondary[i][j];
            }
        }
        temp.push_back(min);
        amp.push_back(ampTemp);
        cluster.push_back(clusterTemp);
        if (temp.size() > 1 && min != sigma)
        {
            dataTertiary.push_back(temp);
            dataAmpTertiary.push_back(amp);
            dataClusterTertiary.push_back(cluster);
        }
    }

    if (is_write_txt)
    {
        ofstream resultFile(filename, ios::app);
        if (!resultFile.is_open())
        {
            cout << filename << " open failed" << endl;
        }
        else
        {
            for (int i = 0; i < dataTertiary.size(); i++)
            {
                resultFile << dataTertiary[i][0] << " ";
                for (int j = 1; j < dataTertiary[i].size(); j++)
                {
                    resultFile << dataTertiary[i][j] << " " << dataAmpTertiary[i][j] << " " << dataClusterTertiary[i][j] << " ";
                }
                resultFile << endl;
            }
        }
    }
    else
    {
        draw_spatial(dataTertiary, xory);
        if (!is_angle)
        {
            draw_energy(dataAmpTertiary, xory);
            draw_cluster(dataClusterTertiary, xory);
        }

        string eff_name;
        double eff;
        if (xory)
        {
            eff_name = "x_eff_" + to_string(idx_disc + 1);
            eff = dataTertiary.size() / vld_x;
        }
        else
        {
            eff_name = "y_eff_" + to_string(idx_disc + 1);
            eff = dataTertiary.size() / vld_y;
        }
        cout << cMAGENTA << eff_name << " " << eff << cRESET << endl;
        outall << eff_name << " " << eff << endl;
    }
}

void fit_pro::cal_all_energy(vector<vector<double>> &xampData, vector<vector<double>> &yampData, vector<double> &Ampall)
{
    for (auto &row : xampData)
    {
        for (const auto &row1 : yampData)
        {
            if (row.front() == row1.front())
            {
                double sum = row.back() + row1.back();
                Ampall.push_back(sum);
            }
        }
    }
}

void fit_pro::writeSpatial()
{
    if (is_req_xy)
    {
        tempx = fit_data_all;
        tempy = fit_data_all;
        vld_x = fit_data_all.size();
        vld_y = fit_data_all.size();
        spatial_name = "xysim_";
    }
    else
    {
        tempx = fec_datax;
        tempy = fec_datay;
        vld_x = fec_datax.size();
        vld_y = fec_datay.size();
        if (is_write_txt)
        {
            spatial_name = "xyunsim_";
            ofstream out(save_path + spatial_name + to_string(idx_disc) + "xyeff.txt", ios::app);
            out << tempx.size() << " " << tempy.size() << endl;
        }
    }
    vector<vector<double>> xDataSecondary, yDataSecondary, xDataAmpSecondary, yDataAmpSecondary, xDataClusterSecondary, yDataClusterSecondary;
    findTarget(xaim_data, xaim_data_amp, xaim_data_cluster, tempx, xDataSecondary, xDataAmpSecondary, xDataClusterSecondary, tx);
    findTarget(yaim_data, yaim_data_amp, yaim_data_cluster, tempy, yDataSecondary, yDataAmpSecondary, yDataClusterSecondary, ty);

    vector<vector<double>> xDataTertiary, yDataTertiary, xDataAmpTertiary, yDataAmpTertiary, xDataClusterTertiary, yDataClusterTertiary;
    calculateSpatial(xDataSecondary, xDataAmpSecondary, xDataClusterSecondary, xDataTertiary, xDataAmpTertiary, xDataClusterTertiary, save_path + spatial_name + to_string(idx_disc) + "idx_x_reslut.txt", true);
    calculateSpatial(yDataSecondary, yDataAmpSecondary, yDataClusterSecondary, yDataTertiary, yDataAmpTertiary, yDataClusterTertiary, save_path + spatial_name + to_string(idx_disc) + "idx_y_reslut.txt", false);
    if (!is_angle)
    {
        vector<double> Ampall;
        cal_all_energy(xDataAmpTertiary, yDataAmpTertiary, Ampall);
        draw_all_energy(Ampall);
    }
}

void fit_pro::idx_gen()
{
    for (int i = setlayer; i < setlayer_used; i++)
    {
        if (!idx_discontinuous || i != idx_disc)
        {
            dec_idx.push_back(i);
        }
    }
    info_out(dec_idx.size());
}

void fit_pro::read_dec_file(string filename, vector<float> &dec)
{
    ifstream dec_file(filename);
    if (!dec_file.is_open())
    {
        cout << filename << " open failed" << endl;
    }
    else
    {
        float tmp;
        while (dec_file >> tmp)
        {
            dec.push_back(tmp);
        }
    }
}

void fit_pro::read_alignment()
{

    for (int i = 0; i < alignment.size(); i++)
    {
        if (i < det_layer_used)
        {
            x_alignment.push_back(alignment[i]);
        }
        else if (i < det_layer_used * 2 && i >= det_layer_used)
        {
            y_alignment.push_back(alignment[i]);
        }
        else if (i < det_layer_used * 3 && i >= det_layer_used * 2)
        {
            z_alignment.push_back(alignment[i]);
        }
    }

    if (idx_discontinuous)
    {
        for (int i = 0; i < x_alignment.size(); i++)
        {
            if (i == idx_disc)
            {
                tx = x_alignment[i];
                ty = y_alignment[i];
                tz = z_alignment[i];
                x_alignment.erase(x_alignment.begin() + i);
                y_alignment.erase(y_alignment.begin() + i);
                z_alignment.erase(z_alignment.begin() + i);
            }
        }
    }
}

void fit_pro::draw_spatial(vector<vector<double>> rms, bool xory)
{
    string name = (xory ? "x" : "y");
    name = name + "_spatial_layer" + to_string(idx_disc + 1);
    TH1F *spatial = new TH1F(name.c_str(), "Histogram from rms", 100, -5, 5);

    for (const auto &rms_vec : rms)
    {
        for (size_t j = 1; j < rms_vec.size(); j++)
        {
            spatial->Fill(rms_vec[j]);
        }
    }

    TF1 *fitFcn = new TF1("fitFcn", "gaus(0) + gaus(3)", -5, 5);
    fitFcn->SetNpx(1600);

    // Set initial parameter values
    fitFcn->SetParameters(spatial->GetMaximum(), spatial->GetMean(), spatial->GetRMS(),
                          spatial->GetMaximum() / 2, spatial->GetMean(), spatial->GetRMS() * 2);

    // Fit histogram
    string xais = (xory ? "#Deltax" : "#Deltay");
    spatial->Fit("fitFcn", "R");
    spatial->GetXaxis()->SetTitle(xais.c_str());
    spatial->GetYaxis()->SetTitle("Counts");
    spatial->GetXaxis()->CenterTitle();
    spatial->GetYaxis()->CenterTitle();

    double lowerBound = -5.0;
    double upperBound = 5.0;

    while (fitFcn->GetParameter(0) < 0 || fitFcn->GetParameter(3) < 0)
    {
        lowerBound += 0.5;
        upperBound -= 0.5;
        delete fitFcn;
        fitFcn = new TF1("fitFcn", "gaus(0) + gaus(3)", lowerBound, upperBound);
        fitFcn->SetNpx(1600);
        fitFcn->SetParameters(spatial->GetMaximum(), spatial->GetMean(), spatial->GetRMS(),
                              spatial->GetMaximum() / 2, spatial->GetMean(), spatial->GetRMS() * 2);
        spatial->Fit("fitFcn", "R");
    }

    TCanvas *c2 = new TCanvas("c2", "spitial_resolution", 800, 600);

    // Draw original histogram and fit function on new canvas
    spatial->Draw();
    // Create TF1 objects representing two Gaussian functions
    TF1 *gaus1 = new TF1("gaus1", "gaus", -5, 5);
    TF1 *gaus2 = new TF1("gaus2", "gaus", -5, 5);

    // Set their parameters
    gaus1->SetParameters(fitFcn->GetParameter(0), fitFcn->GetParameter(1), fitFcn->GetParameter(2));
    gaus2->SetParameters(fitFcn->GetParameter(3), fitFcn->GetParameter(4), fitFcn->GetParameter(5));
    gaus1->SetLineColor(kBlue);
    int brown = TColor::GetColor("#8B4513");
    gaus2->SetLineColor(brown);
    // Draw them on the same canvas
    gaus1->Draw("same");
    gaus2->Draw("same");

    double sigma = (fitFcn->GetParameter(0) > fitFcn->GetParameter(3)) ? fitFcn->GetParameter(2) : fitFcn->GetParameter(5);
    cout << cMAGENTA << "Sigma of Gaussian: " << sigma << endl;
    auto t = new TLatex;
    t->DrawLatex(1.25, 30, to_string(sigma).c_str());

    // Save canvas as PDF file
    string filename;
    string sigmaname;
    int angle_set = angle;

    if (xory)
    {
        filename = save_path + to_string(idx_disc) + "_spatial_x" + (is_angle ? "_angle" + to_string(angle_set) : "") + ".png";
        sigmaname = "Xspatial_" + to_string(idx_disc + 1);
    }
    else
    {
        filename = save_path + to_string(idx_disc) + "_spatial_y" + (is_angle ? "_angle" + to_string(angle_set) : "") + ".png";
        sigmaname = "Yspatial_" + to_string(idx_disc + 1);
    }

    outall << sigmaname << " " << sigma << endl;
    c2->SaveAs(filename.c_str());

    spatial->Reset();
    delete spatial;
    delete fitFcn;
    delete gaus1;
    delete gaus2;
    delete c2;
    delete t;
}

void fit_pro::draw_energy(vector<vector<double>> energy, bool xory)
{
  info_out("draw_energy");
    string name = (xory ? "x" : "y");
    name = name + "_energy_layer" + to_string(idx_disc + 1);

    auto* view_fout = new TFile("fit_pro.root","update");
    view_fout->cd();

    



    TH1F *muonenergy = new TH1F(name.c_str(), "Histogram from muon deposit energy", 100, -5, 800);

    for (const auto &energy_vec : energy)
    {
        for (size_t j = 1; j < energy_vec.size(); j++)
        {
            muonenergy->Fill(energy_vec[j]);
        }
    }

    TF1 *fitFcn = new TF1("fitFcn", "[0]*TMath::Landau(x,[1],[2])*TMath::Gaus(x,[3],[4])", -5, 800);
    fitFcn->SetNpx(500);

    // 设置初始参数值
    fitFcn->SetParameter(0, muonenergy->GetMaximum()); // 高度
    fitFcn->SetParameter(1, muonenergy->GetMean());    // 朗道的中心
    fitFcn->SetParameter(2, muonenergy->GetRMS());     // 朗道的宽度
    fitFcn->SetParameter(3, muonenergy->GetMean());    // 高斯的中心
    fitFcn->SetParameter(4, muonenergy->GetRMS());     // 高斯的宽度

    // 拟合直方图
    muonenergy->Fit("fitFcn", "RQ");


    muonenergy->Write();

    // 创建一个新的画布
    TCanvas *c2 = new TCanvas("c2", "Canvas", 800, 600);

    // 在新的画布上绘制原始直方图和拟合函数
    muonenergy->Draw();
    fitFcn->Draw("same"); // 在原始直方图上添加拟合函数的图像

    double energy_mip = fitFcn->GetParameter(1);
    auto t = new TLatex;
    t->DrawLatex(500, 200, to_string(energy_mip).c_str());
    cout << cMAGENTA << "mip of land-Gaussian: " << energy_mip << endl;

    // 保存画布为PDF文件
    // Save canvas as PDF file
    string filename;
    string energyname;
    int angle_set = angle;

    if (xory)
    {
        filename = save_path + to_string(idx_disc) + "_energy_x" + (is_angle ? "_angle" + to_string(angle_set) : "") + ".png";
        energyname = "Xmuonenergy_" + to_string(idx_disc + 1);
    }
    else
    {
        filename = save_path + to_string(idx_disc) + "_energy_y" + (is_angle ? "_angle" + to_string(angle_set) : "") + ".png";
        energyname = "Ymuonenergy_" + to_string(idx_disc + 1);
    }

    outall << energyname << " " << energy_mip << endl;
    c2->SaveAs(filename.c_str());

    muonenergy->Reset();
    delete muonenergy;
    delete fitFcn;
    delete c2;


    view_fout->Write();
    view_fout->Close();
}
// 拟合优度怪怪的
// void fit_pro::draw_all_energy(vector<double> energy)
// {
//     string name = "xy_energy_layer" + to_string(idx_disc + 1);
//     TH1F *muonenergy = nullptr;
//     TF1 *fitFcn = nullptr;
//     double chisquare_per_ndf = 0.0;
//     int bin = 60;
//     int bestBin = bin;
//     bool isBest = false;

//     while (bin <= 200)
//     {
//         if (muonenergy)
//         {
//             delete muonenergy;
//         }
//         muonenergy = new TH1F(name.c_str(), "Histogram from muon deposit energy", bin, -5, 1200);

//         for (const auto &energy_vec : energy)
//         {
//             muonenergy->Fill(energy_vec);
//         }

//         if (fitFcn)
//         {
//             delete fitFcn;
//         }
//         fitFcn = new TF1("fitFcn", "[0]*TMath::Landau(x,[1],[2])*TMath::Gaus(x,[3],[4])", -5, 1200);
//         fitFcn->SetNpx(500);

//         // 设置初始参数值
//         fitFcn->SetParameter(0, muonenergy->GetMaximum()); // 高度
//         fitFcn->SetParameter(1, muonenergy->GetMean());    // 朗道的中心
//         fitFcn->SetParameter(2, muonenergy->GetRMS());     // 朗道的宽度
//         fitFcn->SetParameter(3, muonenergy->GetMean());    // 高斯的中心
//         fitFcn->SetParameter(4, muonenergy->GetRMS());     // 高斯的宽度

//         muonenergy->Fit("fitFcn", "R");

//         double chisquare = fitFcn->GetChisquare();
//         double ndf = fitFcn->GetNDF();
//         double critical_value = TMath::ChisquareQuantile(0.95, ndf);
//         if (chisquare <= critical_value)
//         {
//             isBest = true;
//             bestBin = bin;
//             break;
//         }
//         else
//         {
//             bin += 5;
//         }
//     }

//     // If the chi-square per degree of freedom is not in the range 0.8~1.5, use the bin that makes it closest to 1
//     if (isBest)
//     {
//         muonenergy->SetBins(bestBin, -5, 1000);
//         muonenergy->Fit("fitFcn", "R");
//     }
//     else
//     {
//         muonenergy->SetBins(100, -5, 1000);
//         muonenergy->Fit("fitFcn", "R");
//     }

//     double energy_mip = fitFcn->GetParameter(1);
//     cout << "Landau center after refitting: " << energy_mip << endl;
//     // // 拟合直方图
//     // muonenergy->Fit("fitFcn", "R");

//     // 创建一个新的画布
//     TCanvas *c2 = new TCanvas("c2", "Canvas", 800, 600);

//     // 在新的画布上绘制原始直方图和拟合函数
//     muonenergy->Draw();
//     fitFcn->Draw("same"); // 在原始直方图上添加拟合函数的图像

//     auto t = new TLatex;
//     t->DrawLatex(600, 100, to_string(energy_mip).c_str());
//     cout << cMAGENTA << "XYmip of land-Gaussian: " << energy_mip << endl;

//     // 保存画布为PDF文件
//     string filename = save_path + to_string(idx_disc) + "_energy_xy.png";
//     string energyname = "XYmuonenergy_" + to_string(idx_disc + 1);
//     outall << energyname << " " << energy_mip << endl;
//     c2->SaveAs(filename.c_str());

//     muonenergy->Reset();
//     delete muonenergy;
//     delete fitFcn;
//     delete c2;
// }

void fit_pro::draw_all_energy(vector<double> energy)
{
    string name = "xy_energy_layer" + to_string(idx_disc + 1);
    TH1F *muonenergy = new TH1F(name.c_str(), "Histogram from muon deposit energy", 100, -5, 1200);

    for (const auto &energy_vec : energy)
    {
        muonenergy->Fill(energy_vec);
    }

    TF1 *fitFcn = new TF1("fitFcn", "[0]*TMath::Landau(x,[1],[2])*TMath::Gaus(x,[3],[4])", -5, 1200);
    fitFcn->SetNpx(500);

    // 设置初始参数值
    fitFcn->SetParameter(0, muonenergy->GetMaximum()); // 高度
    fitFcn->SetParameter(1, muonenergy->GetMean());    // 朗道的中心
    fitFcn->SetParameter(2, muonenergy->GetRMS());     // 朗道的宽度
    fitFcn->SetParameter(3, muonenergy->GetMean());    // 高斯的中心
    fitFcn->SetParameter(4, muonenergy->GetRMS());     // 高斯的宽度

    // 拟合直方图
    muonenergy->Fit("fitFcn", "R");

    // 创建一个新的画布
    TCanvas *c2 = new TCanvas("c2", "Canvas", 800, 600);

    // 在新的画布上绘制原始直方图和拟合函数
    muonenergy->Draw();
    fitFcn->Draw("same"); // 在原始直方图上添加拟合函数的图像

    double energy_mip = fitFcn->GetParameter(1);
    auto t = new TLatex;
    t->DrawLatex(600, 100, to_string(energy_mip).c_str());
    cout << cMAGENTA << "XYmip of land-Gaussian: " << energy_mip << endl;

    // 保存画布为PDF文件
    string filename = save_path + to_string(idx_disc) + "_energy_xy.png";
    string energyname = "XYmuonenergy_" + to_string(idx_disc + 1);
    outall << energyname << " " << energy_mip << endl;
    c2->SaveAs(filename.c_str());

    muonenergy->Reset();
    delete muonenergy;
    delete fitFcn;
    delete c2;
}

void fit_pro::draw_cluster(vector<vector<double>> cluster, bool xory)
{
    string name = (xory ? "x" : "y");
    name = name + "_hit_strip_num_layer" + to_string(idx_disc + 1);
    TH1F *stripnum = new TH1F(name.c_str(), "Histogram from muon hit strip  number", 30, 0, 30);

    for (const auto &cluster_vec : cluster)
    {
        for (size_t j = 1; j < cluster_vec.size(); j++)
        {
            stripnum->Fill(cluster_vec[j]);
        }
    }

    // Create a new canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);

    // Draw the histogram on the canvas
    stripnum->Draw();

    // Save the canvas as a PNG file
    string filename;
    if (xory)
    {
        filename = save_path + to_string(idx_disc) + "_stripnum_x.png";
    }
    else
    {
        filename = save_path + to_string(idx_disc) + "_stripnum_y.png";
    }
    c1->SaveAs(filename.c_str());

    // Clean up
    stripnum->Reset();
    delete stripnum;
    delete c1;
}

void fit_pro::out_result()
{
    string name;
    int angle_set = angle;
    if (is_req_xy)
    {
        name = "xysim_";
    }
    else
    {
        name = "xyunsim_";
    }

    if (is_angle)
    {
        name = name + "angle" + to_string(angle_set) + "_";
    }
    else
    {
        name = name;
    }

    outall.open(save_path + name + "result.txt", ios::app);
    if (!outall.is_open())
    {
        cout << cRED << "Failed to open file: " << (save_path + name + "result.txt") << endl;
    }
}

void fit_pro::write_event_json_file(int outevt, MyPocaResult poca_result)
{
    event_json.clear();
    event_json["type"] = "event";
    event_json["id"] = 35;
    event_json["event_id"] = trig_event;
    event_json["tracks"][0]["start_point"] = {
        kx_up * 600 + bx_up, ky_up * 600 + by_up, 600};
    event_json["tracks"][1]["start_point"] = {
        kx_down * -100 + bx_down, ky_down * -100 + by_down, -100};

    TVector3 vec(kx_up, ky_up, 1);
    vec.SetMag(1);
    event_json["tracks"][0]["direction"] = {
        vec.x(), vec.y(), vec.z()};
    event_json["tracks"][0]["angle"] = {
        round(vec.Theta() / 3.1415 * 180 * 100) / 100, round(vec.Phi() / 3.1415 * 180 * 100) / 100};

    TVector3 vec1(kx_down, ky_down, 1);
    vec1.SetMag(1);
    event_json["tracks"][1]["direction"] = {
        vec1.x(), vec1.y(), vec1.z()};
    event_json["tracks"][1]["angle"] = {
        round(vec1.Theta() / 3.1415 * 180 * 100) / 100, round(vec1.Phi() / 3.1415 * 180 * 100) / 100};

    for (int i = 0; i < 3; i++)
    {
        event_json["hits"][i] = {
            kx_up * z_uper[i] + bx_up, ky_up * z_uper[i] + by_up};
        for (int j = 0; j < x_strip_num_up[i]; j++)
        {
            event_json["x_cells"][i][j] = int(x_uper[i] / 0.4) + j;
        }
        for (int j = 0; j < y_strip_num_up[i]; j++)
        {
            event_json["y_cells"][i][j] = int(y_uper[i] / 0.4) + j;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        event_json["hits"][i + 3] = {
            kx_down * z_downer[i] + bx_down, ky_down * z_downer[i] + by_down};
        for (int j = 0; j < x_strip_num_down[i]; j++)
        {
            event_json["x_cells"][i + 3][j] = int(x_downer[i] / 0.4) + j;
        }
        for (int j = 0; j < y_strip_num_down[i]; j++)
        {
            event_json["y_cells"][i + 3][j] = int(y_downer[i] / 0.4) + j;
        }
    }

    event_json["scatter_point"] = {
        poca_result.ScatterPoint.X(), poca_result.ScatterPoint.Y(), poca_result.ScatterPoint.Z(), 1};
    event_json["scatter_angles"] = {poca_result.PlaneAngleX, poca_result.PlaneAngleY};

    string js_filename = save_path + "event/" + "event_" + to_string(outevt) + ".json";
    ofstream js_file(js_filename);
    js_file << event_json << endl;
    js_file.close();
}

void fit_pro::draw_scatter()
{
    DBSCAN dbscan;
    dbscan.Init_scater(scater, 12, 9);
    dbscan.DoDBSCANRecursive();
    string dbscantxt = save_path + "dbscan.txt";
    // 创建一个非常量字符数组
    char *writable = new char[dbscantxt.size() + 1];
    std::copy(dbscantxt.begin(), dbscantxt.end(), writable);
    writable[dbscantxt.size()] = '\0'; // 添加字符串终止符
    dbscan.WriteToFile(writable);
    delete[] writable; // 释放分配的内存

    string scattername = save_path + "point.json";
    char *writable1 = new char[scattername.size() + 1];
    std::copy(scattername.begin(), scattername.end(), writable1);
    writable1[scattername.size()] = '\0'; // 添加字符串终止符
    dbscan.WriteToJSON(writable1);
    delete[] writable1; // 释放分配的内存
}
