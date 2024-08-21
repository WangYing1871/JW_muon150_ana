#include "info_calc.hh"

//#include "alignment_new.hh"
//#include "alignment_quaternion.hh"
//#include "global_function.hh"
//#include "global_variable.hh"
//#include "list_file.hh"
// #include "RooRect.hh"

#include "langaufit.hh"

#include "TCanvas.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TMath.h"
#include "TStyle.h"
#include "TText.h"
#include "TVector3.h"

// #include "RooRealVar.h"
// #include "RooGaussian.h"
// #include "RooFFTConvPdf.h"
// #include "RooDataHist.h"
// #include "RooPlot.h"

#include <iostream>
#include <string>

//#include "PlotTool.hh"

#define rad2deg 180. / TMath::Pi()
#define deg2rad TMath::Pi() / 180.

using namespace std;

info_calc::info_calc()
//    : entries(0)
{
    //trigger_id = 0;
    //total_layer_num = 0;
    //for (int i = 0; i < det_layer_used; i++)
    //{
    //    detector_data[i] = 0;
    //}
}

info_calc::~info_calc()
{
    //for (int i = 0; i < cMAX_DET_LAYER; i++)
    //{
    //    if (detector_data[i] != nullptr)
    //    {
    //        delete detector_data[i];
    //        detector_data[i] = nullptr;
    //    }
    //}
}

bool info_calc::set_input_filename(string name)
{
    hit_data_filename = name;
    return true;
}

//bool info_calc::set_correct_process(hit_correct *correct)
//{
//    m_correct = correct;
//    return true;
//}

// -------------------------------------------------------------------

bool info_calc::init()
{
    if (!init_input_file())
    {
        return false;
    }

    return true;
}

bool info_calc::execute()
{
    //if (mb_run_amp_sum)
    //{
        run_amp_sum();
        run_eff_calc();
        run_pos_res();
    //}

    /*
    if (mb_run_eff_calc)
    {
        // run_eff_calc();
        // run_eff_calc_use_changed_correct();
        // run_eff_calc_notuse_correct();
        // run_eff_calc_notuse_correct_combine_xy();
        // run_eff_calc_notuse_correct_combine_xy_split();
        // run_eff_calc_use_alignment();
        run_eff_calc_use_alignment_single_dimension();
    }
    if (mb_stats_track)
    {
        // stats_track();
        stats_track_map();
    }
    if (mb_show_time_dist)
    {
        show_time_dist();
    }
    if (mb_run_pos_res)
    {
        // run_pos_res();
        // run_pos_res_position_binned();
        run_pos_res_angular_binned();
        // run_pos_res_angular_binned_micro_TPC();
    }
    if (mb_check_correct_process)
    {
        check_correct_process();
    }
    if (mb_check_align_var)
    {
        check_align_var();
    }
    if (mb_stats_hitnums)
    {
        stats_hitnums();
    }
    if (mb_show_vars_corr)
    {
        show_vars_corr();
    }
    if (mb_check_micro_TPC)
    {
        check_micro_TPC();
    }
    if (mb_tomography)
    {
        tomography();
        // tomography_2();
    }
    if (mb_amp_eff_map)
    {
        amp_eff_map();
    }

    if (mb_show_data)
    {
        show_data();
    }
*/
    return true;
}

bool info_calc::finish()
{
    if (hit_data_file != nullptr)
    {
        delete hit_data_file;
        hit_data_file = nullptr;
    }

    return true;
}

// -------------------------------------------------------------------

bool info_calc::init_input_file()
{
    hit_data_file = new TFile(hit_data_filename.data());
    hit_data_tree = (TTree *)hit_data_file->Get("hit_tracker");

    if (hit_data_tree == nullptr)
    {
        return false;
    }

    hit_data_tree->SetBranchAddress("kx",&kx);
    hit_data_tree->SetBranchAddress("ky",&ky);
    hit_data_tree->SetBranchAddress("bx",&bx);
    hit_data_tree->SetBranchAddress("by",&by);
    hit_data_tree->SetBranchAddress("rmse_x",&rmse_x);
    hit_data_tree->SetBranchAddress("rmse_y",&rmse_y);
    hit_data_tree->SetBranchAddress("r2_x",&r2_x);
    hit_data_tree->SetBranchAddress("r2_y",&r2_y);
    hit_data_tree->SetBranchAddress("det_layer_used",&det_layer_used);
    hit_data_tree->SetBranchAddress("is_x_hit",&is_x_hit[0]);
    hit_data_tree->SetBranchAddress("is_y_hit",&is_y_hit[0]);
    hit_data_tree->SetBranchAddress("x",&x[0]);
    hit_data_tree->SetBranchAddress("y",&y[0]);
    hit_data_tree->SetBranchAddress("z",&z[0]);
    hit_data_tree->SetBranchAddress("x_amp",&x_amp[0]);
    hit_data_tree->SetBranchAddress("y_amp",&y_amp[0]);
    hit_data_tree->SetBranchAddress("x_strip_num",&x_strip_num[0]);
    hit_data_tree->SetBranchAddress("y_strip_num",&y_strip_num[0]);

//    m_input_tree->SetBranchAddress("trigger_id_all", &trigger_id);
//    m_input_tree->SetBranchAddress("nhits_all", &nhits);
//    m_input_tree->SetBranchAddress("board_id_all", &board_id[0]);
//    m_input_tree->SetBranchAddress("chip_id_all", &chip_id[0]);
//    m_input_tree->SetBranchAddress("chn_id_all", &chn_id[0]);
//    m_input_tree->SetBranchAddress("adc_data_all", &adc_data[0]);
//    m_input_tree->SetBranchAddress("adc_max_ceil", &adc_max_ceil[0]);
//    m_input_tree->SetBranchAddress("adc_data_sca", &adc_data_sca[0]);

//    for (int i = 0; i < det_layer_used; i++)
//    {
//        string branchname = "detector" + to_string(i);
//        m_input_tree->SetBranchAddress(branchname.data(), detector_data[i]);
//    }

    entries = hit_data_tree->GetEntries();
    // entries = entries > 50000 ? 50000 : entries;

    cout << "Init data file done. Total entries: " << entries << endl;

    return true;
}

bool info_calc::run_amp_sum()
{

    double cDET_IGNORE_SIZE = 5.;  // this is not a global var


    const int det_divide_bin_num = 1;

    // save total amp map of detector
    //TH2F *amp_map[cMAX_DET_LAYER];
    //for (int i = 0; i <det_layer_used ; i++)
    //{
    //    string ampmapname = "amp-map_" + to_string(i);
    //    amp_map[i] = new TH2F(ampmapname.data(), ampmapname.data(), det_divide_bin_num, cDET_IGNORE_SIZE,
    //                          cDET_SIZE[i] - cDET_IGNORE_SIZE,  // 去掉边上5mm
    //                          det_divide_bin_num, cDET_IGNORE_SIZE, cDET_SIZE[i] - cDET_IGNORE_SIZE);  // unit: mm
    //}

    // init amp hist
    TH1F *amp_spectrum[det_layer_used][det_divide_bin_num][det_divide_bin_num];
    for (int i = 0; i < det_layer_used; i++)
    {
        for (int j = 0; j < det_divide_bin_num; j++)
        {
            for (int k = 0; k < det_divide_bin_num; k++)
            {
                string spectrumname = "amp-spectrum_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                amp_spectrum[i][j][k] = new TH1F(spectrumname.data(), spectrumname.data(), 200, 0, 2000);
            }
        }
    }

    // fill amp data
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run amplitude of event: " << evt <<det_layer_used<< endl;
        }

        for (int i = 0; i < det_layer_used; i++)
        {
            
                double det_divide_bin_width = (cDET_SIZE[i] - cDET_IGNORE_SIZE) / det_divide_bin_num;
		
            if (x[i] > 0 && y[i] > 0)
            {
                double xx = x[i];
		double yy = y[i];
                int xbin = (int)((xx - cDET_IGNORE_SIZE) / det_divide_bin_width);
                int ybin = (int)((yy - cDET_IGNORE_SIZE) / det_divide_bin_width);
                if (xbin < 0 || xbin >= det_divide_bin_num)
                    continue;
                if (ybin < 0 || ybin >= det_divide_bin_num)
                    continue;

                double amp_xx = x_amp[i];
		double amp_yy = y_amp[i];
                //double charge = (amp_x) / 28; // fC
                // double charge = (amp_x) / 28; // fC
                //double charge = (amp_y) / 28; // fC
                //double charge = (amp_x + amp_y) / 28;  // fC
                double charge = (amp_xx + amp_yy) / 28;
                amp_spectrum[i][xbin][ybin]->Fill(charge);
	    }
         }
     }

    
    // fit amp data and save to map
    for (int i = 0; i < det_layer_used; i++)
    {
        for (int j = 0; j < det_divide_bin_num; j++)
        {
            for (int k = 0; k < det_divide_bin_num; k++)
            {
                // if (!(i==0&&j==0&&k==2)) continue;
                Double_t fr[2];
                Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
                double mean = amp_spectrum[i][j][k]->GetMean();
                double dev = amp_spectrum[i][j][k]->GetStdDev();
                double max = amp_spectrum[i][j][k]->GetMaximum();
                int max_bin = amp_spectrum[i][j][k]->GetMaximumBin();
                double max_val = amp_spectrum[i][j][k]->GetBinCenter(max_bin);
                // cout << "Mean: " << mean << " StdDev: " << dev << " Max Bin: " << max_val << " Max: " << max << endl;
                fr[0] = 0.3 * max_val;
                fr[1] = 3.0 * mean;

                sv[0] = 0.2 * dev;
                sv[1] = max_val;
                sv[2] = dev * max;
                sv[3] = 0.05 * dev;
                pllo[0] = 0.5;
                pllo[1] = 5.0;
                pllo[2] = 1.0;
                pllo[3] = 0;
                plhi[0] = 5000000.0;
                plhi[1] = 1e7;
                plhi[2] = 1e9;
                plhi[3] = 5000000.0;

                // 应该用朗道卷polya拟合 !!!
                Double_t chisqr;
                Int_t ndf;
                TF1 *fitsnr = langaufit(amp_spectrum[i][j][k], fr, sv, pllo, plhi, fp, fpe, &chisqr, &ndf);

                // save amp fit data to detector map
                //double det_divide_bin_width = (cDET_SIZE[i] - cDET_IGNORE_SIZE) / det_divide_bin_num;
                //amp_map[i]->Fill(j * det_divide_bin_width + cDET_IGNORE_SIZE,
                //                 k * det_divide_bin_width + cDET_IGNORE_SIZE, fp[1]);
                // amp_map[i]->Fill(j*det_divide_bin_width+cDET_IGNORE_SIZE,
                //                  k*det_divide_bin_width+cDET_IGNORE_SIZE,
                //                  mean);

                TCanvas *c = new TCanvas("c", "c", 800, 600);
                amp_spectrum[i][j][k]->Draw();
                fitsnr->Draw("same");
                //list_file::create_path(m_png_path);
                string pngname =
                    m_png_path + "amp-spectrum_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k) + ".png";
                c->Print(pngname.data());
                delete c;
            }
        }
    }


/*
    for (int i = 0; i < det_layer_used; i++)
    {
        TCanvas *c = new TCanvas("c", "c", 800, 600);
        amp_map[i]->SetStats(kFALSE);
        amp_map[i]->GetXaxis()->SetLabelSize(0.05);
        amp_map[i]->GetYaxis()->SetLabelSize(0.05);
        amp_map[i]->Draw("colz");
        for (int j = 1; j < amp_map[i]->GetNbinsX() + 1; j++)
        {
            for (int k = 1; k < amp_map[i]->GetNbinsY() + 1; k++)
            {
                auto t = new TText(amp_map[i]->GetXaxis()->GetBinCenter(j), amp_map[i]->GetYaxis()->GetBinCenter(k),
                                   Form("%.1f", amp_map[i]->GetBinContent(j, k)));
                t->SetTextAlign(22);
                t->SetTextSize(0.05);
                t->Draw();
            }
        }
        list_file::create_path(m_png_path);
        string pngname = m_png_path + "amp-sum_" + to_string(i) + ".png";
        c->Print(pngname.data());
        delete c;

        // 均匀性RMS/Mean
        double mean = 0;
        double rms = 0;
        int nbins = 0;
        for (int x = 1; x < amp_map[i]->GetNbinsX() + 1; x++)
        {
            for (int y = 1; y < amp_map[i]->GetNbinsY() + 1; y++)
            {
                double amp = amp_map[i]->GetBinContent(x, y);
                mean += amp;
                nbins++;
            }
        }
        mean = mean / nbins;
        for (int x = 1; x < amp_map[i]->GetNbinsX() + 1; x++)
        {
            for (int y = 1; y < amp_map[i]->GetNbinsY() + 1; y++)
            {
                double amp = amp_map[i]->GetBinContent(x, y);
                rms += (amp - mean) * (amp - mean);
            }
        }
        rms = sqrt(rms / nbins);
        cout << "det-" << i << " rms: " << rms << " mean: " << mean << " rms/mean: " << rms / mean << endl;
    }
*/
    for (int i = 0; i < det_layer_used; i++)
    {
        for (int j = 0; j < det_divide_bin_num; j++)
        {
            for (int k = 0; k < det_divide_bin_num; k++)
            {
                if (amp_spectrum[i][j][k] != nullptr)
                {
                    delete amp_spectrum[i][j][k];
                    amp_spectrum[i][j][k] = nullptr;
                }
            }
        }
    }
/*
    for (int i = 0; i < det_layer_used; i++)
    {
        if (amp_map[i] != nullptr)
        {
            delete amp_map[i];
            amp_map[i] = nullptr;
        }
    }
*/
    return true;
}



bool info_calc::run_eff_calc()
{
    // settings
    const int det_divide_bin_num = 1;
    double det_divide_bin_width = 400 / det_divide_bin_num;

    // save detector hit counter
    TH2F *hits_exp_map[cMAX_DET_LAYER];
    TH2F *hits_real_map[cMAX_DET_LAYER];
    for (int i = 0; i < det_layer_used; i++)
    {
        string hitsexpmapname = "hits-exp-map_" + to_string(i);
        hits_exp_map[i] = new TH2F(hitsexpmapname.data(), hitsexpmapname.data(), det_divide_bin_num, 0, 400,
                                   det_divide_bin_num, 0, 400);  // unit: mm
        string hitsrealmapname = "hits-real-map_" + to_string(i);
        hits_real_map[i] = new TH2F(hitsrealmapname.data(), hitsrealmapname.data(), det_divide_bin_num, 0, 400,
                                    det_divide_bin_num, 0, 400);  // unit: mm
    }

    //m_correct->set_use_alignment(false);
    //m_correct->set_rmse_cut(10000);
    //m_correct->set_ndet_num_cut(cDET_LAYER);
    //m_correct->set_theta_cut(90. / 180 * 3.1415);
    //m_correct->set_hit_res_cut(10000);

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        //m_correct->set_evt_data(detector_data);
        //if (m_correct->correct() != 10)
        //    continue;

        for (int i = 0; i < det_layer_used; i++)
        {
            //vector<double> posx = x[i];
            //vector<double> posy = y[i];
            //vector<double> posz = z[i];
            //vector<int> idx_det = det_layer_used;
            //int N = idx_det.size();

            //bool b_have_hit = false;
            //int hit_idx_in_vector = -1;
            //for (int j = 0; j < N; j++)
            //{
            //    if (idx_det[j] == i)
            //    {
            //        b_have_hit = true;
            //        hit_idx_in_vector = j;
            //        break;
            //    }
            //}

            //if (b_have_hit)
            //{
            //    posx.erase(posx.begin() + hit_idx_in_vector);
            //    posy.erase(posy.begin() + hit_idx_in_vector);
            //    posz.erase(posz.begin() + hit_idx_in_vector);
            //    idx_det.erase(idx_det.begin() + hit_idx_in_vector);
            //}

            //double kzx, bzx, rmse_zx;
            //double kzy, bzy, rmse_zy;
            //if (idx_det.size() == 2)
            //{
            //    kzx = (posx[0] - posx[1]) / (posz[0] - posz[1]);
            //    bzx = (posz[0] * posx[0] - posz[0] * posx[1]) / (posz[1] - posz[0]);  // 有问题
            //    kzy = (posy[0] - posy[1]) / (posz[0] - posz[1]);
            //    bzy = (posz[0] * posy[0] - posz[0] * posy[1]) / (posz[1] - posz[0]);
            //}
            //else if (idx_det.size() >= 3)
            //{
                //hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx);
                //hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy);
            //}

            // target layer expected position
            double posx_fit = kx * z[i] + bx;
            double posy_fit = ky * z[i] + by;
	    //std::cout<<"layer: "<<i<<"   x-fit:  "<<posx_fit<<"   y-fit:  "<<posy_fit<<std::endl;

            if (posx_fit > 0 && posx_fit < 400 && posy_fit > 0 && posy_fit < 400)
            {
                hits_exp_map[i]->Fill(posx_fit, posy_fit);

                if (is_x_hit[i] && is_y_hit[i])
                {
                    double delta_x = posx_fit - x[i];
                    double delta_y = posy_fit - y[i];
		    //std::cout<<"layer: "<<i<<"   delta_x:  "<<delta_x<<"   delta_y:  "<<delta_y<<std::endl;

                    if (abs(delta_x) < 20 && abs(delta_y) < 20)
                    {
                        hits_real_map[i]->Fill(x[i], y[i]);
                    }
                }
            }
        }
    }

    // save detector efficient
    TH2F *eff_map[cMAX_DET_LAYER];
    for (int i = 0; i < det_layer_used; i++)
    {
        string effmapname = "eff-map_" + to_string(i);
        eff_map[i] = new TH2F(effmapname.data(), effmapname.data(), det_divide_bin_num, 0, 400, det_divide_bin_num, 0,
                              400);  // unit: mm
    }

    for (int i = 0; i < det_layer_used; i++)
    {
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            for (int k = 1; k <= det_divide_bin_num; k++)
            {
                int real_hits = hits_real_map[i]->GetBinContent(j, k);
                int exp_hits = hits_exp_map[i]->GetBinContent(j, k);
                eff_map[i]->SetBinContent(j, k, (double)real_hits / (double)exp_hits);
            }
        }
    }

    for (int i = 0; i < det_layer_used; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        eff_map[i]->SetStats(kFALSE);
        eff_map[i]->Draw("colztext");
        //list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;
    }
    
    for (int i = 0; i < det_layer_used; i++)
    {
        delete hits_exp_map[i];
        delete hits_real_map[i];
        delete eff_map[i];
    }

    return true;
}


bool info_calc::run_pos_res()
{
    // show res info of each detector
    TH1F *pos_res_x_hist[cMAX_DET_LAYER];
    TH1F *pos_res_y_hist[cMAX_DET_LAYER];
    //TH1F *rmse_x_hist[cMAX_DET_LAYER];
    //TH1F *rmse_y_hist[cMAX_DET_LAYER];
    for (int i = 0; i < det_layer_used; i++)
    {
        string namex = "x" + to_string(i);
        string namey = "y" + to_string(i);
        //string namerx = "rmsex" + to_string(i);
        //string namery = "rmsey" + to_string(i);
        pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -20, 20);
        pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -20, 20);
        //rmse_x_hist[i] = new TH1F(namerx.data(), namerx.data(), 100, 0, 100);
        //rmse_y_hist[i] = new TH1F(namery.data(), namery.data(), 100, 0, 100);
    }

    double pos_res_x[cMAX_DET_LAYER] = {0};
    double pos_res_y[cMAX_DET_LAYER] = {0};
    double eff_cnt_x[cMAX_DET_LAYER][2] = {0};
    double eff_cnt_y[cMAX_DET_LAYER][2] = {0};

    //m_correct->set_use_alignment(false);
    //m_correct->set_rmse_cut(10000);
    //m_correct->set_ndet_num_cut(4);
    //m_correct->set_theta_cut(90. / 180 * 3.1415);
    //m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 12.0;
    double fit_rmse_cut = 12.0;
    double fit_R2_cut = 0.5;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        //m_correct->set_evt_data(detector_data);
        //if (m_correct->correct() != 10)
        //    continue;

        //int N = m_correct->idx_det.size();  // layer counter of this event
        for (int n = 0; n < det_layer_used; n++)
        {
            //vector<double> posx = m_correct->posx;
            //vector<double> posy = m_correct->posy;
            //vector<double> posz = m_correct->posz;
            //vector<int> idx_det = m_correct->idx_det;

            //posx.erase(posx.begin() + n);
            //posy.erase(posy.begin() + n);
            //posz.erase(posz.begin() + n);
            //idx_det.erase(idx_det.begin() + n);

            //double kzx, bzx, rmse_zx, R2_zx;
            //double kzy, bzy, rmse_zy, R2_zy;
            //vector<double> posx_tmp, posy_tmp, posz_tmp;

            // hit res cut
            //bool b_hit_res_flag = false;
            //for (int i = 0; i < posx.size(); i++)
            //
            //    posx_tmp = posx, posy_tmp = posy, posz_tmp = posz;
            //    posx_tmp.erase(posx_tmp.begin() + i);
            //    posy_tmp.erase(posy_tmp.begin() + i);
            //    posz_tmp.erase(posz_tmp.begin() + i);
            //    if (!hit_correct::least_square_fit(posz_tmp, posx_tmp, kzx, bzx, rmse_zx)
            //        || !hit_correct::least_square_fit(posz_tmp, posy_tmp, kzy, bzy, rmse_zy))
            //    {
            //        b_hit_res_flag = true;
            //        break;
            //    }
            //double res_zx = abs(x[n] - kx * z[n] - bx);
            //double res_zy = abs(y[n] - ky * z[n] - by);
            //if (res_zx > fit_hit_res_cut || res_zy > fit_hit_res_cut)
            //    {
            //        b_hit_res_flag = true;
            //        break;
            //    }
            
            //if (b_hit_res_flag)
            //{
            //    continue;
            //}

            // rmse cut
            //if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx)
            //    || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy))
            //{
            //    continue;
            //}
            //if (rmse_x > fit_rmse_cut || rmse_y > fit_rmse_cut)
            //{
            //    continue;
            //}

            // R2 cut
            //if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx)
            //    || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            //{
            //    continue;
            //}
            //if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            //{
            //    continue;
            //}

            // save res
	    if(is_x_hit[n] && is_y_hit[n])
	    {
            double posx_fit = kx * z[n] + bx;
            double delta_x = posx_fit - x[n];
            pos_res_x_hist[n]->Fill(delta_x);
            double posy_fit = ky * z[n] + by;
            double delta_y = posy_fit - y[n];
            pos_res_y_hist[n]->Fill(delta_y);
            
            //rmse_x_hist[n]->Fill(rmse_x);
	    //rmse_x_hist[n]->Fill(rmse_y);

            // debug
            // if (m_correct->idx_det[n] == 1
            //     && abs(delta_x) > 0.15)
            // {
            //     m_correct->print_png();
            // }

            // efficiency calc
            eff_cnt_x[n][0]++;
            eff_cnt_y[n][0]++;
            if (abs(delta_x) < 2)
            {
                eff_cnt_x[n][1]++;
            }
            if (abs(delta_y) < 2)
            {
                eff_cnt_y[n][1]++;
            }
	    }
        }
     }
    

    // cout efficiency
     for (int i = 0; i < det_layer_used; i++)
     {
        cout << "Efficiency X" << i << "=" << eff_cnt_x[i][1] / eff_cnt_x[i][0] << " ";
        cout << "Efficiency Y" << i << "=" << eff_cnt_y[i][1] / eff_cnt_y[i][0] << " ";
        cout << endl;
     }
    


    // calc the sigma of residual
    for (int i = 0; i < det_layer_used; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
            // TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -4, 4);
            // fitx->SetParameter(0, 50);
            // fitx->SetParameter(1, max_pos_x);
            // fitx->SetParameter(2, 0.4);
            // fitx->SetParLimits(2, 0, 1.0);
            // fitx->SetParameter(3, 5);
            // fitx->SetParameter(4, max_pos_x);
            // fitx->SetParameter(5, 2.5);
            // fitx->SetParLimits(5, 1.0, 5.0);
            // pos_res_x_hist[i]->Fit(fitx, "Q");
            // pos_res_x_hist[i]->Fit("gaus", "Q");
            pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x - 2, max_pos_x + 2);
            // pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x-0.3, max_pos_x+0.3);
            TF1 *fitx = pos_res_x_hist[i]->GetFunction("gaus");
            pos_res_x[i] = fitx->GetParameter(2);
            //alignment[i][0] += fitx->GetParameter(1);
            //cout << "x" << i << ": " << pos_res_x[i] << endl;
            // delete fitx;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
            // TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -4, 4);
            // fity->SetParameter(0, 50);
            // fity->SetParameter(1, max_pos_y);
            // fity->SetParameter(2, 0.4);
            // fity->SetParLimits(2, 0, 1.0);
            // fity->SetParameter(3, 5);
            // fity->SetParameter(4, max_pos_y);
            // fity->SetParameter(5, 2.5);
            // fity->SetParLimits(5, 1.0, 5.0);
            // pos_res_y_hist[i]->Fit(fity, "Q");
            // pos_res_y_hist[i]->Fit("gaus", "Q");
            pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y - 2, max_pos_y + 2);
            // pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y-0.3, max_pos_y+0.3);
            TF1 *fity = pos_res_y_hist[i]->GetFunction("gaus");
            pos_res_y[i] = fity->GetParameter(2);
            //alignment[i][1] += fity->GetParameter(1);
            //cout << "y" << i << ": " << pos_res_y[i] << endl;
            // delete fity;
        }
    }

    for (int i = 0; i < det_layer_used; i++)
    {
        //if (pos_res_x_hist[i]->GetEntries() > 0)
        //{
            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_x_hist[i]->Draw();
            //pos_res_x_hist[i]->GetFunction("gaus")->Draw("same");
            // pos_res_x_hist[i]->GetFunction("fitx")->Draw("same");
            //list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "detector_res_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;
        //}

        //if (pos_res_y_hist[i]->GetEntries() > 0)
        //{
            TCanvas *c2 = new TCanvas("c", "c", 800, 600);
            pos_res_y_hist[i]->Draw();
            //pos_res_y_hist[i]->GetFunction("gaus")->Draw("same");
            // pos_res_y_hist[i]->GetFunction("fity")->Draw("same");
            string pngname2 = m_png_path + "detector_res_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        //}
    }

    double detector_id[6] = {0, 1, 2, 3, 4, 5};
    TGraph *pos_res_graphx = new TGraph(6, detector_id, pos_res_x);
    pos_res_graphx->GetXaxis()->SetTitle("detector_id");
    pos_res_graphx->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c1 = new TCanvas("c", "c", 800, 600);
    pos_res_graphx->Draw();
    //list_file::create_path(m_png_path);
    string pngname1 = m_png_path + "detector_res_x.png";
    c1->Print(pngname1.data());
    delete c1;
    delete pos_res_graphx;

    TGraph *pos_res_graphy = new TGraph(6, detector_id, pos_res_y);
    pos_res_graphy->GetXaxis()->SetTitle("detector_id");
    pos_res_graphy->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c2 = new TCanvas("c", "c", 800, 600);
    pos_res_graphy->Draw();
    string pngname2 = m_png_path + "detector_res_y.png";
    c2->Print(pngname2.data());
    delete c2;
    delete pos_res_graphy;

    for (int i = 0; i < det_layer_used; i++)
    {
        delete pos_res_x_hist[i];
        delete pos_res_y_hist[i];
    }

    return true;
}













/*
bool info_calc::run_eff_calc_use_changed_correct()
{
    // settings
    const int det_divide_bin_num = 5;
    double det_divide_bin_width = 150. / det_divide_bin_num;

    // save detector hit counter
    TH2F *hits_exp_map[cDET_LAYER_MAX];
    TH2F *hits_real_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string hitsexpmapname = "hits-exp-map_" + to_string(i);
        hits_exp_map[i] = new TH2F(hitsexpmapname.data(), hitsexpmapname.data(), det_divide_bin_num, 0, 150,
                                   det_divide_bin_num, 0, 150);  // unit: mm
        string hitsrealmapname = "hits-real-map_" + to_string(i);
        hits_real_map[i] = new TH2F(hitsrealmapname.data(), hitsrealmapname.data(), det_divide_bin_num, 0, 150,
                                    det_divide_bin_num, 0, 150);  // unit: mm
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(4);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        for (int i = 0; i < cDET_LAYER; i++)
        {
            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;
            int N = idx_det.size();

            bool b_have_hit = false;
            int hit_idx_in_vector = -1;
            for (int j = 0; j < N; j++)
            {
                if (idx_det[j] == i)
                {
                    b_have_hit = true;
                    hit_idx_in_vector = j;
                    break;
                }
            }

            if (b_have_hit)
            {
                posx.erase(posx.begin() + hit_idx_in_vector);
                posy.erase(posy.begin() + hit_idx_in_vector);
                posz.erase(posz.begin() + hit_idx_in_vector);
                idx_det.erase(idx_det.begin() + hit_idx_in_vector);
            }

            double kzx, bzx, rmse_zx;
            double kzy, bzy, rmse_zy;
            if (idx_det.size() == 2)
            {
                kzx = (posx[0] - posx[1]) / (posz[0] - posz[1]);
                bzx = (posz[0] * posx[0] - posz[0] * posx[1]) / (posz[1] - posz[0]);
                kzy = (posy[0] - posy[1]) / (posz[0] - posz[1]);
                bzy = (posz[0] * posy[0] - posz[0] * posy[1]) / (posz[1] - posz[0]);
            }
            else if (idx_det.size() >= 3)
            {
                hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx);
                hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy);
            }

            // target layer expected position
            double posx_fit = kzx * cDET_Z[i] + bzx;
            double posy_fit = kzy * cDET_Z[i] + bzy;

            if (posx_fit > 0 && posx_fit < 150 && posy_fit > 0 && posy_fit < 150)
            {
                hits_exp_map[i]->Fill(posx_fit, posy_fit);

                if (b_have_hit)
                {
                    double delta_x = posx_fit - m_correct->posx[hit_idx_in_vector];
                    double delta_y = posy_fit - m_correct->posy[hit_idx_in_vector];

                    if (abs(delta_x) < 2 && abs(delta_y) < 2)
                    {
                        hits_real_map[i]->Fill(posx_fit, posy_fit);
                    }
                }
            }
        }
    }

    // save detector efficient
    TH2F *eff_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string effmapname = "eff-map_" + to_string(i);
        eff_map[i] = new TH2F(effmapname.data(), effmapname.data(), det_divide_bin_num, 0, 150, det_divide_bin_num, 0,
                              150);  // unit: mm
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            for (int k = 1; k <= det_divide_bin_num; k++)
            {
                int real_hits = hits_real_map[i]->GetBinContent(j, k);
                int exp_hits = hits_exp_map[i]->GetBinContent(j, k);
                eff_map[i]->SetBinContent(j, k, (double)real_hits / (double)exp_hits);
            }
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        eff_map[i]->SetStats(kFALSE);
        eff_map[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;
    }

    return true;
}

bool info_calc::run_eff_calc_notuse_correct()
{
    // settings
    const int det_divide_bin_num = 10;
    // double det_divide_bin_width = cDET_SIZE / det_divide_bin_num;

    // save detector hit counter
    TH1F *hits_exp_map_x[cDET_LAYER_MAX];
    TH1F *hits_exp_map_y[cDET_LAYER_MAX];
    TH1F *hits_real_map_x[cDET_LAYER_MAX];
    TH1F *hits_real_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string hitsexpmapxname = "hits-exp-map-x_" + to_string(i);
        hits_exp_map_x[i] =
            new TH1F(hitsexpmapxname.data(), hitsexpmapxname.data(), det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsexpmapyname = "hits-exp-map-y_" + to_string(i);
        hits_exp_map_y[i] =
            new TH1F(hitsexpmapyname.data(), hitsexpmapyname.data(), det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapxname = "hits-real-map-x_" + to_string(i);
        hits_real_map_x[i] = new TH1F(hitsrealmapxname.data(), hitsrealmapxname.data(), det_divide_bin_num, 0,
                                      cDET_SIZE[i]);  // unit: mm
        string hitsrealmapyname = "hits-real-map-y_" + to_string(i);
        hits_real_map_y[i] = new TH1F(hitsrealmapyname.data(), hitsrealmapyname.data(), det_divide_bin_num, 0,
                                      cDET_SIZE[i]);  // unit: mm
    }

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1)
            {
                x[i] = detector_data[i]->x;
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;

                // m_correct->alignment_data(x[i], y[i], z[i], i);
            }
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            // zx
            int ref_cnt_x = 0;
            double ref_zxx[cDET_LAYER_MAX] = {0};
            double ref_zxz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (x[j] > 0)
                {
                    ref_zxx[ref_cnt_x] = x[j];
                    ref_zxz[ref_cnt_x] = z[j];
                    ref_cnt_x++;
                }
            }
            if (ref_cnt_x == cDET_LAYER - 1)
            {
                double kzx, bzx, rmse_zx, R2_zx;
                hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
                if (rmse_zx < 10 && R2_zx > 0.99)
                {
                    double fit_x = kzx * cDET_Z[i] + bzx;
                    if (fit_x > 0 && fit_x < cDET_SIZE[i])
                    {
                        hits_exp_map_x[i]->Fill(fit_x);

                        if (x[i] > 0 && abs(x[i] - fit_x) < 5)
                        {
                            hits_real_map_x[i]->Fill(fit_x);
                        }
                        // else if (x[i] > -1000 && x[i] < 0)
                        // {
                        //     int code = (-x[i])-1;
                        //     int encode = code % 100;
                        //     int chip_idx = code / 100;

                        //     vector<int> decode_line =
                        //         m_correct->get_encoding_line(encode);
                        //     for (int k=0; k<decode_line.size(); k++)
                        //     {
                        //         double decode_x = (decode_line[k]
                        //                            + chip_idx * cDECODED_CHNNUM
                        //                            + 1) * cSTRIP_WIDTH;
                        //         if (abs(decode_x - fit_x) < 5)
                        //         {
                        //             hits_real_map_x[i]->Fill(fit_x);
                        //             break;
                        //         }
                        //     }
                        // }
                    }
                }
            }

            // zy
            int ref_cnt_y = 0;
            double ref_zyy[cDET_LAYER_MAX] = {0};
            double ref_zyz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (y[j] > 0)
                {
                    ref_zyy[ref_cnt_y] = y[j];
                    ref_zyz[ref_cnt_y] = z[j];
                    ref_cnt_y++;
                }
            }
            if (ref_cnt_y == cDET_LAYER - 1)
            {
                double kzy, bzy, rmse_zy, R2_zy;
                hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
                if (rmse_zy < 10 && R2_zy > 0.99)
                {
                    double fit_y = kzy * cDET_Z[i] + bzy;
                    if (fit_y > 0 && fit_y < cDET_SIZE[i])
                    {
                        hits_exp_map_y[i]->Fill(fit_y);

                        if (y[i] > 0 && abs(y[i] - fit_y) < 5)
                        {
                            hits_real_map_y[i]->Fill(fit_y);
                        }
                        // else if (y[i] > -1000 && y[i] < 0)
                        // {
                        //     int code = (-y[i])-1;
                        //     int encode = code % 100;
                        //     int chip_idx = code / 100;

                        //     vector<int> decode_line =
                        //         m_correct->get_encoding_line(encode);
                        //     for (int k=0; k<decode_line.size(); k++)
                        //     {
                        //         double decode_y = (decode_line[k]
                        //                            + chip_idx * cDECODED_CHNNUM
                        //                            + 1) * cSTRIP_WIDTH;
                        //         if (abs(decode_y - fit_y) < 5)
                        //         {
                        //             hits_real_map_y[i]->Fill(fit_y);
                        //             break;
                        //         }
                        //     }
                        // }
                    }
                }
            }
        }
    }

    // save detector efficient
    TH1F *eff_map_x[cDET_LAYER_MAX];
    TH1F *eff_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string effmapxname = "eff-map-x_" + to_string(i);
        eff_map_x[i] =
            new TH1F(effmapxname.data(), effmapxname.data(), det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string effmapyname = "eff-map-y_" + to_string(i);
        eff_map_y[i] =
            new TH1F(effmapyname.data(), effmapyname.data(), det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        double eff_x = 0, eff_y = 0;
        int exp_x = 0, exp_y = 0;
        int real_x = 0, real_y = 0;
        int nbins = 0;
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            int hits_exp_x = hits_exp_map_x[i]->GetBinContent(j);
            int hits_real_x = hits_real_map_x[i]->GetBinContent(j);
            if (hits_exp_x > 0)
            {
                eff_map_x[i]->SetBinContent(j, (double)hits_real_x / (double)hits_exp_x);
            }
            eff_x += (double)hits_real_x / (double)hits_exp_x;
            exp_x += hits_exp_x;
            real_x += hits_real_x;

            int hits_exp_y = hits_exp_map_y[i]->GetBinContent(j);
            int hits_real_y = hits_real_map_y[i]->GetBinContent(j);
            if (hits_exp_y > 0)
            {
                eff_map_y[i]->SetBinContent(j, (double)hits_real_y / (double)hits_exp_y);
            }
            eff_y += (double)hits_real_y / (double)hits_exp_y;
            exp_y += hits_exp_y;
            real_y += hits_real_y;

            nbins++;
        }
        eff_x = eff_x / nbins;
        eff_y = eff_y / nbins;
        cout << "Layer " << i << endl;
        cout << "X exp: " << exp_x << " real: " << real_x << " eff: " << eff_x << endl;
        cout << "Y exp: " << exp_y << " real: " << real_y << " eff: " << eff_y << endl;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        eff_map_x[i]->SetStats(kFALSE);
        eff_map_x[i]->Draw();
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        eff_map_y[i]->SetStats(kFALSE);
        eff_map_y[i]->Draw();
        string pngname2 = m_png_path + "eff_map_y" + to_string(i) + ".png";
        c1->Print(pngname2.data());

        delete c1;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        hits_exp_map_x[i]->SetStats(kFALSE);
        hits_exp_map_x[i]->Draw();
        string pngname1 = m_png_path + "hits_exp_map_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        hits_exp_map_y[i]->SetStats(kFALSE);
        hits_exp_map_y[i]->Draw();
        string pngname2 = m_png_path + "hits_exp_map_y" + to_string(i) + ".png";
        c1->Print(pngname2.data());

        delete c1;
    }
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        hits_real_map_x[i]->SetStats(kFALSE);
        hits_real_map_x[i]->Draw();
        string pngname1 = m_png_path + "hits_real_map_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        hits_real_map_y[i]->SetStats(kFALSE);
        hits_real_map_y[i]->Draw();
        string pngname2 = m_png_path + "hits_real_map_y" + to_string(i) + ".png";
        c1->Print(pngname2.data());

        delete c1;
    }

    return true;
}

bool info_calc::run_eff_calc_notuse_correct_combine_xy()
{
    // settings
    const int det_divide_bin_num = 10;
    // double det_divide_bin_width = cDET_SIZE / det_divide_bin_num;

    // save detector hit counter
    TH2F *hits_exp_map[cDET_LAYER_MAX];
    TH2F *hits_real_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string hitsexpmapname = "hits-exp-map_" + to_string(i);
        hits_exp_map[i] = new TH2F(hitsexpmapname.data(), hitsexpmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                                   det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapname = "hits-real-map_" + to_string(i);
        hits_real_map[i] = new TH2F(hitsrealmapname.data(), hitsrealmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                                    det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1)
            {
                x[i] = detector_data[i]->x;
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;

                // m_correct->alignment_data(x[i], y[i], z[i], i);
            }
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            int ref_cnt_x = 0;
            double ref_zxx[cDET_LAYER_MAX] = {0};
            double ref_zxz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (x[j] > 0)
                {
                    ref_zxx[ref_cnt_x] = x[j];
                    ref_zxz[ref_cnt_x] = z[j];
                    ref_cnt_x++;
                }
            }

            int ref_cnt_y = 0;
            double ref_zyy[cDET_LAYER_MAX] = {0};
            double ref_zyz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (y[j] > 0)
                {
                    ref_zyy[ref_cnt_y] = y[j];
                    ref_zyz[ref_cnt_y] = z[j];
                    ref_cnt_y++;
                }
            }

            if (ref_cnt_x == cDET_LAYER - 1 && ref_cnt_y == cDET_LAYER - 1)
            {
                double kzx, bzx, rmse_zx, R2_zx;
                hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
                double kzy, bzy, rmse_zy, R2_zy;
                hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
                if (rmse_zx < 1 && rmse_zy < 1)
                {
                    double fit_x = kzx * cDET_Z[i] + bzx;
                    double fit_y = kzy * cDET_Z[i] + bzy;
                    if (fit_x > 0 && fit_x < cDET_SIZE[i] && fit_y > 0 && fit_y < cDET_SIZE[i])
                    {
                        hits_exp_map[i]->Fill(fit_x, fit_y);

                        if (x[i] > 0 && y[i] > 0 && abs(x[i] - fit_x) < 5 && abs(y[i] - fit_y) < 5)
                        {
                            hits_real_map[i]->Fill(fit_x, fit_y);
                        }
                    }
                }
            }
        }
    }

    // save detector efficient
    TH2F *eff_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string effmapname = "eff-map_" + to_string(i);
        eff_map[i] = new TH2F(effmapname.data(), effmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                              det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        double mean = 0;
        int nbins = 0;
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            for (int k = 1; k <= det_divide_bin_num; k++)
            {
                int hits_exp = hits_exp_map[i]->GetBinContent(j, k);
                int hits_real = hits_real_map[i]->GetBinContent(j, k);
                if (hits_exp > 0)
                {
                    eff_map[i]->SetBinContent(j, k, (double)hits_real / (double)hits_exp);
                }

                mean += (double)hits_real / (double)hits_exp;
                nbins++;
            }
        }
        mean = mean / nbins;

        cout << "det-" << i << " eff: " << mean << endl;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        eff_map[i]->SetStats(kFALSE);
        eff_map[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        delete c1;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        hits_exp_map[i]->SetStats(kFALSE);
        hits_exp_map[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_exp_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        delete c1;
    }
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        hits_real_map[i]->SetStats(kFALSE);
        hits_real_map[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_real_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        delete c1;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete hits_exp_map[i];
        delete hits_real_map[i];
        delete eff_map[i];
    }

    return true;
}

bool info_calc::run_eff_calc_notuse_correct_combine_xy_split()
{
    // settings
    const int det_divide_bin_num = 8;
    // double det_divide_bin_width = cDET_SIZE / det_divide_bin_num;

    // position resolution  ------------------------------------------
    // show res info of each detector
    TH1F *pos_res_x_hist[cDET_LAYER_MAX];
    TH1F *pos_res_y_hist[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string namex = "x_" + to_string(i);
        string namey = "y_" + to_string(i);
        pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -10, 10);
        pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -10, 10);
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(cDET_LAYER);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 5.0;
    double fit_rmse_cut = 2.0;
    double fit_R2_cut = 0.8;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        int N = m_correct->idx_det.size();  // layer counter of this event
        // cout << "detector of hits: " << N << endl;
        for (int n = 0; n < N; n++)
        {
            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;

            int layer = idx_det[n];

            posx.erase(posx.begin() + n);
            posy.erase(posy.begin() + n);
            posz.erase(posz.begin() + n);
            idx_det.erase(idx_det.begin() + n);

            double kzx, bzx, rmse_zx, R2_zx;
            double kzy, bzy, rmse_zy, R2_zy;
            vector<double> posx_tmp, posy_tmp, posz_tmp;

            // hit res cut
            // bool b_hit_res_flag = false;
            // for (int i=0; i<posx.size(); i++)
            // {
            //     posx_tmp = posx, posy_tmp = posy, posz_tmp = posz;
            //     posx_tmp.erase(posx_tmp.begin()+i);
            //     posy_tmp.erase(posy_tmp.begin()+i);
            //     posz_tmp.erase(posz_tmp.begin()+i);
            //     if (!hit_correct::least_square_fit(posz_tmp, posx_tmp, kzx, bzx, rmse_zx) ||
            //         !hit_correct::least_square_fit(posz_tmp, posy_tmp, kzy, bzy, rmse_zy))
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            //     double res_zx = abs(posx[i]-kzx*posz[i]-bzx);
            //     double res_zy = abs(posy[i]-kzy*posz[i]-bzy);
            //     if (res_zx > fit_hit_res_cut || res_zy > fit_hit_res_cut)
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            // }
            // if (b_hit_res_flag)
            // {
            //     continue;
            // }

            // rmse cut
            // R2 cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            {
                continue;
            }
            if (rmse_zx > fit_rmse_cut || rmse_zy > fit_rmse_cut)
            {
                continue;
            }
            if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            {
                continue;
            }

            // save res
            double posx_fit = kzx * m_correct->posz[n] + bzx;
            double delta_x = posx_fit - m_correct->posx[n];
            double anglezx = abs(atan(kzx)) / TMath::Pi() * 180;
            if (anglezx < 30)
                pos_res_x_hist[layer]->Fill(delta_x);

            double posy_fit = kzy * m_correct->posz[n] + bzy;
            double delta_y = posy_fit - m_correct->posy[n];
            double anglezy = abs(atan(kzy)) / TMath::Pi() * 180;
            if (anglezy < 30)
                pos_res_y_hist[layer]->Fill(delta_y);
        }
    }

    double pos_res_x[cDET_LAYER_MAX] = {0};
    double pos_res_y[cDET_LAYER_MAX] = {0};
    for (int i = 0; i < cDET_LAYER; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            // 双高斯拟合
            double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
            TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -10, 10);
            fitx->SetParameter(0, 50);
            fitx->SetParameter(1, max_pos_x);
            fitx->SetParameter(2, 0.4);
            fitx->SetParLimits(2, 0, 2.0);
            fitx->SetParameter(3, 5);
            fitx->SetParameter(4, max_pos_x);
            fitx->SetParameter(5, 5);
            fitx->SetParLimits(5, 1.0, 20.0);
            pos_res_x_hist[i]->Fit(fitx, "Q");
            pos_res_x[i] = fitx->GetParameter(2);
            cout << i << "x: " << pos_res_x[i] << endl;

            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_x_hist[i]->Draw();
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "detector_res_x_test2_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;

            delete fitx;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            // 双高斯拟合
            double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
            TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -10, 10);
            fity->SetParameter(0, 50);
            fity->SetParameter(1, max_pos_y);
            fity->SetParameter(2, 0.4);
            fity->SetParLimits(2, 0, 2.0);
            fity->SetParameter(3, 5);
            fity->SetParameter(4, max_pos_y);
            fity->SetParameter(5, 5);
            fity->SetParLimits(5, 1.0, 20.0);
            pos_res_y_hist[i]->Fit(fity, "Q");
            pos_res_y[i] = fity->GetParameter(2);
            cout << i << "y: " << pos_res_y[i] << endl;

            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_y_hist[i]->Draw();
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "detector_res_y_test2_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;

            delete fity;
        }
    }

    {
        // get detector position resolution ------------------------------
        TH1F *pos_res_x_hist[cDET_LAYER_MAX];
        TH1F *pos_res_y_hist[cDET_LAYER_MAX];
        for (int i = 0; i < cDET_LAYER; i++)
        {
            string namex = "x_" + to_string(i);
            string namey = "y_" + to_string(i);
            pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -10, 10);
            pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -10, 10);
        }

        // m_correct->set_use_alignment(false);
        // m_correct->set_rmse_cut(10000);
        // m_correct->set_ndet_num_cut(cDET_LAYER);
        // m_correct->set_theta_cut(90./180*3.1415);
        // m_correct->set_hit_res_cut(10000);

        // fill hit counter
        for (int evt = 0; evt < entries; evt++)
        {
            hit_data_tree->GetEntry(evt);
            if (evt % 10000 == 0)
            {
                cout << "Run efficient of event: " << evt << endl;
            }

            // m_correct->set_evt_data(detector_data);
            // if (m_correct->correct() != 10) continue;
            // int N = m_correct->idx_det.size(); // layer counter of this event

            double x[cDET_LAYER_MAX] = {0};
            double y[cDET_LAYER_MAX] = {0};
            double z[cDET_LAYER_MAX] = {0};

            // for (int i=0; i<cDET_LAYER; i++)
            // {
            //     x[i] = -1000;
            //     y[i] = -1000;
            // }
            // for (int n=0; n<N; n++)
            // {
            //     vector<double> posx = m_correct->posx;
            //     vector<double> posy = m_correct->posy;
            //     vector<double> posz = m_correct->posz;
            //     vector<int> idx_det = m_correct->idx_det;

            //     int layer = idx_det[n];
            //     x[layer] = posx[n];
            //     y[layer] = posy[n];
            //     z[layer] = posz[n];
            // }

            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig_x != 0)
                {
                    x[i] = detector_data[i]->x;
                    z[i] = detector_data[i]->z;
                }
                else
                {
                    x[i] = -1000;
                }
                if (detector_data[i]->sig_y != 0)
                {
                    y[i] = detector_data[i]->y;
                    z[i] = detector_data[i]->z;
                }
                else
                {
                    y[i] = -1000;
                }
            }

            for (int i = 0; i < cDET_LAYER; i++)
            {
                int ref_cnt_x = 0;
                double ref_zxx[cDET_LAYER_MAX] = {0};
                double ref_zxz[cDET_LAYER_MAX] = {0};
                for (int j = 0; j < cDET_LAYER; j++)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    if (x[j] != -1000)
                    {
                        ref_zxx[ref_cnt_x] = x[j];
                        ref_zxz[ref_cnt_x] = z[j];
                        ref_cnt_x++;
                    }
                }

                int ref_cnt_y = 0;
                double ref_zyy[cDET_LAYER_MAX] = {0};
                double ref_zyz[cDET_LAYER_MAX] = {0};
                for (int j = 0; j < cDET_LAYER; j++)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    if (y[j] != -1000)
                    {
                        ref_zyy[ref_cnt_y] = y[j];
                        ref_zyz[ref_cnt_y] = z[j];
                        ref_cnt_y++;
                    }
                }

                if (ref_cnt_x == cDET_LAYER - 1 && ref_cnt_y == cDET_LAYER - 1)
                {
                    double kzx, bzx, rmse_zx, R2_zx;
                    hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
                    double kzy, bzy, rmse_zy, R2_zy;
                    hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
                    double fit_R2_cut = 0.8;
                    if (rmse_zx < 2 && rmse_zy < 2 && R2_zx > fit_R2_cut && R2_zy > fit_R2_cut)
                    {
                        double fit_x = kzx * cDET_Z[i] + bzx;
                        double fit_y = kzy * cDET_Z[i] + bzy;
                        double fit_z = cDET_Z[i];
                        // 因为cDET_Z没有经过对齐，不准确，所以要对它进行一次对齐
                        m_correct->alignment_data(fit_x, fit_y, fit_z, i);
                        fit_x = kzx * fit_z + bzx;
                        fit_y = kzy * fit_z + bzy;

                        double delta_x = fit_x - x[i];
                        double delta_y = fit_y - y[i];

                        pos_res_x_hist[i]->Fill(delta_x);
                        pos_res_y_hist[i]->Fill(delta_y);
                    }
                }
            }
        }

        double pos_res_x[cDET_LAYER_MAX] = {0};
        double pos_res_y[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (pos_res_x_hist[i]->GetEntries() > 0)
            {
                // 双高斯拟合
                double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
                TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -10, 10);
                fitx->SetParameter(0, 50);
                fitx->SetParameter(1, max_pos_x);
                fitx->SetParameter(2, 0.4);
                fitx->SetParLimits(2, 0, 2.0);
                fitx->SetParameter(3, 5);
                fitx->SetParameter(4, max_pos_x);
                fitx->SetParameter(5, 5);
                fitx->SetParLimits(5, 1.0, 20.0);
                pos_res_x_hist[i]->Fit(fitx, "Q");
                pos_res_x[i] = fitx->GetParameter(2);
                cout << i << "x: " << pos_res_x[i] << endl;

                TCanvas *c1 = new TCanvas("c", "c", 800, 600);
                pos_res_x_hist[i]->Draw();
                list_file::create_path(m_png_path);
                string pngname1 = m_png_path + "detector_res_x_test_" + to_string(i) + ".png";
                c1->Print(pngname1.data());
                delete c1;

                delete fitx;
            }

            if (pos_res_y_hist[i]->GetEntries() > 0)
            {
                // 双高斯拟合
                double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
                TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -10, 10);
                fity->SetParameter(0, 50);
                fity->SetParameter(1, max_pos_y);
                fity->SetParameter(2, 0.4);
                fity->SetParLimits(2, 0, 2.0);
                fity->SetParameter(3, 5);
                fity->SetParameter(4, max_pos_y);
                fity->SetParameter(5, 5);
                fity->SetParLimits(5, 1.0, 20.0);
                pos_res_y_hist[i]->Fit(fity, "Q");
                pos_res_y[i] = fity->GetParameter(2);
                cout << i << "y: " << pos_res_y[i] << endl;

                TCanvas *c1 = new TCanvas("c", "c", 800, 600);
                pos_res_y_hist[i]->Draw();
                list_file::create_path(m_png_path);
                string pngname1 = m_png_path + "detector_res_y_test_" + to_string(i) + ".png";
                c1->Print(pngname1.data());
                delete c1;

                delete fity;
            }
        }
    }

    // calculate detector efficiency ---------------------------------
    // save detector hit counter
    TH2F *hits_exp_map_x[cDET_LAYER_MAX];
    TH2F *hits_exp_map_y[cDET_LAYER_MAX];
    TH2F *hits_real_map_x[cDET_LAYER_MAX];
    TH2F *hits_real_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string hitsexpmapxname = "hits-exp-map-x_" + to_string(i);
        hits_exp_map_x[i] = new TH2F(hitsexpmapxname.data(), hitsexpmapxname.data(), det_divide_bin_num, 0,
                                     cDET_SIZE[i], det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsexpmapyname = "hits-exp-map-y_" + to_string(i);
        hits_exp_map_y[i] = new TH2F(hitsexpmapyname.data(), hitsexpmapyname.data(), det_divide_bin_num, 0,
                                     cDET_SIZE[i], det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapxname = "hits-real-map-x_" + to_string(i);
        hits_real_map_x[i] = new TH2F(hitsrealmapxname.data(), hitsrealmapxname.data(), det_divide_bin_num, 0,
                                      cDET_SIZE[i], det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapyname = "hits-real-map-y_" + to_string(i);
        hits_real_map_y[i] = new TH2F(hitsrealmapyname.data(), hitsrealmapyname.data(), det_divide_bin_num, 0,
                                      cDET_SIZE[i], det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig_x != 0)
            {
                x[i] = detector_data[i]->x;
                z[i] = detector_data[i]->z;
            }
            else
            {
                x[i] = -1000;
            }
            if (detector_data[i]->sig_y != 0)
            {
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;
            }
            else
            {
                y[i] = -1000;
            }
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            int ref_cnt_x = 0;
            double ref_zxx[cDET_LAYER_MAX] = {0};
            double ref_zxz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (x[j] != -1000)
                {
                    ref_zxx[ref_cnt_x] = x[j];
                    ref_zxz[ref_cnt_x] = z[j];
                    ref_cnt_x++;
                }
            }

            int ref_cnt_y = 0;
            double ref_zyy[cDET_LAYER_MAX] = {0};
            double ref_zyz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (y[j] != -1000)
                {
                    ref_zyy[ref_cnt_y] = y[j];
                    ref_zyz[ref_cnt_y] = z[j];
                    ref_cnt_y++;
                }
            }

            if (ref_cnt_x == cDET_LAYER - 1 && ref_cnt_y == cDET_LAYER - 1)
            {
                double kzx, bzx, rmse_zx, R2_zx;
                hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
                double kzy, bzy, rmse_zy, R2_zy;
                hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
                if (rmse_zx < 2 && rmse_zy < 2)
                {
                    double fit_x = kzx * cDET_Z[i] + bzx;
                    double fit_y = kzy * cDET_Z[i] + bzy;
                    double fit_z = cDET_Z[i];
                    // 因为cDET_Z没有经过对齐，不准确，所以要对它进行一次对齐
                    m_correct->alignment_data(fit_x, fit_y, fit_z, i);
                    fit_x = kzx * fit_z + bzx;
                    fit_y = kzy * fit_z + bzy;

                    double fit_x_bias = fit_x - m_correct->get_alignment_data(i, 0);
                    double fit_y_bias = fit_y - m_correct->get_alignment_data(i, 1);
                    if (fit_x_bias > 0 && fit_x_bias < cDET_SIZE[i] && fit_y_bias > 0 && fit_y_bias < cDET_SIZE[i])
                    {
                        hits_exp_map_x[i]->Fill(fit_x_bias, fit_y_bias);
                        hits_exp_map_y[i]->Fill(fit_x_bias, fit_y_bias);

                        if (x[i] != -1000 && abs(x[i] - fit_x) < 10 * pos_res_x[i])
                        {
                            hits_real_map_x[i]->Fill(fit_x_bias, fit_y_bias);
                        }
                        if (y[i] != -1000 && abs(y[i] - fit_y) < 10 * pos_res_y[i])
                        {
                            hits_real_map_y[i]->Fill(fit_x_bias, fit_y_bias);
                        }
                    }
                }
            }
        }
    }

    // save detector efficient
    TH2F *eff_map_x[cDET_LAYER_MAX];
    TH2F *eff_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string effmapxname = "eff-map-x_" + to_string(i);
        eff_map_x[i] = new TH2F(effmapxname.data(), effmapxname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                                det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string effmapyname = "eff-map-y_" + to_string(i);
        eff_map_y[i] = new TH2F(effmapyname.data(), effmapyname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                                det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        // zx
        double mean_x = 0;
        int nbins_x = 0;
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            for (int k = 1; k <= det_divide_bin_num; k++)
            {
                int hits_exp_x = hits_exp_map_x[i]->GetBinContent(j, k);
                int hits_real_x = hits_real_map_x[i]->GetBinContent(j, k);
                if (hits_exp_x > 0)
                {
                    eff_map_x[i]->SetBinContent(j, k, (double)hits_real_x / (double)hits_exp_x);
                }

                mean_x += (double)hits_real_x / (double)hits_exp_x;
                nbins_x++;
            }
        }
        mean_x = mean_x / nbins_x;

        cout << "det-x-" << i << " eff: " << mean_x << endl;

        // zy
        double mean_y = 0;
        int nbins_y = 0;
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            for (int k = 1; k <= det_divide_bin_num; k++)
            {
                int hits_exp_y = hits_exp_map_y[i]->GetBinContent(j, k);
                int hits_real_y = hits_real_map_y[i]->GetBinContent(j, k);
                if (hits_exp_y > 0)
                {
                    eff_map_y[i]->SetBinContent(j, k, (double)hits_real_y / (double)hits_exp_y);
                }

                mean_y += (double)hits_real_y / (double)hits_exp_y;
                nbins_y++;
            }
        }
        mean_y = mean_y / nbins_y;

        cout << "det-y-" << i << " eff: " << mean_y << endl;
    }

    // see https://root-forum.cern.ch/t/setpainttextformat/17085/5
    // 参考c字符串格式化
    // gStyle->SetPaintTextFormat(".3f");
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        eff_map_x[i]->SetStats(kFALSE);
        eff_map_x[i]->GetXaxis()->SetLabelSize(0.05);
        eff_map_x[i]->GetYaxis()->SetLabelSize(0.05);
        // eff_map_x[i]->GetXaxis()->SetTitle("position (mm)");
        // eff_map_x[i]->GetYaxis()->SetTitle("position (mm)");
        eff_map_x[i]->Draw("colz");
        for (int j = 1; j < eff_map_x[i]->GetNbinsX() + 1; j++)
        {
            for (int k = 1; k < eff_map_x[i]->GetNbinsY() + 1; k++)
            {
                auto t = new TText(eff_map_x[i]->GetXaxis()->GetBinCenter(j), eff_map_x[i]->GetYaxis()->GetBinCenter(k),
                                   Form("%.3f", eff_map_x[i]->GetBinContent(j, k)));
                t->SetTextAlign(22);
                t->SetTextSize(0.05);
                t->Draw();
            }
        }
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map_2d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        eff_map_y[i]->SetStats(kFALSE);
        eff_map_y[i]->GetXaxis()->SetLabelSize(0.05);
        eff_map_y[i]->GetYaxis()->SetLabelSize(0.05);
        eff_map_y[i]->Draw("colz");
        for (int j = 1; j < eff_map_y[i]->GetNbinsX() + 1; j++)
        {
            for (int k = 1; k < eff_map_y[i]->GetNbinsY() + 1; k++)
            {
                auto t = new TText(eff_map_y[i]->GetXaxis()->GetBinCenter(j), eff_map_y[i]->GetYaxis()->GetBinCenter(k),
                                   Form("%.3f", eff_map_y[i]->GetBinContent(j, k)));
                t->SetTextAlign(22);
                // t->SetTextSize(0.5);
                t->Draw();
            }
        }
        list_file::create_path(m_png_path);
        string pngname2 = m_png_path + "eff_map_2d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }
    // gStyle->SetPaintTextFormat("g");

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        hits_exp_map_x[i]->SetStats(kFALSE);
        hits_exp_map_x[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_exp_map_2d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        hits_exp_map_y[i]->SetStats(kFALSE);
        hits_exp_map_y[i]->Draw("colztext");
        string pngname2 = m_png_path + "hits_exp_map_2d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        hits_real_map_x[i]->SetStats(kFALSE);
        hits_real_map_x[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_real_map_2d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        hits_real_map_y[i]->SetStats(kFALSE);
        hits_real_map_y[i]->Draw("colztext");
        string pngname2 = m_png_path + "hits_real_map_2d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete hits_exp_map_x[i];
        delete hits_real_map_x[i];
        delete eff_map_x[i];

        delete hits_exp_map_y[i];
        delete hits_real_map_y[i];
        delete eff_map_y[i];
    }

    return true;
}

bool info_calc::run_eff_calc_use_alignment()
{
    // alignment_new *alignment_tool = new alignment_new();
    alignment_quaternion *alignment_tool = new alignment_quaternion();
    alignment_tool->set_alignment_filename(m_alignment_filename);

    // settings
    const int det_divide_bin_num = 8;
    const double check_eff_width = 64;  // chn
    // double det_divide_bin_width = cDET_SIZE / det_divide_bin_num;

    // position resolution  ------------------------------------------
    // show res info of each detector
    TH1F *pos_res_x_hist[cDET_LAYER_MAX];
    TH1F *pos_res_y_hist[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string namex = "x_" + to_string(i);
        string namey = "y_" + to_string(i);
        pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 500, -10, 10);
        pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 500, -10, 10);
    }

    // double fit_hit_res_cut = 5.0;
    double fit_rmse_cut = 2.0;
    // double fit_R2_cut = 0.8;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        // if (evt % 10000 == 0)
        // {
        //     cout << "Run res of event: " << evt << endl;
        // }

        // get data
        vector<double> zx_origin, x_origin, zy_origin, y_origin;
        vector<double> zx, x, zy, y;
        vector<int> layer;
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (i == 9)
                continue;
            if (detector_data[i]->sig == 1)
            {
                x_origin.push_back(detector_data[i]->x);
                zx_origin.push_back(detector_data[i]->z);
                y_origin.push_back(detector_data[i]->y);
                zy_origin.push_back(detector_data[i]->z);

                double xx = detector_data[i]->x;
                double yy = detector_data[i]->y;
                double zz = detector_data[i]->z;
                alignment_tool->local_to_global(xx, yy, zz, i);
                x.push_back(xx);
                zx.push_back(zz);
                y.push_back(yy);
                zy.push_back(zz);

                layer.push_back(i);
            }
        }
        // 只挑选穿过所有层探测器的数据
        if (layer.size() < 8 || layer.size() < cDET_LAYER)
        {
            continue;
        }

        // 探测器分辨率
        double sigmax = 0.4 / sqrt(12);
        double sigmay = 0.4 / sqrt(12);

        double kx, bx, ky, by;
        double chi2Dndf_x, chi2Dndf_y;

        // 直线拟合
        if (!alignment_tool->least_square_fit(zx, x, sigmax, kx, bx, chi2Dndf_x)
            || !alignment_tool->least_square_fit(zy, y, sigmay, ky, by, chi2Dndf_y))
        {
            continue;
        }
        alignment_tool->define_track(kx, ky, bx, by);

        // 计算这条直线的对齐权重
        // double weight = 1;
        double weight = sqrt((chi2Dndf_x + chi2Dndf_y) / 2) * 1;

        for (int idx = 0; idx < layer.size(); idx++)
        {
            int i = layer[idx];

            // 重新拟合参数
            vector<double> re_x = x;
            re_x.erase(re_x.begin() + idx);
            vector<double> re_y = y;
            re_y.erase(re_y.begin() + idx);
            vector<double> re_z = zx;
            re_z.erase(re_z.begin() + idx);
            // vector<double> re_zx = zx; re_zx.erase(re_zx.begin()+idx);
            // vector<double> re_zy = zy; re_zy.erase(re_zy.begin()+idx);
            double re_kx, re_bx, re_ky, re_by;
            if (!alignment_tool->least_square_fit(re_z, re_x, sigmax, re_kx, re_bx, chi2Dndf_x)
                || !alignment_tool->least_square_fit(re_z, re_y, sigmay, re_ky, re_by, chi2Dndf_y))
            {
                continue;
            }
            alignment_tool->define_track(re_kx, re_ky, re_bx, re_by);
            double weight = sqrt((chi2Dndf_x + chi2Dndf_y) / 2) * 1;

            // save res
            double dx = x_origin[idx] - alignment_tool->expect_position_x(i);
            double anglezx = abs(atan(re_kx)) / TMath::Pi() * 180;
            if (anglezx < 30)
                pos_res_x_hist[i]->Fill(dx);

            double dy = y_origin[idx] - alignment_tool->expect_position_y(i);
            double anglezy = abs(atan(re_ky)) / TMath::Pi() * 180;
            if (anglezy < 30)
                pos_res_y_hist[i]->Fill(dy);
        }
    }

    double pos_res_x[cDET_LAYER_MAX] = {0};
    double pos_res_y[cDET_LAYER_MAX] = {0};
    for (int i = 0; i < cDET_LAYER; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            // 双高斯拟合
            double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
            TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -10, 10);
            fitx->SetParameter(0, 50);
            fitx->SetParameter(1, max_pos_x);
            fitx->SetParameter(2, 0.4);
            fitx->SetParLimits(2, 0, 2.0);
            fitx->SetParameter(3, 5);
            fitx->SetParameter(4, max_pos_x);
            fitx->SetParameter(5, 5);
            fitx->SetParLimits(5, 1.0, 20.0);
            pos_res_x_hist[i]->Fit(fitx, "Q");
            pos_res_x[i] = fitx->GetParameter(2);
            cout << i << " test1 x: " << pos_res_x[i] << endl;

            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_x_hist[i]->Draw();
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "detector_res_x_eff_test_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;

            delete fitx;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            // 双高斯拟合
            double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
            TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -10, 10);
            fity->SetParameter(0, 50);
            fity->SetParameter(1, max_pos_y);
            fity->SetParameter(2, 0.4);
            fity->SetParLimits(2, 0, 2.0);
            fity->SetParameter(3, 5);
            fity->SetParameter(4, max_pos_y);
            fity->SetParameter(5, 5);
            fity->SetParLimits(5, 1.0, 20.0);
            pos_res_y_hist[i]->Fit(fity, "Q");
            pos_res_y[i] = fity->GetParameter(2);
            cout << i << " test1 y: " << pos_res_y[i] << endl;

            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_y_hist[i]->Draw();
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "detector_res_y_eff_test_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;

            delete fity;
        }
    }

    {
        // get detector position resolution ------------------------------
        TH1F *pos_res_x_hist[cDET_LAYER_MAX];
        TH1F *pos_res_y_hist[cDET_LAYER_MAX];
        for (int i = 0; i < cDET_LAYER; i++)
        {
            string namex = "x_" + to_string(i);
            string namey = "y_" + to_string(i);
            pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -10, 10);
            pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -10, 10);
        }

        // m_correct->set_use_alignment(false);
        // m_correct->set_rmse_cut(10000);
        // m_correct->set_ndet_num_cut(cDET_LAYER);
        // m_correct->set_theta_cut(90./180*3.1415);
        // m_correct->set_hit_res_cut(10000);

        // fill hit counter
        for (int evt = 0; evt < entries; evt++)
        {
            hit_data_tree->GetEntry(evt);
            // if (evt % 10000 == 0)
            // {
            //     cout << "Run efficient of event: " << evt << endl;
            // }

            // m_correct->set_evt_data(detector_data);
            // if (m_correct->correct() != 10) continue;
            // int N = m_correct->idx_det.size(); // layer counter of this event

            double x[cDET_LAYER_MAX] = {0};
            double y[cDET_LAYER_MAX] = {0};
            double z[cDET_LAYER_MAX] = {0};

            // for (int i=0; i<cDET_LAYER; i++)
            // {
            //     x[i] = -1000;
            //     y[i] = -1000;
            // }
            // for (int n=0; n<N; n++)
            // {
            //     vector<double> posx = m_correct->posx;
            //     vector<double> posy = m_correct->posy;
            //     vector<double> posz = m_correct->posz;
            //     vector<int> idx_det = m_correct->idx_det;

            //     int layer = idx_det[n];
            //     x[layer] = posx[n];
            //     y[layer] = posy[n];
            //     z[layer] = posz[n];
            // }

            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig_x != 0)
                {
                    x[i] = detector_data[i]->x;
                    z[i] = detector_data[i]->z;
                }
                else
                {
                    x[i] = -1000;
                }
                if (detector_data[i]->sig_y != 0)
                {
                    y[i] = detector_data[i]->y;
                    z[i] = detector_data[i]->z;
                }
                else
                {
                    y[i] = -1000;
                }
            }

            for (int i = 0; i < cDET_LAYER; i++)
            {
                int ref_cnt_x = 0;
                double ref_zxx[cDET_LAYER_MAX] = {0};
                double ref_zxz[cDET_LAYER_MAX] = {0};
                for (int j = 0; j < cDET_LAYER; j++)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    if (x[j] != -1000)
                    {
                        ref_zxx[ref_cnt_x] = x[j];
                        ref_zxz[ref_cnt_x] = z[j];
                        ref_cnt_x++;
                    }
                }

                int ref_cnt_y = 0;
                double ref_zyy[cDET_LAYER_MAX] = {0};
                double ref_zyz[cDET_LAYER_MAX] = {0};
                for (int j = 0; j < cDET_LAYER; j++)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    if (y[j] != -1000)
                    {
                        ref_zyy[ref_cnt_y] = y[j];
                        ref_zyz[ref_cnt_y] = z[j];
                        ref_cnt_y++;
                    }
                }

                if (ref_cnt_x == cDET_LAYER - 1 && ref_cnt_y == cDET_LAYER - 1)
                {
                    double kzx, bzx, rmse_zx, R2_zx;
                    hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
                    double kzy, bzy, rmse_zy, R2_zy;
                    hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
                    double fit_R2_cut = 0.8;
                    if (rmse_zx < 2 && rmse_zy < 2 && R2_zx > fit_R2_cut && R2_zy > fit_R2_cut)
                    {
                        double fit_x = kzx * cDET_Z[i] + bzx;
                        double fit_y = kzy * cDET_Z[i] + bzy;
                        double fit_z = cDET_Z[i];
                        // 因为cDET_Z没有经过对齐，不准确，所以要对它进行一次对齐
                        m_correct->alignment_data(fit_x, fit_y, fit_z, i);
                        fit_x = kzx * fit_z + bzx;
                        fit_y = kzy * fit_z + bzy;

                        double delta_x = fit_x - x[i];
                        double delta_y = fit_y - y[i];

                        pos_res_x_hist[i]->Fill(delta_x);
                        pos_res_y_hist[i]->Fill(delta_y);
                    }
                }
            }
        }

        double pos_res_x[cDET_LAYER_MAX] = {0};
        double pos_res_y[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (pos_res_x_hist[i]->GetEntries() > 0)
            {
                // 双高斯拟合
                double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
                TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -10, 10);
                fitx->SetParameter(0, 50);
                fitx->SetParameter(1, max_pos_x);
                fitx->SetParameter(2, 0.4);
                fitx->SetParLimits(2, 0, 2.0);
                fitx->SetParameter(3, 5);
                fitx->SetParameter(4, max_pos_x);
                fitx->SetParameter(5, 5);
                fitx->SetParLimits(5, 1.0, 20.0);
                pos_res_x_hist[i]->Fit(fitx, "Q");
                pos_res_x[i] = fitx->GetParameter(2);
                cout << i << " test2 x: " << pos_res_x[i] << endl;

                TCanvas *c1 = new TCanvas("c", "c", 800, 600);
                pos_res_x_hist[i]->Draw();
                list_file::create_path(m_png_path);
                string pngname1 = m_png_path + "detector_res_x_test_" + to_string(i) + ".png";
                c1->Print(pngname1.data());
                delete c1;

                delete fitx;
            }

            if (pos_res_y_hist[i]->GetEntries() > 0)
            {
                // 双高斯拟合
                double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
                TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -10, 10);
                fity->SetParameter(0, 50);
                fity->SetParameter(1, max_pos_y);
                fity->SetParameter(2, 0.4);
                fity->SetParLimits(2, 0, 2.0);
                fity->SetParameter(3, 5);
                fity->SetParameter(4, max_pos_y);
                fity->SetParameter(5, 5);
                fity->SetParLimits(5, 1.0, 20.0);
                pos_res_y_hist[i]->Fit(fity, "Q");
                pos_res_y[i] = fity->GetParameter(2);
                cout << i << " test2 y: " << pos_res_y[i] << endl;

                TCanvas *c1 = new TCanvas("c", "c", 800, 600);
                pos_res_y_hist[i]->Draw();
                list_file::create_path(m_png_path);
                string pngname1 = m_png_path + "detector_res_y_test_" + to_string(i) + ".png";
                c1->Print(pngname1.data());
                delete c1;

                delete fity;
            }
        }
    }

    // calculate detector efficiency ---------------------------------
    // save detector hit counter
    TH2F *hits_exp_map_x[cDET_LAYER_MAX];
    TH2F *hits_exp_map_y[cDET_LAYER_MAX];
    TH2F *hits_real_map_x[cDET_LAYER_MAX];
    TH2F *hits_real_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        int encode_board_num = (cDET_SIZE[i] / 0.4) / check_eff_width;
        double encode_board_num_rem = fmod(cDET_SIZE[i] / 0.4, check_eff_width);
        if (encode_board_num_rem / check_eff_width > 0.5)
        {
            encode_board_num += 1;
        }
        string hitsexpmapxname = "hits-exp-map-x_" + to_string(i);
        hits_exp_map_x[i] = new TH2F(hitsexpmapxname.data(), hitsexpmapxname.data(), encode_board_num, 0, cDET_SIZE[i],
                                     encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsexpmapyname = "hits-exp-map-y_" + to_string(i);
        hits_exp_map_y[i] = new TH2F(hitsexpmapyname.data(), hitsexpmapyname.data(), encode_board_num, 0, cDET_SIZE[i],
                                     encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapxname = "hits-real-map-x_" + to_string(i);
        hits_real_map_x[i] = new TH2F(hitsrealmapxname.data(), hitsrealmapxname.data(), encode_board_num, 0,
                                      cDET_SIZE[i], encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapyname = "hits-real-map-y_" + to_string(i);
        hits_real_map_y[i] = new TH2F(hitsrealmapyname.data(), hitsrealmapyname.data(), encode_board_num, 0,
                                      cDET_SIZE[i], encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        // get data
        vector<double> zx_origin, x_origin, zy_origin, y_origin;
        vector<double> zx, x, zy, y;
        vector<int> layer;
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (i == 9)
                continue;
            if (detector_data[i]->sig == 1)
            {
                x_origin.push_back(detector_data[i]->x);
                zx_origin.push_back(detector_data[i]->z);
                y_origin.push_back(detector_data[i]->y);
                zy_origin.push_back(detector_data[i]->z);

                double xx = detector_data[i]->x;
                double yy = detector_data[i]->y;
                double zz = detector_data[i]->z;
                alignment_tool->local_to_global(xx, yy, zz, i);
                x.push_back(xx);
                zx.push_back(zz);
                y.push_back(yy);
                zy.push_back(zz);

                layer.push_back(i);
            }
        }
        if (layer.size() < 6)
        {
            continue;
        }

        // 探测器分辨率
        double sigmax = 0.4 / sqrt(12);
        double sigmay = 0.4 / sqrt(12);

        double kx, bx, ky, by;
        double chi2Dndf_x, chi2Dndf_y;

        // 直线拟合
        if (!alignment_tool->least_square_fit(zx, x, sigmax, kx, bx, chi2Dndf_x)
            || !alignment_tool->least_square_fit(zy, y, sigmay, ky, by, chi2Dndf_y))
        {
            continue;
        }
        alignment_tool->define_track(kx, ky, bx, by);

        // 计算这条直线的对齐权重
        // double weight = 1;
        double weight = sqrt((chi2Dndf_x + chi2Dndf_y) / 2) * 1;

        for (int i = 0; i < cDET_LAYER; i++)
        {
            // 重新拟合参数
            vector<double> re_x = x;
            vector<double> re_y = y;
            vector<double> re_z = zx;
            for (int idx = 0; idx < layer.size(); idx++)
            {
                if (layer[idx] == i)
                {
                    re_x.erase(re_x.begin() + idx);
                    re_y.erase(re_y.begin() + idx);
                    re_z.erase(re_z.begin() + idx);
                }
            }
            if (re_x.size() < 8)
                continue;

            double re_kx, re_bx, re_ky, re_by;
            if (!alignment_tool->least_square_fit(re_z, re_x, sigmax, re_kx, re_bx, chi2Dndf_x)
                || !alignment_tool->least_square_fit(re_z, re_y, sigmay, re_ky, re_by, chi2Dndf_y))
            {
                continue;
            }
            alignment_tool->define_track(re_kx, re_ky, re_bx, re_by);
            double weight = sqrt((chi2Dndf_x + chi2Dndf_y) / 2) * 1;

            // static TH1D weight_hist("weight", "weight", 1000, 0, 100);
            // weight_hist.Fill(weight);
            // TCanvas *cc = new TCanvas("cc", "cc", 800, 600);
            // weight_hist.Draw();
            // if (evt % 1000 == 0)
            //     cc->Print("out_info/weight.png");

            // cout << "weight: " << weight << endl;
            // cout << pos_res_x[i] << " " << pos_res_y[i] << endl;

            if (weight < 5)
            {
                double fit_x = alignment_tool->expect_position_x(i);
                double fit_y = alignment_tool->expect_position_y(i);

                if (fit_x > 0 && fit_x < cDET_SIZE[i] - 0 && fit_y > 0 && fit_y < cDET_SIZE[i] - 0)
                {
                    hits_exp_map_x[i]->Fill(fit_x, fit_y);
                    hits_exp_map_y[i]->Fill(fit_x, fit_y);

                    double res = 1;
                    if (i == 8 || i == 9)
                        res = 0.13;
                    else
                        res = 0.1;

                    // if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x-fit_x) < 10*pos_res_x[i])
                    // if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x-fit_x) < 10)
                    if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x - fit_x) < 20 * res)
                    {
                        hits_real_map_x[i]->Fill(fit_x, fit_y);
                    }
                    // if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y-fit_y) < 10*pos_res_y[i])
                    // if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y-fit_y) < 10)
                    if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y - fit_y) < 20 * res)
                    {
                        hits_real_map_y[i]->Fill(fit_x, fit_y);
                    }
                }
            }
        }
    }

    // save detector efficient
    TH2F *eff_map_x[cDET_LAYER_MAX];
    TH2F *eff_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        int encode_board_num = (cDET_SIZE[i] / 0.4) / check_eff_width;
        double encode_board_num_rem = fmod(cDET_SIZE[i] / 0.4, check_eff_width);
        if (encode_board_num_rem / check_eff_width > 0.5)
        {
            encode_board_num += 1;
        }
        string effmapxname = "eff-map-x_" + to_string(i);
        eff_map_x[i] = new TH2F(effmapxname.data(), effmapxname.data(), encode_board_num, 0, cDET_SIZE[i],
                                encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string effmapyname = "eff-map-y_" + to_string(i);
        eff_map_y[i] = new TH2F(effmapyname.data(), effmapyname.data(), encode_board_num, 0, cDET_SIZE[i],
                                encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        int encode_board_num = (cDET_SIZE[i] / 0.4) / check_eff_width;
        double encode_board_num_rem = fmod(cDET_SIZE[i] / 0.4, check_eff_width);
        if (encode_board_num_rem / check_eff_width > 0.5)
        {
            encode_board_num += 1;
        }
        // zx
        int total_exp_x = 0, total_real_x = 0;
        double mean_x = 0;
        int nbins_x = 0;
        for (int j = 1; j <= encode_board_num; j++)
        {
            for (int k = 1; k <= encode_board_num; k++)
            {
                int hits_exp_x = hits_exp_map_x[i]->GetBinContent(j, k);
                int hits_real_x = hits_real_map_x[i]->GetBinContent(j, k);
                if (hits_exp_x > 0)
                {
                    eff_map_x[i]->SetBinContent(j, k, (double)hits_real_x / (double)hits_exp_x);
                }

                if (true)
                // if (i == 0 && j >= 9 && j <= 12 && k >= 3 && k <= 6) // for 400
                // if (i == 0 && j >= 13 && j <= 20 && k >= 7 && k <= 14) // for 600
                {
                    total_exp_x += hits_exp_x;
                    total_real_x += hits_real_x;
                }
                else if (i != 0)
                {
                    total_exp_x += hits_exp_x;
                    total_real_x += hits_real_x;
                }

                mean_x += (double)hits_real_x / (double)hits_exp_x;
                nbins_x++;
            }
        }
        mean_x = mean_x / nbins_x;

        // cout << "det-x-" << i << " eff: " << mean_x << endl;
        cout << "det-x-" << i << " total eff: " << (double)total_real_x / (double)total_exp_x << endl;

        // zy
        int total_exp_y = 0, total_real_y = 0;
        double mean_y = 0;
        int nbins_y = 0;
        for (int j = 1; j <= encode_board_num; j++)
        {
            for (int k = 1; k <= encode_board_num; k++)
            {
                int hits_exp_y = hits_exp_map_y[i]->GetBinContent(j, k);
                int hits_real_y = hits_real_map_y[i]->GetBinContent(j, k);
                if (hits_exp_y > 0)
                {
                    eff_map_y[i]->SetBinContent(j, k, (double)hits_real_y / (double)hits_exp_y);
                }

                if (true)
                // if (i == 0 && j >= 9 && j <= 12 && k >= 3 && k <= 6) // for 400
                // if (i == 0 && j >= 13 && j <= 20 && k >= 7 && k <= 14) // for 600
                {
                    total_exp_y += hits_exp_y;
                    total_real_y += hits_real_y;
                }
                else if (i != 0)
                {
                    total_exp_y += hits_exp_y;
                    total_real_y += hits_real_y;
                }

                mean_y += (double)hits_real_y / (double)hits_exp_y;
                nbins_y++;
            }
        }
        mean_y = mean_y / nbins_y;

        // cout << "det-y-" << i << " eff: " << mean_y << endl;
        cout << "det-y-" << i << " total eff: " << (double)total_real_y / (double)total_exp_y << endl;
    }

    // see https://root-forum.cern.ch/t/setpainttextformat/17085/5
    // 参考c字符串格式化
    // gStyle->SetPaintTextFormat(".3f");
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        eff_map_x[i]->SetStats(kFALSE);
        eff_map_x[i]->GetXaxis()->SetLabelSize(0.05);
        eff_map_x[i]->GetYaxis()->SetLabelSize(0.05);
        // eff_map_x[i]->GetXaxis()->SetTitle("position (mm)");
        // eff_map_x[i]->GetYaxis()->SetTitle("position (mm)");
        eff_map_x[i]->Draw("colz");
        for (int j = 1; j < eff_map_x[i]->GetNbinsX() + 1; j++)
        {
            for (int k = 1; k < eff_map_x[i]->GetNbinsY() + 1; k++)
            {
                auto t = new TText(eff_map_x[i]->GetXaxis()->GetBinCenter(j), eff_map_x[i]->GetYaxis()->GetBinCenter(k),
                                   Form("%.3f", eff_map_x[i]->GetBinContent(j, k)));
                t->SetTextAlign(22);
                t->SetTextSize(0.02);
                t->Draw();
            }
        }
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map_2d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        eff_map_y[i]->SetStats(kFALSE);
        eff_map_y[i]->GetXaxis()->SetLabelSize(0.05);
        eff_map_y[i]->GetYaxis()->SetLabelSize(0.05);
        eff_map_y[i]->Draw("colz");
        for (int j = 1; j < eff_map_y[i]->GetNbinsX() + 1; j++)
        {
            for (int k = 1; k < eff_map_y[i]->GetNbinsY() + 1; k++)
            {
                auto t = new TText(eff_map_y[i]->GetXaxis()->GetBinCenter(j), eff_map_y[i]->GetYaxis()->GetBinCenter(k),
                                   Form("%.3f", eff_map_y[i]->GetBinContent(j, k)));
                t->SetTextAlign(22);
                t->SetTextSize(0.02);
                t->Draw();
            }
        }
        list_file::create_path(m_png_path);
        string pngname2 = m_png_path + "eff_map_2d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }
    // gStyle->SetPaintTextFormat("g");

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        hits_exp_map_x[i]->SetStats(kFALSE);
        hits_exp_map_x[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_exp_map_2d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        hits_exp_map_y[i]->SetStats(kFALSE);
        hits_exp_map_y[i]->Draw("colztext");
        string pngname2 = m_png_path + "hits_exp_map_2d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        hits_real_map_x[i]->SetStats(kFALSE);
        hits_real_map_x[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_real_map_2d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        hits_real_map_y[i]->SetStats(kFALSE);
        hits_real_map_y[i]->Draw("colztext");
        string pngname2 = m_png_path + "hits_real_map_2d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete hits_exp_map_x[i];
        delete hits_real_map_x[i];
        delete eff_map_x[i];

        delete hits_exp_map_y[i];
        delete hits_real_map_y[i];
        delete eff_map_y[i];
    }

    delete alignment_tool;

    return true;
}

bool info_calc::run_eff_calc_use_alignment_single_dimension()
{
    // alignment_new *alignment_tool = new alignment_new();
    alignment_quaternion *alignment_tool = new alignment_quaternion();
    alignment_tool->set_alignment_filename(m_alignment_filename);

    // settings
    const int det_divide_bin_num = 8;
    const double check_eff_width = 64;  // chn
    // double det_divide_bin_width = cDET_SIZE / det_divide_bin_num;

    // calculate detector efficiency ---------------------------------
    // save detector hit counter
    TH1D *hits_exp_map_x[cDET_LAYER_MAX];
    TH1D *hits_exp_map_y[cDET_LAYER_MAX];
    TH1D *hits_real_map_x[cDET_LAYER_MAX];
    TH1D *hits_real_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        int encode_board_num = (cDET_SIZE[i] / 0.4) / check_eff_width;
        double encode_board_num_rem = fmod(cDET_SIZE[i] / 0.4, check_eff_width);
        if (encode_board_num_rem / check_eff_width > 0.5)
        {
            encode_board_num += 1;
        }
        string hitsexpmapxname = "hits-exp-map-x_" + to_string(i);
        hits_exp_map_x[i] =
            new TH1D(hitsexpmapxname.data(), hitsexpmapxname.data(), encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsexpmapyname = "hits-exp-map-y_" + to_string(i);
        hits_exp_map_y[i] =
            new TH1D(hitsexpmapyname.data(), hitsexpmapyname.data(), encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapxname = "hits-real-map-x_" + to_string(i);
        hits_real_map_x[i] =
            new TH1D(hitsrealmapxname.data(), hitsrealmapxname.data(), encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapyname = "hits-real-map-y_" + to_string(i);
        hits_real_map_y[i] =
            new TH1D(hitsrealmapyname.data(), hitsrealmapyname.data(), encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        // get data
        vector<double> zx_origin, x_origin, zy_origin, y_origin;
        vector<double> zx, x, zy, y;
        vector<int> layer;
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1)
            {
                x_origin.push_back(detector_data[i]->x);
                zx_origin.push_back(detector_data[i]->z);
                y_origin.push_back(detector_data[i]->y);
                zy_origin.push_back(detector_data[i]->z);

                double xx = detector_data[i]->x;
                double yy = detector_data[i]->y;
                double zz = detector_data[i]->z;
                alignment_tool->local_to_global(xx, yy, zz, i);
                x.push_back(xx);
                zx.push_back(zz);
                y.push_back(yy);
                zy.push_back(zz);

                layer.push_back(i);
            }
        }
        if (layer.size() < 4)
        {
            continue;
        }

        // 探测器分辨率
        double sigmax = 0.4 / sqrt(12);
        double sigmay = 0.4 / sqrt(12);

        double kx, bx, ky, by;
        double chi2Dndf_x, chi2Dndf_y;

        // 直线拟合
        if (!alignment_tool->least_square_fit(zx, x, sigmax, kx, bx, chi2Dndf_x)
            || !alignment_tool->least_square_fit(zy, y, sigmay, ky, by, chi2Dndf_y))
        {
            continue;
        }
        alignment_tool->define_track(kx, ky, bx, by);

        // 计算这条直线的对齐权重
        // double weight = 1;
        double weight = sqrt((chi2Dndf_x + chi2Dndf_y) / 2) * 1;

        // if (atan(kx) / TMath::Pi() * 180 > 5
        //     || atan(ky) / TMath::Pi() * 180 > 5)
        // {
        //     continue;
        // }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            // 重新拟合参数
            vector<double> re_x = x;
            vector<double> re_y = y;
            vector<double> re_z = zx;
            for (int idx = 0; idx < layer.size(); idx++)
            {
                if (layer[idx] == i)
                {
                    re_x.erase(re_x.begin() + idx);
                    re_y.erase(re_y.begin() + idx);
                    re_z.erase(re_z.begin() + idx);
                }
            }

            if (re_x.size() == 8 && re_y.size() > 4)
            {
                double re_kx, re_bx, re_ky, re_by;
                if (alignment_tool->least_square_fit(re_z, re_x, sigmax, re_kx, re_bx, chi2Dndf_x)
                    && alignment_tool->least_square_fit(re_z, re_y, sigmay, re_ky, re_by, chi2Dndf_y))
                    alignment_tool->define_track(re_kx, re_ky, re_bx, re_by);
                double weight = sqrt(chi2Dndf_x) * 1;
                if (weight < 5)
                {
                    double fit_x = alignment_tool->expect_position_x(i);

                    if (fit_x > 0 && fit_x < cDET_SIZE[i] - 0 && i != 8)
                    {
                        hits_exp_map_x[i]->Fill(fit_x);

                        double res = 1;
                        if (i == 8 || i == 9)
                            res = 0.13;
                        else
                            res = 0.1;

                        // if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x-fit_x) < 10*pos_res_x[i])
                        if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x - fit_x) < 5)
                        // if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x - fit_x) < 10 * res)
                        {
                            hits_real_map_x[i]->Fill(fit_x);
                        }
                    }
                    else if (i == 8 && fit_x > 55 && fit_x < 123)
                    {
                        hits_exp_map_x[i]->Fill(fit_x);

                        double res = 1;
                        if (i == 8 || i == 9)
                            res = 0.13;
                        else
                            res = 0.1;

                        // if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x-fit_x) < 10*pos_res_x[i])
                        if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x - fit_x) < 5)
                        // if (detector_data[i]->sig_x == 1 && abs(detector_data[i]->x - fit_x) < 10 * res)
                        {
                            hits_real_map_x[i]->Fill(fit_x);
                        }
                    }
                }
            }
            if (re_y.size() == 8 && re_x.size() > 4)
            {
                double re_kx, re_bx, re_ky, re_by;
                if (alignment_tool->least_square_fit(re_z, re_x, sigmax, re_kx, re_bx, chi2Dndf_x)
                    && alignment_tool->least_square_fit(re_z, re_y, sigmay, re_ky, re_by, chi2Dndf_y))
                    alignment_tool->define_track(re_kx, re_ky, re_bx, re_by);
                double weight = sqrt(chi2Dndf_y) * 1;
                if (weight < 5)
                {
                    double fit_y = alignment_tool->expect_position_y(i);

                    if (fit_y > 0 && fit_y < cDET_SIZE[i] - 0 && i != 8)
                    {
                        hits_exp_map_y[i]->Fill(fit_y);

                        double res = 1;
                        if (i == 8 || i == 9)
                            res = 0.13;
                        else
                            res = 0.1;

                        // if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y-fit_y) < 10*pos_res_y[i])
                        if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y - fit_y) < 5)
                        // if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y - fit_y) < 10 * res)
                        {
                            hits_real_map_y[i]->Fill(fit_y);
                        }
                    }
                    else if (i == 8 && fit_y > 55 && fit_y < 123)
                    {
                        hits_exp_map_y[i]->Fill(fit_y);

                        double res = 1;
                        if (i == 8 || i == 9)
                            res = 0.13;
                        else
                            res = 0.1;

                        // if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y-fit_y) < 10*pos_res_y[i])
                        if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y - fit_y) < 5)
                        // if (detector_data[i]->sig_y == 1 && abs(detector_data[i]->y - fit_y) < 10 * res)
                        {
                            hits_real_map_y[i]->Fill(fit_y);
                        }
                    }
                }
            }
        }
    }

    // save detector efficient
    TH1D *eff_map_x[cDET_LAYER_MAX];
    TH1D *eff_map_y[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        int encode_board_num = (cDET_SIZE[i] / 0.4) / check_eff_width;
        double encode_board_num_rem = fmod(cDET_SIZE[i] / 0.4, check_eff_width);
        if (encode_board_num_rem / check_eff_width > 0.5)
        {
            encode_board_num += 1;
        }
        string effmapxname = "eff-map-x_" + to_string(i);
        eff_map_x[i] = new TH1D(effmapxname.data(), effmapxname.data(), encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
        string effmapyname = "eff-map-y_" + to_string(i);
        eff_map_y[i] = new TH1D(effmapyname.data(), effmapyname.data(), encode_board_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    TH1D *eff_map_x_large = new TH1D("eff_map_x_large", "eff_map_x_large", 6, 25, 175);
    TH1D *eff_map_y_large = new TH1D("eff_map_y_large", "eff_map_y_large", 6, 75, 225);

    for (int i = 0; i < cDET_LAYER; i++)
    {
        int encode_board_num = (cDET_SIZE[i] / 0.4) / check_eff_width;
        double encode_board_num_rem = fmod(cDET_SIZE[i] / 0.4, check_eff_width);
        if (encode_board_num_rem / check_eff_width > 0.5)
        {
            encode_board_num += 1;
        }
        // zx
        int total_exp_x = 0, total_real_x = 0;
        double mean_x = 0;
        int nbins_x = 0;
        for (int j = 1; j <= encode_board_num; j++)
        {
            int hits_exp_x = hits_exp_map_x[i]->GetBinContent(j);
            int hits_real_x = hits_real_map_x[i]->GetBinContent(j);
            if (hits_exp_x > 0)
            {
                eff_map_x[i]->SetBinContent(j, (double)hits_real_x / (double)hits_exp_x);
            }

            if (i == 8 && j >= 2 && j <= 7)
            {
                eff_map_x_large->SetBinContent(j - 1, (double)hits_real_x / (double)hits_exp_x);
            }

            if (true)
            // if (i == 0 && j >= 9 && j <= 12 && k >= 3 && k <= 6) // for 400
            // if (i == 0 && j >= 13 && j <= 20 && k >= 7 && k <= 14) // for 600
            {
                total_exp_x += hits_exp_x;
                total_real_x += hits_real_x;
            }
            else if (i != 0)
            {
                total_exp_x += hits_exp_x;
                total_real_x += hits_real_x;
            }

            mean_x += (double)hits_real_x / (double)hits_exp_x;
            nbins_x++;
        }
        mean_x = mean_x / nbins_x;

        // cout << "det-x-" << i << " eff: " << mean_x << endl;
        cout << "det-x-" << i << " total eff: " << (double)total_real_x / (double)total_exp_x << endl;

        // zy
        int total_exp_y = 0, total_real_y = 0;
        double mean_y = 0;
        int nbins_y = 0;
        for (int j = 1; j <= encode_board_num; j++)
        {
            int hits_exp_y = hits_exp_map_y[i]->GetBinContent(j);
            int hits_real_y = hits_real_map_y[i]->GetBinContent(j);
            if (hits_exp_y > 0)
            {
                eff_map_y[i]->SetBinContent(j, (double)hits_real_y / (double)hits_exp_y);
            }

            if (i == 8 && j >= 4 && j <= 9)
            {
                eff_map_y_large->SetBinContent(j - 3, (double)hits_real_y / (double)hits_exp_y);
            }

            if (true)
            // if (i == 0 && j >= 9 && j <= 12 && k >= 3 && k <= 6) // for 400
            // if (i == 0 && j >= 13 && j <= 20 && k >= 7 && k <= 14) // for 600
            {
                total_exp_y += hits_exp_y;
                total_real_y += hits_real_y;
            }
            else if (i != 0)
            {
                total_exp_y += hits_exp_y;
                total_real_y += hits_real_y;
            }

            mean_y += (double)hits_real_y / (double)hits_exp_y;
            nbins_y++;
        }
        mean_y = mean_y / nbins_y;

        // cout << "det-y-" << i << " eff: " << mean_y << endl;
        cout << "det-y-" << i << " total eff: " << (double)total_real_y / (double)total_exp_y << endl;
    }

    // see https://root-forum.cern.ch/t/setpainttextformat/17085/5
    // 参考c字符串格式化
    // gStyle->SetPaintTextFormat(".3f");
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        eff_map_x[i]->SetStats(kFALSE);
        eff_map_x[i]->GetXaxis()->SetLabelSize(0.05);
        eff_map_x[i]->GetYaxis()->SetLabelSize(0.05);
        // eff_map_x[i]->GetXaxis()->SetTitle("position (mm)");
        // eff_map_x[i]->GetYaxis()->SetTitle("position (mm)");
        eff_map_x[i]->Draw("");
        // for (int j=1; j<eff_map_x[i]->GetNbinsX()+1; j++)
        // {
        //     auto t = new TText(eff_map_x[i]->GetXaxis()->GetBinCenter(j),
        //                        Form("%.3f", eff_map_x[i]->GetBinContent(j)));
        //     t->SetTextAlign(22);
        //     t->SetTextSize(0.02);
        //     t->Draw();
        // }
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map_1d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        eff_map_y[i]->SetStats(kFALSE);
        eff_map_y[i]->GetXaxis()->SetLabelSize(0.05);
        eff_map_y[i]->GetYaxis()->SetLabelSize(0.05);
        eff_map_y[i]->Draw("");
        // for (int j=1; j<eff_map_y[i]->GetNbinsX()+1; j++)
        // {
        //     auto t = new TText(eff_map_y[i]->GetXaxis()->GetBinCenter(j),
        //                        Form("%.3f", eff_map_y[i]->GetBinContent(j)));
        //     t->SetTextAlign(22);
        //     t->SetTextSize(0.02);
        //     t->Draw();
        // }
        list_file::create_path(m_png_path);
        string pngname2 = m_png_path + "eff_map_1d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }
    // gStyle->SetPaintTextFormat("g");

    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        eff_map_x_large->SetStats(kFALSE);
        eff_map_x_large->GetXaxis()->SetLabelSize(0.05);
        eff_map_x_large->GetYaxis()->SetLabelSize(0.05);
        eff_map_x_large->Draw("");
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map_1d_x_layer.png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        eff_map_y_large->SetStats(kFALSE);
        eff_map_y_large->GetXaxis()->SetLabelSize(0.05);
        eff_map_y_large->GetYaxis()->SetLabelSize(0.05);
        eff_map_y_large->Draw("");
        list_file::create_path(m_png_path);
        string pngname2 = m_png_path + "eff_map_1d_y_layer.png";
        c2->Print(pngname2.data());
        delete c2;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        hits_exp_map_x[i]->SetStats(kFALSE);
        hits_exp_map_x[i]->Draw("");
        string pngname1 = m_png_path + "hits_exp_map_1d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        hits_exp_map_y[i]->SetStats(kFALSE);
        hits_exp_map_y[i]->Draw("");
        string pngname2 = m_png_path + "hits_exp_map_1d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        hits_real_map_x[i]->SetStats(kFALSE);
        hits_real_map_x[i]->Draw("");
        string pngname1 = m_png_path + "hits_real_map_1d_x" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        hits_real_map_y[i]->SetStats(kFALSE);
        hits_real_map_y[i]->Draw("");
        string pngname2 = m_png_path + "hits_real_map_1d_y" + to_string(i) + ".png";
        c2->Print(pngname2.data());
        delete c2;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete hits_exp_map_x[i];
        delete hits_real_map_x[i];
        delete eff_map_x[i];

        delete hits_exp_map_y[i];
        delete hits_real_map_y[i];
        delete eff_map_y[i];
    }

    delete alignment_tool;

    return true;
}

bool info_calc::stats_track()
{
    // settings
    int track_cnt = 0;

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig_x == 1)
            {
                x[i] = detector_data[i]->x;
                z[i] = detector_data[i]->z;
            }
            if (detector_data[i]->sig_y == 1)
            {
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;
            }
        }

        int ref_cnt_x = 0;
        double ref_zxx[cDET_LAYER_MAX] = {0};
        double ref_zxz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (x[j] > 0)
            {
                ref_zxx[ref_cnt_x] = x[j];
                ref_zxz[ref_cnt_x] = z[j];
                ref_cnt_x++;
            }
        }
        int ref_cnt_y = 0;
        double ref_zyy[cDET_LAYER_MAX] = {0};
        double ref_zyz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (y[j] > 0)
            {
                ref_zyy[ref_cnt_y] = y[j];
                ref_zyz[ref_cnt_y] = z[j];
                ref_cnt_y++;
            }
        }

        if (ref_cnt_x >= 3 && ref_cnt_y >= 3)
        {
            double kzx, bzx, rmse_zx, R2_zx;
            hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
            double kzy, bzy, rmse_zy, R2_zy;
            hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
            if (rmse_zx < 10 && rmse_zx < 10)
            {
                track_cnt++;
            }
        }
    }

    cout << "track counter: " << track_cnt << endl;

    return true;
}

bool info_calc::stats_track_map()
{
    // settings
    TH1D *h1_track_angle = new TH1D("h1", "h1", 100, 0, 90);
    TH1D *h1_track_angle_x = new TH1D("h1_x", "h1_x", 100, 0, 90);
    TH1D *h1_track_angle_y = new TH1D("h1_y", "h1_y", 100, 0, 90);
    int track_cnt = 0;

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run stats track map of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig_x == 1)
            {
                x[i] = detector_data[i]->x;
                z[i] = detector_data[i]->z;
            }
            if (detector_data[i]->sig_y == 1)
            {
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;
            }
        }

        int ref_cnt_x = 0;
        double ref_zxx[cDET_LAYER_MAX] = {0};
        double ref_zxz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (x[j] > 0)
            {
                ref_zxx[ref_cnt_x] = x[j];
                ref_zxz[ref_cnt_x] = z[j];
                ref_cnt_x++;
            }
        }
        int ref_cnt_y = 0;
        double ref_zyy[cDET_LAYER_MAX] = {0};
        double ref_zyz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (y[j] > 0)
            {
                ref_zyy[ref_cnt_y] = y[j];
                ref_zyz[ref_cnt_y] = z[j];
                ref_cnt_y++;
            }
        }

        if (ref_cnt_x >= 3 && ref_cnt_y >= 3)
        {
            double kzx, bzx, rmse_zx, R2_zx;
            hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
            double kzy, bzy, rmse_zy, R2_zy;
            hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
            if (rmse_zx < 10 && rmse_zx < 10)
            {
                double theta = acos(1. / sqrt(kzx * kzx + kzy * kzy + 1));
                theta = theta * rad2deg;
                h1_track_angle->Fill(theta);

                double theta_zx = atan(kzx) * rad2deg;
                double theta_zy = atan(kzy) * rad2deg;
                h1_track_angle_x->Fill(abs(theta_zx));
                h1_track_angle_y->Fill(abs(theta_zy));

                track_cnt++;
            }
        }
    }

    cout << "track counter: " << track_cnt << endl;

    TCanvas *c1 = new TCanvas("c", "c", 800, 600);

    h1_track_angle->Draw();
    list_file::create_path(m_png_path);
    string pngname = m_png_path + "track_angle.png";
    c1->Print(pngname.data());

    h1_track_angle_x->Draw();
    string pngname1 = m_png_path + "track_angle_x.png";
    c1->Print(pngname1.data());
    h1_track_angle_y->Draw();
    string pngname2 = m_png_path + "track_angle_y.png";
    c1->Print(pngname2.data());

    delete c1;

    return true;
}

bool info_calc::show_time_dist()
{
    const int angle_bin_num = 6;
    double angle_bin_size = 5;                          // degree
    double angle_max = angle_bin_size * angle_bin_num;  // degree

    TH1D *time_dist_x[angle_bin_num];
    TH1D *time_dist_y[angle_bin_num];
    for (int i = 0; i < angle_bin_num; i++)
    {
        string namex = "time_dist_x" + to_string(i);
        time_dist_x[i] = new TH1D(namex.data(), namex.data(), 200, 1200, 2000);
        string namey = "time_dist_y" + to_string(i);
        time_dist_y[i] = new TH1D(namey.data(), namey.data(), 200, 1200, 2000);
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(3);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run time dist calculate of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;
        int N = m_correct->idx_det.size();  // layer counter of this event

        int angle_idzx = (int)(m_correct->theta_zx * rad2deg / angle_bin_size);
        int angle_idzy = (int)(m_correct->theta_zy * rad2deg / angle_bin_size);
        if (angle_idzx >= angle_bin_num || angle_idzy >= angle_bin_num)
        {
            continue;
        }

        // save time dist to time_dist
        for (int i = 1; i < 2; i++)
        {
            if (detector_data[i]->sig == 1 && detector_data[i]->x > 0 && detector_data[i]->y > 0)
            {
                for (int j = 0; j < detector_data[i]->strips_num_x; j++)
                {
                    time_dist_x[angle_idzx]->Fill(detector_data[i]->hit_time_x[j]);
                }
                for (int j = 0; j < detector_data[i]->strips_num_y; j++)
                {
                    time_dist_y[angle_idzy]->Fill(detector_data[i]->hit_time_y[j]);
                }
            }
        }
    }

    // fit
    // for (int i=0; i<angle_bin_num; i++)
    // {
    //     if (time_dist_x[i]->GetEntries() > 0)
    //     {
    //         TF1 *fit = new TF1("RectGausFun", RectGausFun, 1200, 1600, 4);
    //         fit->SetParameters(1320, 100, 50, 30);
    //         time_dist_x[i]->Fit(fit);
    //         cout << "direction: x, angle id: " << i << ", fit params: ("
    //              << fit->GetParameter(0) << ", " << fit->GetParameter(1) << ", "
    //              << fit->GetParameter(2) << ", " << fit->GetParameter(3) << ")" << endl;

    //         TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    //         time_dist_x[i]->Draw();
    //         fit->Draw("same");
    //         string name = m_png_path + "time_dist_x_" + to_string(i) + ".png";
    //         c1->Print(name.data());
    //         delete c1;
    //     }
    //     if (time_dist_y[i]->GetEntries() > 0)
    //     {
    //         TF1 *fit = new TF1("RectGausFun", RectGausFun, 1200, 1600, 4);
    //         fit->SetParameters(1320, 100, 50, 30);
    //         time_dist_y[i]->Fit(fit);
    //         cout << "direction: y, angle id: " << i << ", fit params: ("
    //              << fit->GetParameter(0) << ", " << fit->GetParameter(1) << ", "
    //              << fit->GetParameter(2) << ", " << fit->GetParameter(3) << ")" << endl;

    //         TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    //         time_dist_y[i]->Draw();
    //         fit->Draw("same");
    //         string name = m_png_path + "time_dist_y_" + to_string(i) + ".png";
    //         c1->Print(name.data());
    //         delete c1;
    //     }
    // }

    // RooFit
    // for (int i=0; i<angle_bin_num; i++)
    // {
    //     if (time_dist_x[i]->GetEntries() > 0)
    //     {
    //         RooRealVar t("t","t",1200,1550);

    //         RooRealVar s("s","s",1320,1200,1550);
    //         RooRealVar l("l","l",106,50,150);
    //         RooRect rect("rect","rect",t,s,l);

    //         RooRealVar mg("mg","mg",0);
    //         RooRealVar sg("sg","sg",30,0,100);
    //         RooGaussian gaus("gaus","gaus",t,mg,sg);

    //         t.setBins(10000,"fft");
    //         RooFFTConvPdf con("con","con",t,rect,gaus);

    //         RooDataHist data("data","data",t,time_dist_x[i]);
    //         con.fitTo(data);

    //         cout << "direction: x, angle id: " << i << ", fit params: ("
    //              << s << ", " << l << ", " << sg << ")" << endl;

    //         RooPlot *f = t.frame();
    //         data.plotOn(f);
    //         con.plotOn(f);

    //         TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    //         f->Draw();
    //         string name = m_png_path + "time_dist_x_" + to_string(i) + ".png";
    //         c1->Print(name.data());
    //         delete c1;
    //     }
    //     if (time_dist_y[i]->GetEntries() > 0)
    //     {
    //         TF1 *fit = new TF1("RectGausFun", RectGausFun, 1200, 1600, 4);
    //         fit->SetParameters(1320, 100, 50, 30);
    //         time_dist_y[i]->Fit(fit);
    //         cout << "direction: y, angle id: " << i << ", fit params: ("
    //              << fit->GetParameter(0) << ", " << fit->GetParameter(1) << ", "
    //              << fit->GetParameter(2) << ", " << fit->GetParameter(3) << ")" << endl;

    //         TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    //         time_dist_y[i]->Draw();
    //         fit->Draw("same");
    //         string name = m_png_path + "time_dist_y_" + to_string(i) + ".png";
    //         c1->Print(name.data());
    //         delete c1;
    //     }
    // }

    TFile *file = new TFile("test2.root", "recreate");
    for (int i = 0; i < angle_bin_num; i++)
    {
        if (time_dist_x[i]->GetEntries() > 0)
        {
            time_dist_x[i]->Write();
        }
        if (time_dist_y[i]->GetEntries() > 0)
        {
            time_dist_y[i]->Write();
        }
    }
    delete file;

    // delete hists

    return true;
}

bool info_calc::run_pos_res()
{
    // show res info of each detector
    TH1F *pos_res_x_hist[cDET_LAYER_MAX];
    TH1F *pos_res_y_hist[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string namex = "x" + to_string(i);
        string namey = "y" + to_string(i);
        pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -10, 10);
        pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -10, 10);
    }

    double pos_res_x[cDET_LAYER_MAX] = {0};
    double pos_res_y[cDET_LAYER_MAX] = {0};
    double eff_cnt_x[cDET_LAYER_MAX][2] = {0};
    double eff_cnt_y[cDET_LAYER_MAX][2] = {0};

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(4);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 12.0;
    double fit_rmse_cut = 12.0;
    double fit_R2_cut = 0.5;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        int N = m_correct->idx_det.size();  // layer counter of this event
        for (int n = 0; n < N; n++)
        {
            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;

            posx.erase(posx.begin() + n);
            posy.erase(posy.begin() + n);
            posz.erase(posz.begin() + n);
            idx_det.erase(idx_det.begin() + n);

            double kzx, bzx, rmse_zx, R2_zx;
            double kzy, bzy, rmse_zy, R2_zy;
            vector<double> posx_tmp, posy_tmp, posz_tmp;

            // hit res cut
            bool b_hit_res_flag = false;
            for (int i = 0; i < posx.size(); i++)
            {
                posx_tmp = posx, posy_tmp = posy, posz_tmp = posz;
                posx_tmp.erase(posx_tmp.begin() + i);
                posy_tmp.erase(posy_tmp.begin() + i);
                posz_tmp.erase(posz_tmp.begin() + i);
                if (!hit_correct::least_square_fit(posz_tmp, posx_tmp, kzx, bzx, rmse_zx)
                    || !hit_correct::least_square_fit(posz_tmp, posy_tmp, kzy, bzy, rmse_zy))
                {
                    b_hit_res_flag = true;
                    break;
                }
                double res_zx = abs(posx[i] - kzx * posz[i] - bzx);
                double res_zy = abs(posy[i] - kzy * posz[i] - bzy);
                if (res_zx > fit_hit_res_cut || res_zy > fit_hit_res_cut)
                {
                    b_hit_res_flag = true;
                    break;
                }
            }
            if (b_hit_res_flag)
            {
                continue;
            }

            // rmse cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy))
            {
                continue;
            }
            if (rmse_zx > fit_rmse_cut || rmse_zy > fit_rmse_cut)
            {
                continue;
            }

            // R2 cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            {
                continue;
            }
            if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            {
                continue;
            }

            // save res
            double posx_fit = kzx * m_correct->posz[n] + bzx;
            double delta_x = posx_fit - m_correct->posx[n];
            pos_res_x_hist[m_correct->idx_det[n]]->Fill(delta_x);
            double posy_fit = kzy * m_correct->posz[n] + bzy;
            double delta_y = posy_fit - m_correct->posy[n];
            pos_res_y_hist[m_correct->idx_det[n]]->Fill(delta_y);

            // debug
            // if (m_correct->idx_det[n] == 1
            //     && abs(delta_x) > 0.15)
            // {
            //     m_correct->print_png();
            // }

            // efficiency calc
            eff_cnt_x[m_correct->idx_det[n]][0]++;
            eff_cnt_y[m_correct->idx_det[n]][0]++;
            if (abs(delta_x) < 2)
            {
                eff_cnt_x[m_correct->idx_det[n]][1]++;
            }
            if (abs(delta_y) < 2)
            {
                eff_cnt_y[m_correct->idx_det[n]][1]++;
            }
        }
    }

    // cout efficiency
    for (int i = 0; i < cDET_LAYER; i++)
    {
        cout << "Efficiency X" << i << "=" << eff_cnt_x[i][1] / eff_cnt_x[i][0] << " ";
        cout << "Efficiency Y" << i << "=" << eff_cnt_y[i][1] / eff_cnt_y[i][0] << " ";
        cout << endl;
    }

    // calc the sigma of residual
    for (int i = 0; i < cDET_LAYER; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
            // TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -4, 4);
            // fitx->SetParameter(0, 50);
            // fitx->SetParameter(1, max_pos_x);
            // fitx->SetParameter(2, 0.4);
            // fitx->SetParLimits(2, 0, 1.0);
            // fitx->SetParameter(3, 5);
            // fitx->SetParameter(4, max_pos_x);
            // fitx->SetParameter(5, 2.5);
            // fitx->SetParLimits(5, 1.0, 5.0);
            // pos_res_x_hist[i]->Fit(fitx, "Q");
            // pos_res_x_hist[i]->Fit("gaus", "Q");
            pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x - 2, max_pos_x + 2);
            // pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x-0.3, max_pos_x+0.3);
            TF1 *fitx = pos_res_x_hist[i]->GetFunction("gaus");
            pos_res_x[i] = fitx->GetParameter(2);
            alignment[i][0] += fitx->GetParameter(1);
            cout << "x" << i << ": " << pos_res_x[i] << endl;
            // delete fitx;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
            // TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -4, 4);
            // fity->SetParameter(0, 50);
            // fity->SetParameter(1, max_pos_y);
            // fity->SetParameter(2, 0.4);
            // fity->SetParLimits(2, 0, 1.0);
            // fity->SetParameter(3, 5);
            // fity->SetParameter(4, max_pos_y);
            // fity->SetParameter(5, 2.5);
            // fity->SetParLimits(5, 1.0, 5.0);
            // pos_res_y_hist[i]->Fit(fity, "Q");
            // pos_res_y_hist[i]->Fit("gaus", "Q");
            pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y - 2, max_pos_y + 2);
            // pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y-0.3, max_pos_y+0.3);
            TF1 *fity = pos_res_y_hist[i]->GetFunction("gaus");
            pos_res_y[i] = fity->GetParameter(2);
            alignment[i][1] += fity->GetParameter(1);
            cout << "y" << i << ": " << pos_res_y[i] << endl;
            // delete fity;
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_x_hist[i]->Draw();
            pos_res_x_hist[i]->GetFunction("gaus")->Draw("same");
            // pos_res_x_hist[i]->GetFunction("fitx")->Draw("same");
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "detector_res_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            TCanvas *c2 = new TCanvas("c", "c", 800, 600);
            pos_res_y_hist[i]->Draw();
            pos_res_y_hist[i]->GetFunction("gaus")->Draw("same");
            // pos_res_y_hist[i]->GetFunction("fity")->Draw("same");
            string pngname2 = m_png_path + "detector_res_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        }
    }

    double detector_id[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    TGraph *pos_res_graphx = new TGraph(8, detector_id, pos_res_x);
    pos_res_graphx->GetXaxis()->SetTitle("detector_id");
    pos_res_graphx->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c1 = new TCanvas("c", "c", 800, 600);
    pos_res_graphx->Draw();
    list_file::create_path(m_png_path);
    string pngname1 = m_png_path + "detector_res_x.png";
    c1->Print(pngname1.data());
    delete c1;
    delete pos_res_graphx;

    TGraph *pos_res_graphy = new TGraph(8, detector_id, pos_res_y);
    pos_res_graphy->GetXaxis()->SetTitle("detector_id");
    pos_res_graphy->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c2 = new TCanvas("c", "c", 800, 600);
    pos_res_graphy->Draw();
    string pngname2 = m_png_path + "detector_res_y.png";
    c2->Print(pngname2.data());
    delete c2;
    delete pos_res_graphy;

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete pos_res_x_hist[i];
        delete pos_res_y_hist[i];
    }

    return true;
}

bool info_calc::run_pos_res_position_binned()
{
    const int position_bin_num = 8;
    double angle_max = 20;  // degree

    // show res info of each detector
    TH1F *pos_res_x_hist[cDET_LAYER_MAX][position_bin_num][position_bin_num];
    TH1F *pos_res_y_hist[cDET_LAYER_MAX][position_bin_num][position_bin_num];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < position_bin_num; j++)
        {
            for (int k = 0; k < position_bin_num; k++)
            {
                string namex = "x_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                string namey = "y_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                pos_res_x_hist[i][j][k] = new TH1F(namex.data(), namex.data(), 100, -100, 100);
                pos_res_y_hist[i][j][k] = new TH1F(namey.data(), namey.data(), 100, -100, 100);
            }
        }
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10);
    m_correct->set_ndet_num_cut(cDET_LAYER);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10);

    double fit_hit_res_cut = 1.0;
    double fit_rmse_cut = 1.0;
    double fit_R2_cut = 0.95;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        int N = m_correct->idx_det.size();  // layer counter of this event
        // cout << "detector of hits: " << N << endl;
        for (int n = 0; n < N; n++)
        {
            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;

            int layer = idx_det[n];

            posx.erase(posx.begin() + n);
            posy.erase(posy.begin() + n);
            posz.erase(posz.begin() + n);
            idx_det.erase(idx_det.begin() + n);

            double kzx, bzx, rmse_zx, R2_zx;
            double kzy, bzy, rmse_zy, R2_zy;
            vector<double> posx_tmp, posy_tmp, posz_tmp;

            // rmse cut
            // R2 cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            {
                continue;
            }
            if (rmse_zx > fit_rmse_cut || rmse_zy > fit_rmse_cut)
            {
                continue;
            }
            if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            {
                continue;
            }

            double position_bin_size = cDET_SIZE[layer] / position_bin_num;  // mm
            // save res
            double posx_fit = kzx * m_correct->posz[n] + bzx;
            double delta_x = posx_fit - m_correct->posx[n];
            double anglezx = abs(atan(kzx)) / TMath::Pi() * 180;
            int position_idzx = (int)(posx_fit / position_bin_size);
            if (position_idzx >= position_bin_num || position_idzx < 0)
                continue;

            double posy_fit = kzy * m_correct->posz[n] + bzy;
            double delta_y = posy_fit - m_correct->posy[n];
            double anglezy = abs(atan(kzy)) / TMath::Pi() * 180;
            int position_idzy = (int)(posy_fit / position_bin_size);
            if (position_idzy >= position_bin_num || position_idzy < 0)
                continue;

            if (anglezx < angle_max)
                pos_res_x_hist[layer][position_idzx][position_idzy]->Fill(delta_x);
            if (anglezy < angle_max)
                pos_res_y_hist[layer][position_idzx][position_idzy]->Fill(delta_y);
        }
    }

    TH2D *pos_res_x_mean[cDET_LAYER_MAX], *pos_res_x_sigma[cDET_LAYER_MAX];
    TH2D *pos_res_y_mean[cDET_LAYER_MAX], *pos_res_y_sigma[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string namex_mean = "pos_res_x_mean_" + to_string(i);
        pos_res_x_mean[i] = new TH2D(namex_mean.data(), namex_mean.data(), position_bin_num, 0, cDET_SIZE[i],
                                     position_bin_num, 0, cDET_SIZE[i]);
        string namex_sigma = "pos_res_x_sigma_" + to_string(i);
        pos_res_x_sigma[i] = new TH2D(namex_sigma.data(), namex_sigma.data(), position_bin_num, 0, cDET_SIZE[i],
                                      position_bin_num, 0, cDET_SIZE[i]);
        string namey_mean = "pos_res_y_mean_" + to_string(i);
        pos_res_y_mean[i] = new TH2D(namey_mean.data(), namey_mean.data(), position_bin_num, 0, cDET_SIZE[i],
                                     position_bin_num, 0, cDET_SIZE[i]);
        string namey_sigma = "pos_res_y_sigma_" + to_string(i);
        pos_res_y_sigma[i] = new TH2D(namey_sigma.data(), namey_sigma.data(), position_bin_num, 0, cDET_SIZE[i],
                                      position_bin_num, 0, cDET_SIZE[i]);
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < position_bin_num; j++)
        {
            for (int k = 0; k < position_bin_num; k++)
            {
                if (pos_res_x_hist[i][j][k]->GetEntries() > 0)
                {
                    double max_pos_x = pos_res_x_hist[i][j][k]->GetBinCenter(pos_res_x_hist[i][j][k]->GetMaximumBin());
                    pos_res_x_hist[i][j][k]->Fit("gaus", "Q", "", max_pos_x - 2, max_pos_x + 2);
                    TF1 *fitx = pos_res_x_hist[i][j][k]->GetFunction("gaus");
                    if (fitx)
                    {
                        double position_bin_size = cDET_SIZE[i] / position_bin_num;  // mm
                        double meanx = fitx->GetParameter(1);
                        double sigmax = fitx->GetParameter(2);
                        pos_res_x_mean[i]->Fill(j * position_bin_size, k * position_bin_size, meanx);
                        pos_res_x_sigma[i]->Fill(j * position_bin_size, k * position_bin_size, sigmax);
                    }

                    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
                    pos_res_x_hist[i][j][k]->Draw();
                    list_file::create_path(m_png_path);
                    string pngname = m_png_path + "position_binned_res_x_" + to_string(i) + "_" + to_string(j) + "_"
                                   + to_string(k) + ".png";
                    c1->Print(pngname.data());
                    delete c1;
                }

                if (pos_res_y_hist[i][j][k]->GetEntries() > 0)
                {
                    double max_pos_y = pos_res_y_hist[i][j][k]->GetBinCenter(pos_res_y_hist[i][j][k]->GetMaximumBin());
                    pos_res_y_hist[i][j][k]->Fit("gaus", "Q", "", max_pos_y - 2, max_pos_y + 2);
                    TF1 *fity = pos_res_y_hist[i][j][k]->GetFunction("gaus");
                    if (fity)
                    {
                        double position_bin_size = cDET_SIZE[i] / position_bin_num;  // mm
                        double meany = fity->GetParameter(1);
                        double sigmay = fity->GetParameter(2);
                        pos_res_y_mean[i]->Fill(j * position_bin_size, k * position_bin_size, meany);
                        pos_res_y_sigma[i]->Fill(j * position_bin_size, k * position_bin_size, sigmay);
                    }

                    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
                    pos_res_y_hist[i][j][k]->Draw();
                    list_file::create_path(m_png_path);
                    string pngname = m_png_path + "position_binned_res_y_" + to_string(i) + "_" + to_string(j) + "_"
                                   + to_string(k) + ".png";
                    c1->Print(pngname.data());
                    delete c1;
                }
            }
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
        string pngname;

        pos_res_x_mean[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        pngname = m_png_path + "position_binned_res_x_mean_" + to_string(i) + ".png";
        c1->Print(pngname.data());
        pos_res_x_sigma[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        pngname = m_png_path + "position_binned_res_x_sigma_" + to_string(i) + ".png";
        c1->Print(pngname.data());

        pos_res_y_mean[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        pngname = m_png_path + "position_binned_res_y_mean_" + to_string(i) + ".png";
        c1->Print(pngname.data());
        pos_res_y_sigma[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        pngname = m_png_path + "position_binned_res_y_sigma_" + to_string(i) + ".png";
        c1->Print(pngname.data());

        delete c1;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < position_bin_num; j++)
        {
            for (int k = 0; k < position_bin_num; k++)
            {
                delete pos_res_x_hist[i][j][k];
                delete pos_res_y_hist[i][j][k];
            }
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete pos_res_x_mean[i];
        delete pos_res_x_sigma[i];
        delete pos_res_y_mean[i];
        delete pos_res_y_sigma[i];
    }

    return true;
}

bool info_calc::run_pos_res_angular_binned()
{
    const int angle_bin_num = 1;
    double angle_bin_size = 5;                          // degree
    double angle_max = angle_bin_size * angle_bin_num;  // degree

    // show res info of each detector
    TH1F *pos_res_x_hist[cDET_LAYER_MAX][angle_bin_num];
    TH1F *pos_res_y_hist[cDET_LAYER_MAX][angle_bin_num];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < angle_bin_num; j++)
        {
            string namex = "x_" + to_string(i) + "_" + to_string(j);
            string namey = "y_" + to_string(i) + "_" + to_string(j);
            pos_res_x_hist[i][j] = new TH1F(namex.data(), namex.data(), 200, -5, 5);
            pos_res_y_hist[i][j] = new TH1F(namey.data(), namey.data(), 200, -5, 5);
        }
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(6);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 50.0;
    double fit_rmse_cut = 50.0;
    double fit_R2_cut = 0.1;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        int N = m_correct->idx_det.size();  // layer counter of this event
        // cout << "detector of hits: " << N << endl;

        // remove layer 5
        // for (int n=0; n<N; n++)
        // {
        //     if (m_correct->idx_det[n] == 5)
        //     {
        //         m_correct->posx.erase(m_correct->posx.begin()+n);
        //         m_correct->posy.erase(m_correct->posy.begin()+n);
        //         m_correct->posz.erase(m_correct->posz.begin()+n);
        //         m_correct->idx_det.erase(m_correct->idx_det.begin()+n);
        //         N--;
        //         break;
        //     }
        // }
        // if (N != cDET_LAYER-1)
        // {
        //     continue;
        // }

        for (int n = 0; n < N; n++)
        {
            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;

            int layer = idx_det[n];

            posx.erase(posx.begin() + n);
            posy.erase(posy.begin() + n);
            posz.erase(posz.begin() + n);
            idx_det.erase(idx_det.begin() + n);

            double kzx, bzx, rmse_zx, R2_zx;
            double kzy, bzy, rmse_zy, R2_zy;
            vector<double> posx_tmp, posy_tmp, posz_tmp;

            // hit res cut
            // bool b_hit_res_flag = false;
            // for (int i=0; i<posx.size(); i++)
            // {
            //     posx_tmp = posx, posy_tmp = posy, posz_tmp = posz;
            //     posx_tmp.erase(posx_tmp.begin()+i);
            //     posy_tmp.erase(posy_tmp.begin()+i);
            //     posz_tmp.erase(posz_tmp.begin()+i);
            //     if (!hit_correct::least_square_fit(posz_tmp, posx_tmp, kzx, bzx, rmse_zx) ||
            //         !hit_correct::least_square_fit(posz_tmp, posy_tmp, kzy, bzy, rmse_zy))
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            //     double res_zx = abs(posx[i]-kzx*posz[i]-bzx);
            //     double res_zy = abs(posy[i]-kzy*posz[i]-bzy);
            //     if (res_zx > fit_hit_res_cut || res_zy > fit_hit_res_cut)
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            // }
            // if (b_hit_res_flag)
            // {
            //     continue;
            // }

            // rmse cut
            // R2 cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            {
                continue;
            }
            if (rmse_zx > fit_rmse_cut || rmse_zy > fit_rmse_cut)
            {
                continue;
            }
            if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            {
                continue;
            }

            // save res
            double posx_fit = kzx * m_correct->posz[n] + bzx;
            double delta_x = posx_fit - m_correct->posx[n];
            double anglezx = abs(atan(kzx)) / TMath::Pi() * 180;
            int angle_idzx = (int)(anglezx / angle_bin_size);
            if (angle_idzx < angle_bin_num)
                // if (angle_idzx < angle_bin_num && kzx > 0)
                // if (angle_idzx < angle_bin_num
                //     && m_correct->posx[n] > 100
                //     && m_correct->posx[n] < 150
                //     && m_correct->posy[n] > 100
                //     && m_correct->posy[n] < 150)
                pos_res_x_hist[layer][angle_idzx]->Fill(delta_x);
            // else
            //     cout << "angle idx x: " << angle_idzx << endl;

            double posy_fit = kzy * m_correct->posz[n] + bzy;
            double delta_y = posy_fit - m_correct->posy[n];
            double anglezy = abs(atan(kzy)) / TMath::Pi() * 180;
            int angle_idzy = (int)(anglezy / angle_bin_size);
            if (angle_idzy < angle_bin_num)
                pos_res_y_hist[layer][angle_idzy]->Fill(delta_y);
            // else
            //     cout << "angle idx y: " << angle_idzy << endl;

            // for all decode data
            // double posx_fit = kzx * m_correct->posz[n] + bzx;
            // double anglezx = abs(atan(kzx)) / TMath::Pi() * 180;
            // int angle_idzx = (int) (anglezx/angle_bin_size);
            // double delta_x = 10000;
            // for (int i=0; i<20; i++)
            // {
            //     double xx = detector_data[layer]->x_other[i] * cSTRIP_WIDTH;
            //     double yy = m_correct->posy[n];
            //     double zz = m_correct->posz[n];
            //     m_correct->alignment_data(xx, yy, zz, layer);
            //     if (xx > 0)
            //     {
            //         double delta_x_tmp = posx_fit - xx;
            //         if (abs(delta_x_tmp) < abs(delta_x))
            //         {
            //             delta_x = delta_x_tmp;
            //         }
            //     }
            // }
            // if (angle_idzx < angle_bin_num)
            //     pos_res_x_hist[layer][angle_idzx]->Fill(delta_x);

            // double posy_fit = kzy * m_correct->posz[n] + bzy;
            // double anglezy = abs(atan(kzy)) / TMath::Pi() * 180;
            // int angle_idzy = (int) (anglezy/angle_bin_size);
            // double delta_y = 10000;
            // for (int i=0; i<20; i++)
            // {
            //     double yy = detector_data[layer]->y_other[i] * cSTRIP_WIDTH;
            //     double xx = m_correct->posx[n];
            //     double zz = m_correct->posz[n];
            //     m_correct->alignment_data(xx, yy, zz, layer);
            //     if (yy > 0)
            //     {
            //         double delta_y_tmp = posy_fit - yy;
            //         if (abs(delta_y_tmp) < abs(delta_y))
            //         {
            //             delta_y = delta_y_tmp;
            //         }
            //     }
            // }
            // if (angle_idzy < angle_bin_num)
            //     pos_res_y_hist[layer][angle_idzy]->Fill(delta_y);
        }
    }

    // calc the sigma of residual
    double pos_res_x[cDET_LAYER_MAX][angle_bin_num] = {0};
    double pos_res_y[cDET_LAYER_MAX][angle_bin_num] = {0};
    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < angle_bin_num; j++)
        {
            if (pos_res_x_hist[i][j]->GetEntries() > 0)
            {
                // 单高斯拟合
                double max_pos_x = pos_res_x_hist[i][j]->GetBinCenter(pos_res_x_hist[i][j]->GetMaximumBin());
                // pos_res_x_hist[i]->Fit("gaus", "Q");
                // pos_res_x_hist[i][j]->Fit("gaus", "Q", "", max_pos_x-0.3, max_pos_x+0.3);
                pos_res_x_hist[i][j]->Fit("gaus", "Q", "", max_pos_x - 2, max_pos_x + 2);
                TF1 *fitx = pos_res_x_hist[i][j]->GetFunction("gaus");
                if (fitx)
                {
                    pos_res_x[i][j] = fitx->GetParameter(2);
                }
                cout << i << "x" << j << ": " << pos_res_x[i][j] << endl;

                // 双高斯拟合
                // double max_pos_x = pos_res_x_hist[i][j]->
                //     GetBinCenter(pos_res_x_hist[i][j]->GetMaximumBin());
                // TF1 *fitx = new TF1("fitx", "gaus(0)+gaus(3)", -10, 10);
                // fitx->SetParameter(0, 50);
                // fitx->SetParameter(1, max_pos_x);
                // fitx->SetParameter(2, 0.4);
                // fitx->SetParLimits(2, 0, 2.0);
                // fitx->SetParameter(3, 5);
                // fitx->SetParameter(4, max_pos_x);
                // fitx->SetParameter(5, 5);
                // fitx->SetParLimits(5, 1.0, 20.0);
                // pos_res_x_hist[i][j]->Fit(fitx, "Q");
                // pos_res_x[i][j] = fitx->GetParameter(2);
                // cout << i << "x" << j << ": " << pos_res_x[i][j] << endl;
                // delete fitx;
            }

            if (pos_res_y_hist[i][j]->GetEntries() > 0)
            {
                // 单高斯拟合
                double max_pos_y = pos_res_y_hist[i][j]->GetBinCenter(pos_res_y_hist[i][j]->GetMaximumBin());
                // pos_res_y_hist[i]->Fit("gaus", "Q");
                // pos_res_y_hist[i][j]->Fit("gaus", "Q", "", max_pos_y-0.3, max_pos_y+0.3);
                pos_res_y_hist[i][j]->Fit("gaus", "Q", "", max_pos_y - 1, max_pos_y + 1);
                TF1 *fity = pos_res_y_hist[i][j]->GetFunction("gaus");
                if (fity)
                {
                    pos_res_y[i][j] = fity->GetParameter(2);
                }
                cout << i << "y" << j << ": " << pos_res_y[i][j] << endl;

                // 双高斯拟合
                // double max_pos_y = pos_res_y_hist[i][j]->
                //     GetBinCenter(pos_res_y_hist[i][j]->GetMaximumBin());
                // TF1 *fity = new TF1("fity", "gaus(0)+gaus(3)", -10, 10);
                // fity->SetParameter(0, 50);
                // fity->SetParameter(1, max_pos_y);
                // fity->SetParameter(2, 0.4);
                // fity->SetParLimits(2, 0, 2.0);
                // fity->SetParameter(3, 5);
                // fity->SetParameter(4, max_pos_y);
                // fity->SetParameter(5, 5);
                // fity->SetParLimits(5, 1.0, 20.0);
                // pos_res_y_hist[i][j]->Fit(fity, "Q");
                // pos_res_y[i][j] = fity->GetParameter(2);
                // cout << i << "y" << j << ": " << pos_res_y[i][j] << endl;
                // delete fity;
            }
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < angle_bin_num; j++)
        {
            if (pos_res_x_hist[i][j]->GetEntries() > 0)
            {
                TCanvas *c1 = new TCanvas("c", "c", 800, 600);
                pos_res_x_hist[i][j]->Draw();
                // pos_res_x_hist[i][j]->GetFunction("gaus")->Draw("same");
                list_file::create_path(m_png_path);
                string pngname1 = m_png_path + "detector_res_x_" + to_string(i) + "_" + to_string(j) + ".png";
                c1->Print(pngname1.data());
                delete c1;
            }

            if (pos_res_y_hist[i][j]->GetEntries() > 0)
            {
                TCanvas *c2 = new TCanvas("c", "c", 800, 600);
                pos_res_y_hist[i][j]->Draw();
                // pos_res_y_hist[i][j]->GetFunction("gaus")->Draw("same");
                string pngname2 = m_png_path + "detector_res_y_" + to_string(i) + "_" + to_string(j) + ".png";
                c2->Print(pngname2.data());
                delete c2;
            }
        }
    }

    double detector_id[angle_bin_num];
    for (int i = 0; i < angle_bin_num; i++)
    {
        detector_id[i] = i * angle_bin_size;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TGraph *pos_res_graphx = new TGraph(angle_bin_num, detector_id, pos_res_x[i]);
        pos_res_graphx->GetXaxis()->SetTitle("angle(degree)");
        pos_res_graphx->GetYaxis()->SetTitle("resolution(mm)");
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        pos_res_graphx->Draw("APL*");
        string pngname1 = m_png_path + "detector_res_x_" + to_string(i) + ".png";
        list_file::create_path(m_png_path);
        c1->Print(pngname1.data());
        delete c1;
        delete pos_res_graphx;

        TGraph *pos_res_graphy = new TGraph(angle_bin_num, detector_id, pos_res_y[i]);
        pos_res_graphy->GetXaxis()->SetTitle("angle(degree)");
        pos_res_graphy->GetYaxis()->SetTitle("resolution(mm)");
        TCanvas *c2 = new TCanvas("c", "c", 800, 600);
        pos_res_graphy->Draw("APL*");
        string pngname2 = m_png_path + "detector_res_y_" + to_string(i) + ".png";
        list_file::create_path(m_png_path);
        c2->Print(pngname2.data());
        delete c2;
        delete pos_res_graphy;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < angle_bin_num; j++)
        {
            delete pos_res_x_hist[i][j];
            delete pos_res_y_hist[i][j];
        }
    }

    return true;
}
*/

/*
bool info_calc::run_pos_res_angular_binned_micro_TPC()
{
*/
    /*
    // 100MHz 180ns 550V 688V
    const int angle_bin_num = 6;
    double angle_bin_size = 5; // degree
    double angle_max = angle_bin_size * angle_bin_num; // degree

    const double c_drift_time_zero = 1372.94; // ns
    const double c_drift_time_zx[angle_bin_num] = {
        55.2219, 75.9757, 77.2623, 88.7353, 98.144, 100.616
    };
    const double c_drift_time_zy[angle_bin_num] = {
        61.1912, 89.1183, 90.5915, 82.7226, 89.4878, 89.0816
    };
    const double c_drift_time_theory = 106.5; // ns
    */
/*
    // 100MHz 180ns 550V 600V
    const int angle_bin_num = 6;
    double angle_bin_size = 5;                          // degree
    double angle_max = angle_bin_size * angle_bin_num;  // degree

    const double c_drift_time_zero = 1580;  // ns
    const double c_drift_time_zx[angle_bin_num] = {
        // 55.2219, 75.9757, 77.2623, 88.7353, 98.144, 100.616
    };
    const double c_drift_time_zy[angle_bin_num] = {
        // 61.1912, 89.1183, 90.5915, 82.7226, 89.4878, 89.0816
    };
    const double c_drift_time_theory = 570.457;  // ns

    // modified drift time parameters

    const double c_drift_time_zero_list[cDET_LAYER_MAX] = {// 1579.85, 1578.13, 1560.38, 1587.47
                                                           1590.05, 1567.55, 1521.85, 1594.91};
    const double c_drift_time_theory_list[cDET_LAYER_MAX] = {570.457, 570.457, 570.457, 570.457};

    // show res info of each detector
    TH1F *pos_res_x_hist[angle_bin_num];
    TH1F *pos_res_y_hist[angle_bin_num];
    for (int i = 0; i < angle_bin_num; i++)
    {
        string namex = "x" + to_string(i);
        string namey = "y" + to_string(i);
        pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -10, 10);
        pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -10, 10);
    }

    // error of charge-center and micro tpc
    TH1D *error_hist = new TH1D("error_hist", "error_hist", 100, -10, 10);

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(4);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 12.0;
    double fit_rmse_cut = 0.5;
    double fit_R2_cut = 0.95;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        // micro-TPC
        vector<double> x, y, z;
        vector<int> idx;

        int angle_idzx = (int)(m_correct->theta_zx * rad2deg / angle_bin_size);
        int angle_idzy = (int)(m_correct->theta_zy * rad2deg / angle_bin_size);
        if (angle_idzx >= angle_bin_num || angle_idzy >= angle_bin_num)
        {
            continue;
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1 && detector_data[i]->x > 0 && detector_data[i]->y > 0)
            {
                // zx, use hough trans
                vector<double> pos_x, dis_x, time_x, amp_x;
                for (int j = 0; j < detector_data[i]->strips_num_x; j++)
                {
                    double pos = detector_data[i]->hit_chn_x[j];
                    double dis =
                        (detector_data[i]->hit_time_x[j] - c_drift_time_zero_list[i]) * 5. / c_drift_time_theory
                        // * 5. / c_drift_time_zx[angle_idzx]
                        + detector_data[i]->z;
                    double d =
                        abs(m_correct->kzx * dis - pos + m_correct->bzx) / sqrt(m_correct->kzx * m_correct->kzx + 1);
                    // if (d < 0.5) // 就像是知道答案找过程似的
                    if (true)
                    {
                        pos_x.push_back(pos);
                        dis_x.push_back(dis - detector_data[i]->z);
                        time_x.push_back(detector_data[i]->hit_time_x[j]);
                        amp_x.push_back(detector_data[i]->hit_amp_x[j]);
                    }
                }
                // check hit number
                if (pos_x.size() > 1)
                {
                    // TH2D *h2 = new TH2D("h2", "h2", 2000, 0, 200, 400, -20, 20);
                    // for (int j=0; j<pos_x.size(); j++)
                    // {
                    //     h2->Fill(pos_x[j], dis_x[j], amp_x[j]);
                    // }
                    // h2->Fit("pol1", "Q");
                    // TF1 *fit = h2->GetFunction("pol1");

                    TGraph *g1 = new TGraph(pos_x.size(), &pos_x[0], &dis_x[0]);
                    g1->Fit("pol1", "Q");
                    TF1 *fit = g1->GetFunction("pol1");

                    if (fit)
                    {
                        double slope_nt = fit->GetParameter(1);
                        double const_nt = fit->GetParameter(0);
                        double chn_rec = -const_nt / slope_nt;

                        double error = chn_rec - detector_data[i]->x;
                        error_hist->Fill(error);
                        if (abs(error) < 2)
                        {
                            x.push_back(chn_rec);
                        }
                        else
                        {
                            x.push_back(-detector_data[i]->x);
                        }
                    }
                    else
                    {
                        x.push_back(-detector_data[i]->x);

                        for (int j = 0; j < pos_x.size(); j++)
                        {
                            double dis = (time_x[j] - c_drift_time_zero_list[i]) * 5. / c_drift_time_theory;
                            // * 5. / c_drift_time_zx[angle_idzx];
                            cout << " " << pos_x[j] << " " << dis;
                        }
                        cout << endl;
                    }

                    // delete h2;
                    delete g1;
                }
                else
                {
                    x.push_back(-detector_data[i]->x);
                }

                // zy, use hough trans
                vector<double> pos_y, dis_y, time_y, amp_y;
                for (int j = 0; j < detector_data[i]->strips_num_y; j++)
                {
                    double pos = detector_data[i]->hit_chn_y[j];
                    double dis =
                        (detector_data[i]->hit_time_y[j] - c_drift_time_zero) * 5. / c_drift_time_zy[angle_idzy]
                        + detector_data[i]->z;
                    double d =
                        abs(m_correct->kzy * dis - pos + m_correct->bzy) / sqrt(m_correct->kzy * m_correct->kzy + 1);
                    if (d < 2)
                    {
                        pos_y.push_back(pos);
                        dis_y.push_back(dis);
                        time_y.push_back(detector_data[i]->hit_time_y[j]);
                        amp_y.push_back(detector_data[i]->hit_amp_y[j]);
                    }
                }
                // check hit number
                // if (pos_y.size() > 1)
                if (false)
                {
                    TH2D *h2 = new TH2D("h2", "h2", 1500, 0, 150, 200, -20, 20);
                    for (int j = 0; j < pos_y.size(); j++)
                    {
                        double dis = (time_y[j] - c_drift_time_zero) * 5. / c_drift_time_zy[angle_idzy];
                        h2->Fill(pos_y[j], dis, amp_y[i]);
                    }
                    h2->Fit("pol1", "Q");
                    TF1 *fit = h2->GetFunction("pol1");
                    if (fit)
                    {
                        double slope_nt = fit->GetParameter(1);
                        double const_nt = fit->GetParameter(0);
                        double chn_rec = -const_nt / slope_nt;

                        double error = chn_rec - detector_data[i]->y;
                        error_hist->Fill(error);
                        if (abs(error) < 2)
                        {
                            y.push_back(chn_rec);
                        }
                        else
                        {
                            y.push_back(detector_data[i]->y);
                        }
                    }
                    else
                    {
                        y.push_back(detector_data[i]->y);
                    }
                    delete h2;
                }
                else
                {
                    y.push_back(detector_data[i]->y);
                }

                z.push_back(detector_data[i]->z);
                idx.push_back(i);
            }
        }

        // if (m_correct->theta_zx*rad2deg > 10
        //     || m_correct->theta_zy*rad2deg > 10)
        {
            m_correct->set_evt_data(x, y, z, idx);
            if (m_correct->correct() != 10)
                continue;
        }

        int N = m_correct->idx_det.size();  // layer counter of this event
        for (int n = 0; n < N; n++)
        {
            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;

            int layer = idx_det[n];
            if (layer != 1)
            {
                continue;
            }

            posx.erase(posx.begin() + n);
            posy.erase(posy.begin() + n);
            posz.erase(posz.begin() + n);
            idx_det.erase(idx_det.begin() + n);

            double kzx, bzx, rmse_zx, R2_zx;
            double kzy, bzy, rmse_zy, R2_zy;
            vector<double> posx_tmp, posy_tmp, posz_tmp;

            // hit res cut
            // bool b_hit_res_flag = false;
            // for (int i=0; i<posx.size(); i++)
            // {
            //     posx_tmp = posx, posy_tmp = posy, posz_tmp = posz;
            //     posx_tmp.erase(posx_tmp.begin()+i);
            //     posy_tmp.erase(posy_tmp.begin()+i);
            //     posz_tmp.erase(posz_tmp.begin()+i);
            //     if (!hit_correct::least_square_fit(posz_tmp, posx_tmp, kzx, bzx, rmse_zx) ||
            //         !hit_correct::least_square_fit(posz_tmp, posy_tmp, kzy, bzy, rmse_zy))
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            //     double res_zx = abs(posx[i]-kzx*posz[i]-bzx);
            //     double res_zy = abs(posy[i]-kzy*posz[i]-bzy);
            //     if (res_zx > fit_hit_res_cut || res_zy > fit_hit_res_cut)
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            // }
            // if (b_hit_res_flag)
            // {
            //     continue;
            // }

            // rmse cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy))
            {
                continue;
            }
            if (rmse_zx > fit_rmse_cut || rmse_zy > fit_rmse_cut)
            {
                continue;
            }

            // R2 cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx)
                || !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            {
                continue;
            }
            if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            {
                continue;
            }

            // save res
            double posx_fit = kzx * m_correct->posz[n] + bzx;
            double delta_x = posx_fit - m_correct->posx[n];
            double anglezx = abs(atan(kzx)) / TMath::Pi() * 180;
            int angle_idzx = (int)(anglezx / angle_bin_size);
            if (angle_idzx < angle_bin_num)
                pos_res_x_hist[angle_idzx]->Fill(delta_x);
            // else
            //     cout << "angle idx x: " << angle_idzx << endl;

            double posy_fit = kzy * m_correct->posz[n] + bzy;
            double delta_y = posy_fit - m_correct->posy[n];
            double anglezy = abs(atan(kzy)) / TMath::Pi() * 180;
            int angle_idzy = (int)(anglezy / angle_bin_size);
            if (angle_idzy < angle_bin_num)
                pos_res_y_hist[angle_idzy]->Fill(delta_y);
            // else
            //     cout << "angle idx y: " << angle_idzy << endl;
        }
    }

    // calc the sigma of residual
    double pos_res_x[angle_bin_num] = {0};
    double pos_res_y[angle_bin_num] = {0};
    for (int i = 0; i < angle_bin_num; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            double max_pos_x = pos_res_x_hist[i]->GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
            // pos_res_x_hist[i]->Fit("gaus", "Q");
            pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x - 1, max_pos_x + 1);
            // pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x-2, max_pos_x+2);
            TF1 *fitx = pos_res_x_hist[i]->GetFunction("gaus");
            pos_res_x[i] = fitx->GetParameter(2);
            cout << "x" << i << ": " << pos_res_x[i] << endl;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            double max_pos_y = pos_res_y_hist[i]->GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
            // pos_res_y_hist[i]->Fit("gaus", "Q");
            // pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y-0.3, max_pos_y+0.3);
            pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y - 2, max_pos_y + 2);
            TF1 *fity = pos_res_y_hist[i]->GetFunction("gaus");
            pos_res_y[i] = fity->GetParameter(2);
            cout << "y" << i << ": " << pos_res_y[i] << endl;
        }
    }

    for (int i = 0; i < angle_bin_num; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_x_hist[i]->Draw();
            pos_res_x_hist[i]->GetFunction("gaus")->Draw("same");
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "tpc-detector_res_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            TCanvas *c2 = new TCanvas("c", "c", 800, 600);
            pos_res_y_hist[i]->Draw();
            pos_res_y_hist[i]->GetFunction("gaus")->Draw("same");
            string pngname2 = m_png_path + "tpc-detector_res_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        }
    }

    double detector_id[angle_bin_num];
    for (int i = 0; i < angle_bin_num; i++)
    {
        detector_id[i] = i * angle_bin_size;
    }

    TGraph *pos_res_graphx = new TGraph(angle_bin_num, detector_id, pos_res_x);
    pos_res_graphx->GetXaxis()->SetTitle("angle(degree)");
    pos_res_graphx->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c1 = new TCanvas("c", "c", 800, 600);
    pos_res_graphx->Draw();
    string pngname1 = m_png_path + "tpc-detector_res_x.png";
    list_file::create_path(m_png_path);
    c1->Print(pngname1.data());
    delete c1;
    delete pos_res_graphx;

    TGraph *pos_res_graphy = new TGraph(angle_bin_num, detector_id, pos_res_y);
    pos_res_graphy->GetXaxis()->SetTitle("angle(degree)");
    pos_res_graphy->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c2 = new TCanvas("c", "c", 800, 600);
    pos_res_graphy->Draw();
    string pngname2 = m_png_path + "tpc-detector_res_y.png";
    list_file::create_path(m_png_path);
    c2->Print(pngname2.data());
    delete c2;
    delete pos_res_graphy;

    // error
    TCanvas *c = new TCanvas("c", "c", 800, 600);
    error_hist->Draw();
    string pngname = m_png_path + "error_hist.png";
    list_file::create_path(m_png_path);
    c->Print(pngname.data());
    delete c;

    for (int i = 0; i < angle_bin_num; i++)
    {
        delete pos_res_x_hist[i];
        delete pos_res_y_hist[i];
    }

    return true;
}


/*
bool info_calc::run_pos_res_angular_binned_micro_TPC()
{
    const int angle_bin_num = 6;
    double angle_bin_size = 5; // degree
    double angle_max = angle_bin_size * angle_bin_num; // degree

    const double c_drift_time_zero = 1372.94; // ns
    const double c_drift_time_zx[angle_bin_num] = {
        55.2219, 75.9757, 77.2623, 88.7353, 98.144, 100.616
    };
    const double c_drift_time_zy[angle_bin_num] = {
        61.1912, 89.1183, 90.5915, 82.7226, 89.4878, 89.0816
    };

    // show res info of each detector
    TH1F *pos_res_x_hist[angle_bin_num];
    TH1F *pos_res_y_hist[angle_bin_num];
    for (int i=0; i<angle_bin_num; i++)
    {
        string namex = "x" + to_string(i);
        string namey = "y" + to_string(i);
        pos_res_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -10, 10);
        pos_res_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -10, 10);
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(4);
    m_correct->set_theta_cut(90./180*3.1415);
    m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 12.0;
    double fit_rmse_cut = 12.0;
    double fit_R2_cut = 0.5;
    for (int evt=0; evt<entries; evt++)
    {
        m_input_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10) continue;

        // micro-TPC
        vector<double> x, y, z;
        vector<int> idx;

        for (int i=0; i<0; i++)
        {
            if (detector_data[i]->sig == 1
                && detector_data[i]->x > 0
                && detector_data[i]->y > 0)
            {
                if (m_correct->theta_zx*rad2deg > 5)
                {
                    TH2D *h2 = new TH2D("h2", "h2", 1500, 0, 150, 200, -10, 10);
                    for (int j=0; j<detector_data[i]->strips_num_x; j++)
                    {
                        double dis = (detector_data[i]->hit_time_x[j]-c_drift_time_zero)*0.0087649;
                        if (dis < -10) dis = -10;
                        if (dis > 10) dis = 10;
                        h2->Fill(detector_data[i]->hit_chn_x[j],
                                 dis,
                                 detector_data[i]->hit_amp_x[i]);
                    }
                    h2->Fit("pol1", "Q");
                    TF1 *fit = h2->GetFunction("pol1");
                    if (fit)
                    {
                        double slope_nt = fit->GetParameter(1);
                        double const_nt = fit->GetParameter(0);
                        double chn_rec = -const_nt/slope_nt;
                        x.push_back(chn_rec);
                    }
                    else
                    {
                        x.push_back(detector_data[i]->x);
                    }
                    delete h2;
                }
                else
                {
                    x.push_back(detector_data[i]->x);
                }
                if (m_correct->theta_zy*rad2deg > 5)
                {
                    TH2D *h2 = new TH2D("h2", "h2", 1500, 0, 150, 200, -10, 10);
                    for (int j=0; j<detector_data[i]->strips_num_y; j++)
                    {
                        double dis = (detector_data[i]->hit_time_y[j]-c_drift_time_zero)*0.0087649;
                        if (dis < -10) dis = -10;
                        if (dis > 10) dis = 10;
                        h2->Fill(detector_data[i]->hit_chn_y[j],
                                 dis,
                                 detector_data[i]->hit_amp_y[i]);
                    }
                    h2->Fit("pol1", "Q");
                    TF1 *fit = h2->GetFunction("pol1");
                    if (fit)
                    {
                        double slope_nt = fit->GetParameter(1);
                        double const_nt = fit->GetParameter(0);
                        double chn_rec = -const_nt/slope_nt;
                        y.push_back(chn_rec);
                    }
                    else
                    {
                        y.push_back(detector_data[i]->y);
                    }
                    delete h2;
                }
                else
                {
                    y.push_back(detector_data[i]->y);
                }
                z.push_back(detector_data[i]->z);
                idx.push_back(i);
            }
        }

        // if (m_correct->theta_zx*rad2deg > 10
        //     || m_correct->theta_zy*rad2deg > 10)
        // {
        //     m_correct->set_evt_data(x, y, z, idx);
        //     if (m_correct->correct() != 10) continue;
        // }

        int N = m_correct->idx_det.size(); // layer counter of this event
        for (int n=0; n<N; n++)
        {
            if (m_correct->idx_det[n] == 0
                || m_correct->idx_det[n] == 3)
            {
                continue;
            }

            vector<double> posx = m_correct->posx;
            vector<double> posy = m_correct->posy;
            vector<double> posz = m_correct->posz;
            vector<int> idx_det = m_correct->idx_det;

            posx.erase(posx.begin()+n);
            posy.erase(posy.begin()+n);
            posz.erase(posz.begin()+n);
            idx_det.erase(idx_det.begin()+n);

            double kzx, bzx, rmse_zx, R2_zx;
            double kzy, bzy, rmse_zy, R2_zy;
            vector<double> posx_tmp, posy_tmp, posz_tmp;

            // hit res cut
            // bool b_hit_res_flag = false;
            // for (int i=0; i<posx.size(); i++)
            // {
            //     posx_tmp = posx, posy_tmp = posy, posz_tmp = posz;
            //     posx_tmp.erase(posx_tmp.begin()+i);
            //     posy_tmp.erase(posy_tmp.begin()+i);
            //     posz_tmp.erase(posz_tmp.begin()+i);
            //     if (!hit_correct::least_square_fit(posz_tmp, posx_tmp, kzx, bzx, rmse_zx) ||
            //         !hit_correct::least_square_fit(posz_tmp, posy_tmp, kzy, bzy, rmse_zy))
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            //     double res_zx = abs(posx[i]-kzx*posz[i]-bzx);
            //     double res_zy = abs(posy[i]-kzy*posz[i]-bzy);
            //     if (res_zx > fit_hit_res_cut || res_zy > fit_hit_res_cut)
            //     {
            //         b_hit_res_flag = true;
            //         break;
            //     }
            // }
            // if (b_hit_res_flag)
            // {
            //     continue;
            // }

            // rmse cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx) ||
                !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy))
            {
                continue;
            }
            if (rmse_zx > fit_rmse_cut || rmse_zy > fit_rmse_cut)
            {
                continue;
            }

            // R2 cut
            if (!hit_correct::least_square_fit(posz, posx, kzx, bzx, rmse_zx, R2_zx) ||
                !hit_correct::least_square_fit(posz, posy, kzy, bzy, rmse_zy, R2_zy))
            {
                continue;
            }
            if (R2_zx < fit_R2_cut || R2_zy < fit_R2_cut)
            {
                continue;
            }

            // save res
            double posx_fit = kzx * m_correct->posz[n] + bzx;
            double delta_x = posx_fit - m_correct->posx[n];
            double anglezx = abs(atan(kzx)) / TMath::Pi() * 180;
            int angle_idzx = (int) (anglezx/angle_bin_size);
            if (angle_idzx < 10)
                pos_res_x_hist[angle_idzx]->Fill(delta_x);
            else
                cout << "angle idx x: " << angle_idzx << endl;

            double posy_fit = kzy * m_correct->posz[n] + bzy;
            double delta_y = posy_fit - m_correct->posy[n];
            double anglezy = abs(atan(kzy)) / TMath::Pi() * 180;
            int angle_idzy = (int) (anglezy/angle_bin_size);
            if (angle_idzy < 10)
                pos_res_y_hist[angle_idzy]->Fill(delta_y);
            else
                cout << "angle idx y: " << angle_idzy << endl;
        }
    }

    // calc the sigma of residual
    double pos_res_x[angle_bin_num] = {0};
    double pos_res_y[angle_bin_num] = {0};
    for (int i=0; i<angle_bin_num; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            double max_pos_x = pos_res_x_hist[i]->
                GetBinCenter(pos_res_x_hist[i]->GetMaximumBin());
            // pos_res_x_hist[i]->Fit("gaus", "Q");
            // pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x-0.3, max_pos_x+0.3);
            pos_res_x_hist[i]->Fit("gaus", "Q", "", max_pos_x-2, max_pos_x+2);
            TF1 *fitx = pos_res_x_hist[i]->GetFunction("gaus");
            pos_res_x[i] = fitx->GetParameter(2);
            cout << "x" << i << ": " << pos_res_x[i] << endl;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            double max_pos_y = pos_res_y_hist[i]->
                GetBinCenter(pos_res_y_hist[i]->GetMaximumBin());
            // pos_res_y_hist[i]->Fit("gaus", "Q");
            // pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y-0.3, max_pos_y+0.3);
            pos_res_y_hist[i]->Fit("gaus", "Q", "", max_pos_y-2, max_pos_y+2);
            TF1 *fity = pos_res_y_hist[i]->GetFunction("gaus");
            pos_res_y[i] = fity->GetParameter(2);
            cout << "y" << i << ": " << pos_res_y[i] << endl;
        }
    }

    for (int i=0; i<angle_bin_num; i++)
    {
        if (pos_res_x_hist[i]->GetEntries() > 0)
        {
            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            pos_res_x_hist[i]->Draw();
            pos_res_x_hist[i]->GetFunction("gaus")->Draw("same");
            string pngname1 = m_png_path + "tpc-detector_res_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;
        }

        if (pos_res_y_hist[i]->GetEntries() > 0)
        {
            TCanvas *c2 = new TCanvas("c", "c", 800, 600);
            pos_res_y_hist[i]->Draw();
            pos_res_y_hist[i]->GetFunction("gaus")->Draw("same");
            string pngname2 = m_png_path + "tpc-detector_res_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        }
    }

    double detector_id[angle_bin_num];
    for (int i=0; i<angle_bin_num; i++)
    {
        detector_id[i] = i * angle_bin_size;
    }

    TGraph *pos_res_graphx = new TGraph(angle_bin_num, detector_id, pos_res_x);
    pos_res_graphx->GetXaxis()->SetTitle("angle(degree)");
    pos_res_graphx->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c1 = new TCanvas("c", "c", 800, 600);
    pos_res_graphx->Draw();
    string pngname1 = m_png_path + "tpc-detector_res_x.png";
    list_file::create_path(m_png_path);
    c1->Print(pngname1.data());
    delete c1;
    delete pos_res_graphx;

    TGraph *pos_res_graphy = new TGraph(angle_bin_num, detector_id, pos_res_y);
    pos_res_graphy->GetXaxis()->SetTitle("angle(degree)");
    pos_res_graphy->GetYaxis()->SetTitle("resolution(mm)");
    TCanvas *c2 = new TCanvas("c", "c", 800, 600);
    pos_res_graphy->Draw();
    string pngname2 = m_png_path + "tpc-detector_res_y.png";
    list_file::create_path(m_png_path);
    c2->Print(pngname2.data());
    delete c2;
    delete pos_res_graphy;

    for (int i=0; i<angle_bin_num; i++)
    {
        delete pos_res_x_hist[i];
        delete pos_res_y_hist[i];
    }

    return true;
}
*/

/*

bool info_calc::check_correct_process()
{
    // test code -----------------------------------------------------
    m_correct->set_rmse_cut(2);
    m_correct->set_ndet_num_cut(6);
    m_correct->set_theta_cut(3.14);
    m_correct->set_hit_res_cut(3);
    for (int evt = 0, cnt = 0; evt < 1000 && cnt < 50; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run hit correct of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        int level = m_correct->correct();
        if (level > 0)
        {
            if (level < 10)
            {
                m_correct->print_png(false);
            }
            else
            {
                m_correct->print_png();
            }
            cnt++;
        }
    }
    // ---------------------------------------------------------------

    // for (int evt=0; evt<entries; evt++)
    // {
    //     m_input_tree->GetEntry(evt);
    //     if (evt % 1000 == 0)
    //     {
    //         cout << "Run hit correct of event: " << evt << endl;
    //     }

    //     int layer_hits = 0;
    //     for (int i=0; i<cDET_LAYER; i++)
    //     {
    //         layer_hits += detector_data[i]->sig;
    //     }
    //     if (layer_hits != 8) continue;

    //     m_correct->set_evt_data(detector_data);
    //     m_correct->print_png();
    // }

    return 0;
}

bool info_calc::check_align_var()  // not finished...
{
    // set correct status
    // m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(0.5);
    m_correct->set_ndet_num_cut(6);
    m_correct->set_theta_cut(10. / 180 * 3.1415);
    m_correct->set_hit_res_cut(0.5);

    // show align info of each detector
    TH1F *pos_deltax_hist[cDET_LAYER_MAX];
    TH1F *pos_deltay_hist[cDET_LAYER_MAX];
    TH1F *pos_deltaz_hist[cDET_LAYER_MAX];
    TH1F *pos_rotx_hist[cDET_LAYER_MAX];
    TH1F *pos_roty_hist[cDET_LAYER_MAX];
    TH1F *pos_rotz_hist[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string deltax_name = "deltax" + to_string(i);
        string deltay_name = "deltay" + to_string(i);
        string deltaz_name = "deltaz" + to_string(i);
        string rotx_name = "rotx" + to_string(i);
        string roty_name = "roty" + to_string(i);
        string rotz_name = "rotz" + to_string(i);
        pos_deltax_hist[i] = new TH1F(deltax_name.data(), deltax_name.data(), 100, -4, 4);
        pos_deltay_hist[i] = new TH1F(deltay_name.data(), deltay_name.data(), 100, -4, 4);
        pos_deltaz_hist[i] = new TH1F(deltaz_name.data(), deltaz_name.data(), 100, -1, 1);
        pos_rotx_hist[i] = new TH1F(rotx_name.data(), rotx_name.data(), 100, -0.5, 0.5);
        pos_roty_hist[i] = new TH1F(roty_name.data(), roty_name.data(), 100, -0.5, 0.5);
        pos_rotz_hist[i] = new TH1F(rotz_name.data(), rotz_name.data(), 100, -0.5, 0.5);
    }

    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run align check of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        vector<int> idx_det = m_correct->idx_det;
        vector<double> posx = m_correct->posx;
        vector<double> posy = m_correct->posy;
        vector<double> posz = m_correct->posz;
        double Kx = m_correct->kzx;
        double Bx = m_correct->bzx;
        double Ky = m_correct->kzy;
        double By = m_correct->bzy;

        int N = idx_det.size();  // layer counter of this event

        double x[cDET_LAYER_MAX] = {0}, y[cDET_LAYER_MAX] = {0}, z[cDET_LAYER_MAX] = {0};
        for (int n = 0; n < N; n++)
        {
            int i = idx_det[n];
            x[i] = posx[n];
            y[i] = posy[n];
            z[i] = posz[n];
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete pos_deltax_hist[i];
        delete pos_deltay_hist[i];
        delete pos_deltaz_hist[i];
        delete pos_rotx_hist[i];
        delete pos_roty_hist[i];
        delete pos_rotz_hist[i];
    }

    return true;
}

bool info_calc::stats_hitnums()
{
    TH1D *histx = new TH1D("histx", "histx", 1000, 0, 1000);
    TH1D *histy = new TH1D("histy", "histy", 1000, 0, 1000);
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run hit correct of event: " << evt << endl;
        }

        // int layer_hits = 0;
        // for (int i=0; i<cDET_LAYER; i++)
        // {
        //     layer_hits += detector_data[i]->sig;
        // }
        // if (layer_hits != 8) continue;

        // int hitnumx = 0;
        // int hitnumy = 0;
        // int ndetx = 0;
        // int ndety = 0;
        // for (int i=0; i<cDET_LAYER; i++)
        // {
        //     if (detector_data[i]->sig_x == 1)
        //     {
        //         hitnumx += 1;
        //         hitnumx += detector_data[i]->x_nhits;
        //         ndetx++;
        //     }
        //     if (detector_data[i]->sig_y == 1)
        //     {
        //         hitnumy += 1;
        //         hitnumy += detector_data[i]->y_nhits;
        //         ndety++;
        //     }
        // }
        // if (ndetx > 0)
        //     histx->Fill((hitnumx+.0)/ndetx);
        // if (ndety > 0)
        //     histy->Fill((hitnumy+.0)/ndety);

        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig_x == 1)
            {
                int hitnumx = 0;
                hitnumx += 1;
                // histx->Fill(hitnumx);
                TH1D h("h", "h", 160, 0, 160);
                if (detector_data[i]->x > 0)
                {
                    h.Fill(detector_data[i]->x);
                }
                histx->Fill(h.GetStdDev() * hitnumx);
            }
            if (detector_data[i]->sig_y == 1)
            {
                int hitnumy = 0;
                hitnumy += 1;
                // histy->Fill(hitnumy);
                TH1D h("h", "h", 160, 0, 160);
                if (detector_data[i]->y > 0)
                {
                    h.Fill(detector_data[i]->y);
                }
                histy->Fill(h.GetStdDev() * hitnumy);
            }
        }
    }

    TCanvas *cx = new TCanvas("cx", "cx", 800, 600);
    histx->Draw();
    cx->SetLogy();
    list_file::create_path(m_png_path);
    string pngname = "./png-info/hitnumx.png";
    cx->Print(pngname.data());
    delete cx;
    TCanvas *cy = new TCanvas("cy", "cy", 800, 600);
    histy->Draw();
    cy->SetLogy();
    pngname = "./png-info/hitnumy.png";
    cy->Print(pngname.data());
    delete cy;

    return true;
}

bool info_calc::show_vars_corr()
{
    // settings
    const bool b_corr_angle_stripnum = true;
    // hole is the strip which have no signal
    // but its two neighbor strips have signal
    const bool b_corr_angle_holenum = false;
    const bool b_corr_angle_microTPCangle = false;

    const int angle_bin_num = 6;
    double angle_bin_size = 5;                          // degree
    double angle_max = angle_bin_size * angle_bin_num;  // degree

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(3);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    if (b_corr_angle_stripnum)
    {
        // alignment_new *alignment_tool = new alignment_new();
        alignment_quaternion *alignment_tool = new alignment_quaternion();
        alignment_tool->set_alignment_filename(m_alignment_filename);

        // correlation of angle and hit strip number
        TH1D *h1_stripnum_x[cDET_LAYER_MAX];
        TH1D *h1_stripnum_y[cDET_LAYER_MAX];
        TH2D *corr_angle_stripnum_x[cDET_LAYER_MAX];
        TH2D *corr_angle_stripnum_y[cDET_LAYER_MAX];
        for (int i = 0; i < cDET_LAYER; i++)
        {
            string h1namex = "h1_stripnum_x_" + to_string(i);
            h1_stripnum_x[i] = new TH1D(h1namex.data(), h1namex.data(), 20, 0, 20);
            string h1namey = "h1_stripnum_y_" + to_string(i);
            h1_stripnum_y[i] = new TH1D(h1namey.data(), h1namey.data(), 20, 0, 20);
            string namex = "corr_angle_stripnum_x_" + to_string(i);
            corr_angle_stripnum_x[i] = new TH2D(namex.data(), namex.data(), 30, 0, 30, 20, 0, 20);
            string namey = "corr_angle_stripnum_y_" + to_string(i);
            corr_angle_stripnum_y[i] = new TH2D(namey.data(), namey.data(), 30, 0, 30, 20, 0, 20);
        }

        for (int evt = 0; evt < entries; evt++)
        {
            hit_data_tree->GetEntry(evt);
            if (evt % 10000 == 0)
            {
                cout << "Run corr of angle and strip number of event: " << evt << endl;
            }

            // get data
            vector<double> zx_origin, x_origin, zy_origin, y_origin;
            vector<double> zx, x, zy, y;
            vector<int> layer;
            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig == 1)
                {
                    x_origin.push_back(detector_data[i]->x);
                    zx_origin.push_back(detector_data[i]->z);
                    y_origin.push_back(detector_data[i]->y);
                    zy_origin.push_back(detector_data[i]->z);

                    double xx = detector_data[i]->x;
                    double yy = detector_data[i]->y;
                    double zz = detector_data[i]->z;
                    alignment_tool->local_to_global(xx, yy, zz, i);
                    x.push_back(xx);
                    zx.push_back(zz);
                    y.push_back(yy);
                    zy.push_back(zz);

                    layer.push_back(i);
                }
            }
            if (layer.size() < 3)
            {
                continue;
            }

            // 探测器分辨率
            double sigmax = 0.4 / sqrt(12);
            double sigmay = 0.4 / sqrt(12);

            double kx, bx, ky, by;
            double chi2Dndf_x, chi2Dndf_y;

            // 直线拟合
            if (!alignment_tool->least_square_fit(zx, x, sigmax, kx, bx, chi2Dndf_x)
                || !alignment_tool->least_square_fit(zy, y, sigmay, ky, by, chi2Dndf_y))
            {
                continue;
            }
            alignment_tool->define_track(kx, ky, bx, by);

            // 计算这条直线的对齐权重
            // double weight = 1;
            double weight = sqrt((chi2Dndf_x + chi2Dndf_y) / 2) * 1;
            if (weight > 20)
                continue;

            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig == 1 && i != -1)
                {
                    h1_stripnum_x[i]->Fill(detector_data[i]->strips_num_x);
                    h1_stripnum_y[i]->Fill(detector_data[i]->strips_num_y);
                }
                else if (detector_data[i]->sig == 1 && i == -1)
                {
                    // if (detector_data[i]->x > 230 && detector_data[i]->x < 300 && detector_data[i]->y > 76 && detector_data[i]->y < 128) // encode
                    if (detector_data[i]->x > 200 && detector_data[i]->x < 300 && detector_data[i]->y > 76
                        && detector_data[i]->y < 230)  // direct
                    {
                        h1_stripnum_x[i]->Fill(detector_data[i]->strips_num_x);
                        h1_stripnum_y[i]->Fill(detector_data[i]->strips_num_y);
                    }
                }
            }

            m_correct->set_evt_data(detector_data);
            if (m_correct->correct() != 10)
                continue;

            int N = m_correct->idx_det.size();  // layer counter of this event
            for (int n = 0; n < N; n++)
            {
                double angle_zx = m_correct->theta_zx * rad2deg;
                int stripnum_x = detector_data[m_correct->idx_det[n]]->strips_num_x;
                // h1_stripnum_x[m_correct->idx_det[n]]->Fill(stripnum_x);
                corr_angle_stripnum_x[m_correct->idx_det[n]]->Fill(angle_zx, stripnum_x);

                double angle_zy = m_correct->theta_zy * rad2deg;
                int stripnum_y = detector_data[m_correct->idx_det[n]]->strips_num_y;
                // h1_stripnum_y[m_correct->idx_det[n]]->Fill(stripnum_y);
                corr_angle_stripnum_y[m_correct->idx_det[n]]->Fill(angle_zy, stripnum_y);
            }
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            cout << i << "x mean cluster size: " << h1_stripnum_x[i]->GetMean() << endl;
            cout << i << "y mean cluster size: " << h1_stripnum_y[i]->GetMean() << endl;
            cout << i << " mean cluster size: " << (h1_stripnum_x[i]->GetMean() + h1_stripnum_y[i]->GetMean()) / 2
                 << endl;

            TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
            PlotTool plot;
            plot.SetStyle();
            plot.SetPrelimStyle();
            plot.NameAxis(h1_stripnum_x[i], "cluster size", "count");
            plot.FormatData(h1_stripnum_x[i]);
            c1->SetLeftMargin(0.2);
            c1->SetBottomMargin(0.2);
            h1_stripnum_x[i]->Draw();
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "h1_stripnum_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;

            TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
            h1_stripnum_y[i]->Draw();
            string pngname2 = m_png_path + "h1_stripnum_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
            corr_angle_stripnum_x[i]->Draw("colztext");
            list_file::create_path(m_png_path);
            string pngname1 = m_png_path + "corr_angle_stripnum_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;

            TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
            corr_angle_stripnum_y[i]->Draw("colztext");
            string pngname2 = m_png_path + "corr_angle_stripnum_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        }

        double a_angle_zx[30], a_stripnum_zx[30];
        double a_angle_zy[30], a_stripnum_zy[30];
        for (int i = 0; i < cDET_LAYER; i++)
        {
            for (int j = 0; j < 30; j++)
            {
                a_angle_zx[j] = j;
                corr_angle_stripnum_x[i]->GetXaxis()->SetRange(j + 1, j + 1);
                a_stripnum_zx[j] = corr_angle_stripnum_x[i]->GetMean(2);

                a_angle_zy[j] = j;
                corr_angle_stripnum_y[i]->GetXaxis()->SetRange(j + 1, j + 1);
                a_stripnum_zy[j] = corr_angle_stripnum_y[i]->GetMean(2);
            }
            TGraph *g1 = new TGraph(30, a_angle_zx, a_stripnum_zx);
            TGraph *g2 = new TGraph(30, a_angle_zy, a_stripnum_zy);
            g1->SetLineColor(kRed);
            g2->SetLineColor(kBlue);
            TCanvas *c = new TCanvas("c", "c", 800, 600);
            g1->Draw("APL*");
            g2->Draw("samePL*");
            list_file::create_path(m_png_path);
            string pngname = m_png_path + "corr_angle_stripnum_line_" + to_string(i) + ".png";
            c->Print(pngname.data());
            delete c, g1, g2;
        }
    }

    if (b_corr_angle_holenum)
    {
        // correlation of angle and hole number
        TH2D *corr_angle_holenum_x = new TH2D("corr_angle_holenum_x", "corr_angle_holenum_x", 30, 0, 30, 20, 0, 20);
        TH2D *corr_angle_holenum_y = new TH2D("corr_angle_holenum_y", "corr_angle_holenum_y", 30, 0, 30, 20, 0, 20);

        for (int evt = 0; evt < entries; evt++)
        {
            hit_data_tree->GetEntry(evt);
            if (evt % 1000 == 0)
            {
                cout << "Run corr of angle and hole number of event: " << evt << endl;
            }

            m_correct->set_evt_data(detector_data);
            if (m_correct->correct() != 10)
                continue;

            double cluster_size_low, cluster_size_upp;
            int N = m_correct->idx_det.size();  // layer counter of this event
            for (int n = 0; n < N; n++)
            {
                // x
                cluster_size_low = 10000, cluster_size_upp = -10000;
                double angle_zx = m_correct->theta_zx * rad2deg;
                int stripnum_x = detector_data[m_correct->idx_det[n]]->strips_num_x;
                for (int i = 0; i < stripnum_x; i++)
                {
                    double chn_pos = detector_data[m_correct->idx_det[n]]->hit_chn_x[i];
                    if (chn_pos < cluster_size_low)
                    {
                        cluster_size_low = chn_pos;
                    }
                    if (chn_pos > cluster_size_upp)
                    {
                        cluster_size_upp = chn_pos;
                    }
                }
                double cluster_num_x = int((cluster_size_upp - cluster_size_low) / cSTRIP_WIDTH + 0.1) + 1;
                int holenum_x = cluster_num_x - stripnum_x;
                corr_angle_holenum_x->Fill(angle_zx, holenum_x);

                // y
                cluster_size_low = 10000, cluster_size_upp = -10000;
                double angle_zy = m_correct->theta_zy * rad2deg;
                int stripnum_y = detector_data[m_correct->idx_det[n]]->strips_num_y;
                for (int i = 0; i < stripnum_y; i++)
                {
                    double chn_pos = detector_data[m_correct->idx_det[n]]->hit_chn_y[i];
                    if (chn_pos < cluster_size_low)
                    {
                        cluster_size_low = chn_pos;
                    }
                    if (chn_pos > cluster_size_upp)
                    {
                        cluster_size_upp = chn_pos;
                    }
                }
                double cluster_num_y = int((cluster_size_upp - cluster_size_low) / cSTRIP_WIDTH + 0.1) + 1;
                int holenum_y = cluster_num_y - stripnum_y;
                corr_angle_holenum_y->Fill(angle_zy, holenum_y);
            }
        }

        TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
        corr_angle_holenum_x->Draw("colztext");
        string pngname1 = m_png_path + "corr_angle_holenum_x.png";
        c1->Print(pngname1.data());
        delete c1;

        TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
        corr_angle_holenum_y->Draw("colztext");
        string pngname2 = m_png_path + "corr_angle_holenum_y.png";
        c2->Print(pngname2.data());
        delete c2;
    }

    if (b_corr_angle_microTPCangle)
    {
	    */
        /*
        // 100MHz 180ns 550V 688V
        const int angle_bin_num = 6;
        double angle_bin_size = 5; // degree
        double angle_max = angle_bin_size * angle_bin_num; // degree

        const double c_drift_time_zero = 1372.94; // ns
        const double c_drift_time_zx[angle_bin_num] = {
            55.2219, 75.9757, 77.2623, 88.7353, 98.144, 100.616
        };
        const double c_drift_time_zy[angle_bin_num] = {
            61.1912, 89.1183, 90.5915, 82.7226, 89.4878, 89.0816
        };
        const double c_drift_time_theory = 106.5; // ns
        */
/*
        // 100MHz 180ns 550V 600V
        const int angle_bin_num = 6;
        double angle_bin_size = 5;                          // degree
        double angle_max = angle_bin_size * angle_bin_num;  // degree

        const double c_drift_time_zero = 1580;  // ns
        const double c_drift_time_zx[angle_bin_num] = {
            // 55.2219, 75.9757, 77.2623, 88.7353, 98.144, 100.616
        };
        const double c_drift_time_zy[angle_bin_num] = {
            // 61.1912, 89.1183, 90.5915, 82.7226, 89.4878, 89.0816
        };
        const double c_drift_time_theory = 570.457;  // ns

        // 横轴是stripid，纵轴是信号时间
        // 同时对方向进行限定，方法是以信号最早的为0点，以strip条数多的方向为正向
        TH2D *corr_angle_microTPCangle_zx[angle_bin_num];
        TH2D *corr_angle_microTPCangle_zy[angle_bin_num];
        for (int i = 0; i < angle_bin_num; i++)
        {
            string namex = "microTPCangle_zx_" + to_string(i);
            corr_angle_microTPCangle_zx[i] = new TH2D(namex.data(), namex.data(), 20, -10, 10, 100, -600, 600);
            string namey = "microTPCangle_zy_" + to_string(i);
            corr_angle_microTPCangle_zy[i] = new TH2D(namey.data(), namey.data(), 20, -10, 10, 100, -600, 600);
        }

        m_correct->set_use_alignment(false);
        m_correct->set_rmse_cut(10000);
        m_correct->set_ndet_num_cut(4);
        m_correct->set_theta_cut(90. / 180 * 3.1415);
        m_correct->set_hit_res_cut(10000);

        for (int evt = 0; evt < entries; evt++)
        {
            hit_data_tree->GetEntry(evt);
            if (evt % 1000 == 0)
            {
                cout << "Run res of event: " << evt << endl;
            }

            m_correct->set_evt_data(detector_data);
            if (m_correct->correct() != 10)
                continue;

            int angle_idzx = (int)(m_correct->theta_zx * rad2deg / angle_bin_size);
            int angle_idzy = (int)(m_correct->theta_zy * rad2deg / angle_bin_size);
            if (angle_idzx >= angle_bin_num || angle_idzy >= angle_bin_num)
            {
                continue;
            }

            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig == 1 && detector_data[i]->x > 0 && detector_data[i]->y > 0)
                {
                    // zx, use hough trans
                    vector<double> pos_x, dis_x, time_x, amp_x;
                    for (int j = 0; j < detector_data[i]->strips_num_x; j++)
                    {
                        double pos = detector_data[i]->hit_chn_x[j];
                        double dis = (detector_data[i]->hit_time_x[j] - c_drift_time_zero) * 5. / c_drift_time_theory
                                   // * 5. / c_drift_time_zx[angle_idzx]
                                   + detector_data[i]->z;
                        double d = abs(m_correct->kzx * dis - pos + m_correct->bzx)
                                 / sqrt(m_correct->kzx * m_correct->kzx + 1);
                        if (d < 2)
                        {
                            pos_x.push_back(pos);
                            dis_x.push_back(dis);
                            time_x.push_back(detector_data[i]->hit_time_x[j]);
                            amp_x.push_back(detector_data[i]->hit_amp_x[j]);
                        }
                    }
                    // check hit number
                    if (pos_x.size() < 2)
                    {
                        continue;
                    }
                    // draw corr
                    int strips_num_x = pos_x.size();
                    int time_earliest_id_x;
                    double time_earliest_chn_x;
                    double time_earliest_time_x = 10000;
                    for (int j = 0; j < strips_num_x; j++)
                    {
                        if (time_x[j] < time_earliest_time_x)
                        {
                            time_earliest_time_x = time_x[j];
                            time_earliest_chn_x = pos_x[j];
                            time_earliest_id_x = j;
                        }
                    }
                    if ((time_earliest_chn_x - pos_x[0]) <= (pos_x[strips_num_x - 1] - pos_x[0]) / 2.)
                    // if ((time_earliest_id_x + 1) <= (strips_num_x + 1)/2.)
                    {
                        for (int j = 0; j < strips_num_x; j++)
                        {
                            double pos = pos_x[j] - time_earliest_chn_x;
                            int chn = pos >= 0 ? (pos + 0.1) / 0.4 : (pos - 0.1) / 0.4;
                            double time = time_x[j] - c_drift_time_zero;
                            // time *= 5./c_drift_time_zx[angle_idzx];
                            corr_angle_microTPCangle_zx[angle_idzx]->Fill(chn, time);
                        }
                    }
                    else
                    {
                        for (int j = 0; j < strips_num_x; j++)
                        {
                            double pos = time_earliest_chn_x - pos_x[j];
                            int chn = pos >= 0 ? (pos + 0.1) / 0.4 : (pos - 0.1) / 0.4;
                            double time = time_x[j] - c_drift_time_zero;
                            // time *= 5./c_drift_time_zx[angle_idzx];
                            corr_angle_microTPCangle_zx[angle_idzx]->Fill(chn, time);
                        }
                    }

                    /*
                    // zx, fix amp, time, holenum
                    // check amp
                    vector<double> pos_x, time_x, amp_x;
                    for (int j=0; j<detector_data[i]->strips_num_x; j++)
                    {
                        pos_x.push_back(detector_data[i]->hit_chn_x[j]);
                        time_x.push_back(detector_data[i]->hit_time_x[j]);
                        amp_x.push_back(detector_data[i]->hit_amp_x[j]);
                    }
                    for (int j=0; j<pos_x.size();)
                    {
                        if (j-1>=0 && amp_x[j]<amp_x[j-1]*0.15)
                        {
                            pos_x.erase(pos_x.begin()+j);
                            time_x.erase(time_x.begin()+j);
                            amp_x.erase(amp_x.begin()+j);
                            continue;
                        }
                        if (j+1<=pos_x.size()-1 && amp_x[j]<amp_x[j+1]*0.15)
                        {
                            pos_x.erase(pos_x.begin()+j);
                            time_x.erase(time_x.begin()+j);
                            amp_x.erase(amp_x.begin()+j);
                            continue;
                        }
                        j++;
                    }
                    // check time: all time should be in range of +-c_drift_time_zx
                    for (int j=0; j<pos_x.size();)
                    {
                        if ((time_x[j]-c_drift_time_zero) > c_drift_time_zx[angle_idzx]
                            || (time_x[j]-c_drift_time_zero) < -c_drift_time_zx[angle_idzx])
                        {
                            pos_x.erase(pos_x.begin()+j);
                            time_x.erase(time_x.begin()+j);
                            amp_x.erase(amp_x.begin()+j);
                            continue;
                        }
                        j++;
                    }
                    // check holenum
                    vector<vector<double>> split_pos_x, split_time_x, split_amp_x;
                    vector<double> tmp_pos_x, tmp_time_x, tmp_amp_x;
                    tmp_pos_x.push_back(pos_x[0]);
                    tmp_time_x.push_back(time_x[0]);
                    tmp_amp_x.push_back(amp_x[0]);
                    double last_chn = pos_x[0];
                    for (int j=1; j<pos_x.size(); j++)
                    {
                        double pos = pos_x[j];
                        if (int((pos-last_chn+0.1)/0.4) < 3)
                        {
                            tmp_pos_x.push_back(pos_x[j]);
                            tmp_time_x.push_back(time_x[j]);
                            tmp_amp_x.push_back(amp_x[j]);
                            last_chn = pos_x[j];
                        }
                        else
                        {
                            split_pos_x.push_back(tmp_pos_x);
                            split_time_x.push_back(tmp_time_x);
                            split_amp_x.push_back(tmp_amp_x);
                            tmp_pos_x.clear();
                            tmp_time_x.clear();
                            tmp_amp_x.clear();
                            tmp_pos_x.push_back(pos_x[j]);
                            tmp_time_x.push_back(time_x[j]);
                            tmp_amp_x.push_back(amp_x[j]);
                            last_chn = pos_x[j];
                        }
                    }
                    split_pos_x.push_back(tmp_pos_x);
                    split_time_x.push_back(tmp_time_x);
                    split_amp_x.push_back(tmp_amp_x);
                    if (split_pos_x.size() > 1)
                    {
                        int max_size=0, max_size_idx;
                        for (int j=0; j<split_pos_x.size(); j++)
                        {
                            double size = split_pos_x[j].size();
                            if (size > max_size)
                            {
                                max_size = size;
                                max_size_idx = j;
                            }
                        }
                        pos_x = split_pos_x[max_size_idx];
                        time_x = split_time_x[max_size_idx];
                        amp_x = split_amp_x[max_size_idx];
                    }
                    // check hit number
                    if (pos_x.size() < 2)
                    {
                        continue;
                    }
                    // draw corr
                    int strips_num_x = pos_x.size();
                    int time_earliest_id_x;
                    double time_earliest_chn_x;
                    double time_earliest_time_x = 10000;
                    for (int j=0; j<strips_num_x; j++)
                    {
                        if (time_x[j] < time_earliest_time_x)
                        {
                            time_earliest_time_x = time_x[j];
                            time_earliest_chn_x = pos_x[j];
                            time_earliest_id_x = j;
                        }
                    }
                    if ((time_earliest_chn_x - pos_x[0]) <=
                        (pos_x[strips_num_x-1] - pos_x[0]) / 2.)
                    // if ((time_earliest_id_x + 1) <= (strips_num_x + 1)/2.)
                    {
                        for (int j=0; j<strips_num_x; j++)
                        {
                            double pos = pos_x[j] - time_earliest_chn_x;
                            int chn = pos>=0 ? (pos+0.1)/0.4 : (pos-0.1)/0.4;
                            double time = time_x[j] - c_drift_time_zero;
                            // time *= 5./c_drift_time_zx[angle_idzx];
                            corr_angle_microTPCangle_zx[angle_idzx]->Fill(chn, time);
                        }
                    }
                    else
                    {
                        for (int j=0; j<strips_num_x; j++)
                        {
                            double pos = time_earliest_chn_x - pos_x[j];
                            int chn = pos>=0 ? (pos+0.1)/0.4 : (pos-0.1)/0.4;
                            double time = time_x[j] - c_drift_time_zero;
                            // time *= 5./c_drift_time_zx[angle_idzx];
                            corr_angle_microTPCangle_zx[angle_idzx]->Fill(chn, time);
                        }
                    }
                    */

                    /*
                    // zx, no fix
                    int strips_num_x = detector_data[i]->strips_num_x;
                    int time_earliest_id_x;
                    double time_earliest_chn_x;
                    double time_earliest_time_x = 10000;
                    for (int j=0; j<strips_num_x; j++)
                    {
                        if (detector_data[i]->hit_time_x[j] < time_earliest_time_x
                            && detector_data[i]->hit_time_x[j] > (c_drift_time_zero-100))
                        {
                            time_earliest_time_x = detector_data[i]->hit_time_x[j];
                            time_earliest_chn_x = detector_data[i]->hit_chn_x[j];
                            time_earliest_id_x = j;
                        }
                    }
                    if ((time_earliest_chn_x - detector_data[i]->hit_chn_x[0]) <=
                        (detector_data[i]->hit_chn_x[strips_num_x-1]
                            - detector_data[i]->hit_chn_x[0]) / 2.)
                    // if ((time_earliest_id_x + 1) <= (strips_num_x + 1)/2.)
                    {
                        for (int j=0; j<strips_num_x; j++)
                        {
                            double pos = detector_data[i]->hit_chn_x[j] - time_earliest_chn_x;
                            int chn = pos>=0 ? (pos+0.1)/0.4 : (pos-0.1)/0.4;
                            double time = detector_data[i]->hit_time_x[j] - c_drift_time_zero;
                            // time *= 5./c_drift_time_zx[angle_idzx];
                            corr_angle_microTPCangle_zx[angle_idzx]->Fill(chn, time);
                        }
                    }
                    else
                    {
                        for (int j=0; j<strips_num_x; j++)
                        {
                            double pos = time_earliest_chn_x - detector_data[i]->hit_chn_x[j];
                            int chn = pos>=0 ? (pos+0.1)/0.4 : (pos-0.1)/0.4;
                            double time = detector_data[i]->hit_time_x[j] - c_drift_time_zero;
                            // time *= 5./c_drift_time_zx[angle_idzx];
                            corr_angle_microTPCangle_zx[angle_idzx]->Fill(chn, time);
                        }
                    }
                    */
/*
                    // zy
                    int strips_num_y = detector_data[i]->strips_num_y;
                    int time_earliest_id_y;
                    double time_earliest_chn_y;
                    double time_earliest_time_y = 10000;
                    for (int j = 0; j < strips_num_y; j++)
                    {
                        if (detector_data[i]->hit_time_y[j] < time_earliest_time_y)
                        {
                            time_earliest_time_y = detector_data[i]->hit_time_y[j];
                            time_earliest_chn_y = detector_data[i]->hit_chn_y[j];
                            time_earliest_id_y = j;
                        }
                    }
                    if ((time_earliest_chn_y - detector_data[i]->hit_chn_y[0])
                        <= (detector_data[i]->hit_chn_y[strips_num_y - 1] - detector_data[i]->hit_chn_y[0]) / 2.)
                    {
                        for (int j = 0; j < strips_num_y; j++)
                        {
                            double pos = detector_data[i]->hit_chn_y[j] - time_earliest_chn_y;
                            int chn = pos >= 0 ? (pos + 0.1) / 0.4 : (pos - 0.1) / 0.4;
                            double time = detector_data[i]->hit_time_y[j] - c_drift_time_zero;
                            corr_angle_microTPCangle_zy[angle_idzy]->Fill(chn, time);
                        }
                    }
                    else
                    {
                        for (int j = 0; j < strips_num_y; j++)
                        {
                            double pos = time_earliest_chn_y - detector_data[i]->hit_chn_y[j];
                            int chn = pos >= 0 ? (pos + 0.1) / 0.4 : (pos - 0.1) / 0.4;
                            double time = detector_data[i]->hit_time_y[j] - c_drift_time_zero;
                            corr_angle_microTPCangle_zy[angle_idzy]->Fill(chn, time);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < angle_bin_num; i++)
        {
            if (corr_angle_microTPCangle_zx[i]->GetEntries() > 0)
            {
                TF1 line_drift_time_zx_upp("line_drift_time_zx_upp", to_string(c_drift_time_zx[i] / 2).data(), -10, 10);
                TF1 line_drift_time_zx_low("line_drift_time_zx_low", to_string(-c_drift_time_zx[i] / 2).data(), -10,
                                           10);
                line_drift_time_zx_upp.SetLineColor(2);
                line_drift_time_zx_low.SetLineColor(2);
                TF1 line_drift_time_theory_upp("line_drift_time_theory_upp", to_string(c_drift_time_theory / 2).data(),
                                               -10, 10);
                TF1 line_drift_time_theory_low("line_drift_time_theory_low", to_string(-c_drift_time_theory / 2).data(),
                                               -10, 10);
                line_drift_time_theory_upp.SetLineColor(3);
                line_drift_time_theory_low.SetLineColor(3);

                TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
                corr_angle_microTPCangle_zx[i]->Draw("colz");
                line_drift_time_zx_upp.Draw("same");
                line_drift_time_zx_low.Draw("same");
                line_drift_time_theory_upp.Draw("same");
                line_drift_time_theory_low.Draw("same");
                string name = m_png_path + "angle_microTPCangle_x_" + to_string(i) + ".png";
                c1->Print(name.data());
                delete c1;
            }
            if (corr_angle_microTPCangle_zy[i]->GetEntries() > 0)
            {
                TF1 line_drift_time_zy_upp("line_drift_time_zy_upp", to_string(c_drift_time_zy[i] / 2).data(), -10, 10);
                TF1 line_drift_time_zy_low("line_drift_time_zy_low", to_string(-c_drift_time_zy[i] / 2).data(), -10,
                                           10);
                line_drift_time_zy_upp.SetLineColor(2);
                line_drift_time_zy_low.SetLineColor(2);
                TF1 line_drift_time_theory_upp("line_drift_time_theory_upp", to_string(c_drift_time_theory / 2).data(),
                                               -10, 10);
                TF1 line_drift_time_theory_low("line_drift_time_theory_low", to_string(-c_drift_time_theory / 2).data(),
                                               -10, 10);
                line_drift_time_theory_upp.SetLineColor(3);
                line_drift_time_theory_low.SetLineColor(3);

                TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
                corr_angle_microTPCangle_zy[i]->Draw("colz");
                line_drift_time_zy_upp.Draw("same");
                line_drift_time_zy_low.Draw("same");
                line_drift_time_theory_upp.Draw("same");
                line_drift_time_theory_low.Draw("same");
                string name = m_png_path + "angle_microTPCangle_y_" + to_string(i) + ".png";
                c1->Print(name.data());
                delete c1;
            }
        }

        TFile *file = new TFile("test3.root", "recreate");
        for (int i = 0; i < angle_bin_num; i++)
        {
            if (corr_angle_microTPCangle_zx[i]->GetEntries() > 0)
            {
                corr_angle_microTPCangle_zx[i]->Write();
            }
            if (corr_angle_microTPCangle_zy[i]->GetEntries() > 0)
            {
                corr_angle_microTPCangle_zy[i]->Write();
            }
        }
        delete file;
    }

    return 0;
}

bool info_calc::check_micro_TPC()
{
    // 100MHz 180ns 560V 610V
    const int angle_bin_num = 6;
    double angle_bin_size = 5;                          // degree
    double angle_max = angle_bin_size * angle_bin_num;  // degree

    const double c_drift_time_zero = 1580;  // ns
    const double c_drift_time_zx[angle_bin_num] = {
        // 55.2219, 75.9757, 77.2623, 88.7353, 98.144, 100.616
    };
    const double c_drift_time_zy[angle_bin_num] = {
        // 61.1912, 89.1183, 90.5915, 82.7226, 89.4878, 89.0816
    };
    const double c_drift_time_theory = 570.457;  // ns

    // modified drift time parameters
    const double c_drift_time_zero_list[cDET_LAYER_MAX] = {// 1579.85, 1578.13, 1560.38, 1587.47
                                                           1590.05, 1567.55, 1521.85, 1594.91};
    const double c_drift_time_theory_list[cDET_LAYER_MAX] = {570.457, 570.457, 570.457, 570.457};

    // show res info of each detector
    TH1F *tpc_angle_x_hist[angle_bin_num];
    TH1F *tpc_angle_y_hist[angle_bin_num];
    for (int i = 0; i < angle_bin_num; i++)
    {
        string namex = "x" + to_string(i);
        string namey = "y" + to_string(i);
        tpc_angle_x_hist[i] = new TH1F(namex.data(), namex.data(), 100, -90, 90);  // deg
        tpc_angle_y_hist[i] = new TH1F(namey.data(), namey.data(), 100, -90, 90);
    }

    m_correct->set_use_alignment(false);
    m_correct->set_rmse_cut(10000);
    m_correct->set_ndet_num_cut(cDET_LAYER);
    m_correct->set_theta_cut(90. / 180 * 3.1415);
    m_correct->set_hit_res_cut(10000);

    double fit_hit_res_cut = 12.0;
    double fit_rmse_cut = 0.5;
    double fit_R2_cut = 0.95;
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 1000 == 0)
        {
            cout << "Run res of event: " << evt << endl;
        }

        m_correct->set_evt_data(detector_data);
        if (m_correct->correct() != 10)
            continue;

        // micro-TPC
        vector<double> x, y, z;
        vector<int> idx;

        int angle_idzx = (int)(m_correct->theta_zx * rad2deg / angle_bin_size);
        int angle_idzy = (int)(m_correct->theta_zy * rad2deg / angle_bin_size);
        if (angle_idzx >= angle_bin_num || angle_idzy >= angle_bin_num)
        {
            continue;
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1 && detector_data[i]->x > 0 && detector_data[i]->y > 0)
            {
                // zx, use hough trans
                vector<double> pos_x, dis_x, time_x, amp_x;
                for (int j = 0; j < detector_data[i]->strips_num_x; j++)
                {
                    double pos = detector_data[i]->hit_chn_x[j];
                    double dis =
                        (detector_data[i]->hit_time_x[j] - c_drift_time_zero_list[i]) * 5. / c_drift_time_theory
                        + detector_data[i]->z;
                    double d =
                        abs(m_correct->kzx * dis - pos + m_correct->bzx) / sqrt(m_correct->kzx * m_correct->kzx + 1);
                    if (d < 2)  // 就像是知道答案找过程似的
                        if (true)
                        {
                            pos_x.push_back(pos);
                            dis_x.push_back(dis - detector_data[i]->z);
                            time_x.push_back(detector_data[i]->hit_time_x[j]);
                            amp_x.push_back(detector_data[i]->hit_amp_x[j]);
                        }
                }
                // check hit number
                if (pos_x.size() > 3)
                {
                    TH2D *h2 = new TH2D("h2", "h2", 2000, 0, 200, 400, -20, 20);
                    for (int j = 0; j < pos_x.size(); j++)
                    {
                        h2->Fill(pos_x[j], dis_x[j], amp_x[j]);
                    }
                    h2->Fit("pol1", "Q");
                    TF1 *fit = h2->GetFunction("pol1");

                    // TGraph *g1 = new TGraph(pos_x.size(), &pos_x[0], &dis_x[0]);
                    // g1->Fit("pol1", "Q");
                    // TF1 *fit = g1->GetFunction("pol1");

                    if (fit)
                    {
                        double slope_nt = fit->GetParameter(1);
                        double const_nt = fit->GetParameter(0);
                        double chn_rec = -const_nt / slope_nt;

                        double error = chn_rec - detector_data[i]->x;
                        if (abs(error) < 2)
                        {
                            x.push_back(chn_rec);
                        }
                        else
                        {
                            x.push_back(-detector_data[i]->x);
                        }

                        double angle_track = atan(m_correct->kzx) * rad2deg;
                        double angle_tpc = atan(1. / slope_nt) * rad2deg;
                        tpc_angle_x_hist[angle_idzx]->Fill(angle_tpc - angle_track);
                    }
                    else
                    {
                        x.push_back(-detector_data[i]->x);

                        for (int j = 0; j < pos_x.size(); j++)
                        {
                            double dis = (time_x[j] - c_drift_time_zero_list[i]) * 5. / c_drift_time_theory;
                            // * 5. / c_drift_time_zx[angle_idzx];
                            cout << " " << pos_x[j] << " " << dis;
                        }
                        cout << endl;
                    }

                    delete h2;
                    // delete g1;
                }
                else
                {
                    x.push_back(-detector_data[i]->x);
                }

                // zy, use hough trans
                vector<double> pos_y, dis_y, time_y, amp_y;
                for (int j = 0; j < detector_data[i]->strips_num_y; j++)
                {
                    double pos = detector_data[i]->hit_chn_y[j];
                    double dis = (detector_data[i]->hit_time_y[j] - c_drift_time_zero) * 5. / c_drift_time_theory
                               + detector_data[i]->z;
                    // double d = abs(m_correct->kzy*dis-pos+m_correct->bzy)/sqrt(m_correct->kzy*m_correct->kzy+1);
                    // if (d < 2)
                    if (true)
                    {
                        pos_y.push_back(pos);
                        dis_y.push_back(dis);
                        time_y.push_back(detector_data[i]->hit_time_y[j]);
                        amp_y.push_back(detector_data[i]->hit_amp_y[j]);
                    }
                }
                // check hit number
                if (pos_y.size() > 1)
                // if (false)
                {
                    TGraph *g1 = new TGraph(pos_y.size(), &pos_y[0], &dis_y[0]);
                    g1->Fit("pol1", "Q");
                    TF1 *fit = g1->GetFunction("pol1");
                    if (fit)
                    {
                        double slope_nt = fit->GetParameter(1);
                        double const_nt = fit->GetParameter(0);
                        double chn_rec = -const_nt / slope_nt;

                        double error = chn_rec - detector_data[i]->y;
                        if (abs(error) < 2)
                        {
                            y.push_back(chn_rec);
                        }
                        else
                        {
                            y.push_back(-detector_data[i]->y);
                        }

                        double angle_track = atan(m_correct->kzy) * rad2deg;
                        double angle_tpc = atan(1. / slope_nt) * rad2deg;
                        tpc_angle_y_hist[angle_idzy]->Fill(angle_tpc - angle_track);
                    }
                    else
                    {
                        y.push_back(-detector_data[i]->y);

                        for (int j = 0; j < pos_y.size(); j++)
                        {
                            double dis = (time_y[j] - c_drift_time_zero_list[i]) * 5. / c_drift_time_theory;
                            // * 5. / c_drift_time_zy[angle_idzy];
                            cout << " " << pos_y[j] << " " << dis;
                        }
                        cout << endl;
                    }

                    delete g1;
                }
                else
                {
                    y.push_back(detector_data[i]->y);
                }

                z.push_back(detector_data[i]->z);
                idx.push_back(i);
            }
        }
    }

    for (int i = 0; i < angle_bin_num; i++)
    {
        if (tpc_angle_x_hist[i]->GetEntries() > 0)
        {
            TCanvas *c1 = new TCanvas("c", "c", 800, 600);
            tpc_angle_x_hist[i]->Draw();
            string pngname1 = m_png_path + "tpc_angle_x_" + to_string(i) + ".png";
            c1->Print(pngname1.data());
            delete c1;
        }

        if (tpc_angle_y_hist[i]->GetEntries() > 0)
        {
            TCanvas *c2 = new TCanvas("c", "c", 800, 600);
            tpc_angle_y_hist[i]->Draw();
            string pngname2 = m_png_path + "tpc_angle_y_" + to_string(i) + ".png";
            c2->Print(pngname2.data());
            delete c2;
        }
    }

    for (int i = 0; i < angle_bin_num; i++)
    {
        delete tpc_angle_x_hist[i];
        delete tpc_angle_y_hist[i];
    }

    return true;
}

bool info_calc::tomography()
{
    // string det_target = "sig";
    string det_target = "bkg";

    // settings
    int binNum = 40;
    int phiBinNum = 40, thetaBinNum = 40;
    double phimin = -30, phimax = 30;
    double thetamin = 30, thetamax = 90;

    // double det_rotx = 1.0*deg2rad;
    double det_rotx = 0.0 * deg2rad;
    double det_roty = 30 * deg2rad;
    if (det_target == "sig")
    {
        det_rotx = -det_rotx;
        det_roty = -det_roty;
    }
    else if (det_target == "bkg")
    {
        det_rotx = det_rotx;
        det_roty = det_roty;
    }

    TH2D *hist_bkg = new TH2D("hist_bkg", "hist_bkg", phiBinNum, phimin, phimax, thetaBinNum, thetamin, thetamax);

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run tomography of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig_x != 0)
            {
                x[i] = detector_data[i]->x;
                z[i] = detector_data[i]->z;

                if (i == 0 && (x[i] < 50 || x[i] > 350))
                {
                    x[i] = -1000;
                }
            }
            else
            {
                x[i] = -1000;
            }
            if (detector_data[i]->sig_y != 0)
            {
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;
            }
            else
            {
                y[i] = -1000;
            }
        }

        int ref_cnt_x = 0;
        double ref_zxx[cDET_LAYER_MAX] = {0};
        double ref_zxz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (x[j] != -1000)
            {
                ref_zxx[ref_cnt_x] = x[j];
                ref_zxz[ref_cnt_x] = z[j];
                ref_cnt_x++;
            }
        }
        int ref_cnt_y = 0;
        double ref_zyy[cDET_LAYER_MAX] = {0};
        double ref_zyz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (y[j] != -1000)
            {
                ref_zyy[ref_cnt_y] = y[j];
                ref_zyz[ref_cnt_y] = z[j];
                ref_cnt_y++;
            }
        }

        if (ref_cnt_x >= 3 && ref_cnt_y >= 3)
        {
            double kzx, bzx, rmse_zx, R2_zx;
            hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
            double kzy, bzy, rmse_zy, R2_zy;
            hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
            if (rmse_zx < 5 && rmse_zx < 5)
            {
                TVector3 dirc(kzx, kzy, 1);
                dirc.RotateX(det_rotx);
                dirc.RotateY(det_roty);

                double theta = 90 - dirc.Theta() * rad2deg;

                // correct phi direction
                double phi = (dirc.Phi() * rad2deg);
                if (det_roty >= 0)
                {
                    phi = -(dirc.Phi() * rad2deg);
                }
                if (det_roty < 0 && phi > 0)
                {
                    phi -= 180;
                }
                else if (det_roty < 0 && phi < 0)
                {
                    phi += 180;
                }

                // if (theta > 0)
                // {
                //     continue;
                // }
                // else if (theta <= 0)
                // {
                //     theta = -theta;
                //     phi = -phi;
                // }

                // cout << "phi: " << phi << " theta: " << theta << endl;

                if (phi > phimin && phi < phimax && theta > thetamin && theta < thetamax)
                {
                    hist_bkg->Fill(phi, theta);
                }
            }
        }
    }

    // for(int x=1; x<hist_bkg->GetNbinsX()+1; x++)
    // {
    //     for(int y=1; y<hist_bkg->GetNbinsY()+1; y++)
    //     {
    //         double z = hist_bkg->GetBinContent(x, y);
    //         double theta = 90 - hist_bkg->GetYaxis()->GetBinCenter(y);
    //         z = z * 1. / (cos(theta)*cos(theta));
    //         hist_bkg->SetBinContent(x, y, z);
    //     }
    // }

    TCanvas *c1 = new TCanvas("c", "c", 800, 600);
    hist_bkg->Draw("colz");
    list_file::create_path(m_png_path);
    string pngname = m_png_path + "bkghist.png";
    c1->Print(pngname.data());
    delete c1;

    string filename = "bkg.root";
    // string filename = det_target + ".root";
    TFile *f = new TFile(filename.data(), "recreate");
    hist_bkg->Write();
    delete f;

    return true;
}

bool info_calc::tomography_2()  // 平面投影成像
{
    string det_target = "sig";
    // string det_target = "bkg";

    // settings
    int binNum = 50;
    double xmin = -300, xmax = 300;
    double ymin = -300, ymax = 300;

    // double det_rotx = 1.0*deg2rad;
    double det_rotx = 0.0 * deg2rad;
    double det_roty = 0 * deg2rad;
    if (det_target == "sig")
    {
        det_rotx = -det_rotx;
        det_roty = -det_roty;
    }
    else if (det_target == "bkg")
    {
        det_rotx = det_rotx;
        det_roty = det_roty;
    }

    TH2D *hist_bkg = new TH2D("hist_bkg", "hist_bkg", binNum, xmin, xmax, binNum, ymin, ymax);

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run tomography of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1)
            {
                x[i] = detector_data[i]->x;
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;

                // m_correct->alignment_data(x[i], y[i], z[i], i);
            }
        }

        int ref_cnt_x = 0;
        double ref_zxx[cDET_LAYER_MAX] = {0};
        double ref_zxz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (x[j] > 0)
            {
                ref_zxx[ref_cnt_x] = x[j];
                ref_zxz[ref_cnt_x] = z[j];
                ref_cnt_x++;
            }
        }
        int ref_cnt_y = 0;
        double ref_zyy[cDET_LAYER_MAX] = {0};
        double ref_zyz[cDET_LAYER_MAX] = {0};
        for (int j = 0; j < cDET_LAYER; j++)
        {
            if (y[j] > 0)
            {
                ref_zyy[ref_cnt_y] = y[j];
                ref_zyz[ref_cnt_y] = z[j];
                ref_cnt_y++;
            }
        }

        if (ref_cnt_x >= 3 && ref_cnt_y >= 3)
        {
            double kzx, bzx, rmse_zx, R2_zx;
            hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
            double kzy, bzy, rmse_zy, R2_zy;
            hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
            if (rmse_zx < 1 && rmse_zx < 1)
            {
                TVector3 dirc(kzx, kzy, 1);
                dirc.RotateX(det_rotx);
                dirc.RotateY(det_roty);

                double target_plane = 1000;  // mm
                dirc *= target_plane;
                double xx = dirc.X();
                double yy = dirc.Y();

                if (xx > xmin && xx < xmax && yy > ymin && yy < ymax)
                {
                    hist_bkg->Fill(xx, yy);
                }
            }
        }
    }

    // for(int x=1; x<hist_bkg->GetNbinsX()+1; x++)
    // {
    //     for(int y=1; y<hist_bkg->GetNbinsY()+1; y++)
    //     {
    //         double z = hist_bkg->GetBinContent(x, y);
    //         double theta = 90 - hist_bkg->GetYaxis()->GetBinCenter(y);
    //         z = z * 1. / (cos(theta)*cos(theta));
    //         hist_bkg->SetBinContent(x, y, z);
    //     }
    // }

    TCanvas *c1 = new TCanvas("c", "c", 800, 600);
    hist_bkg->Draw("colz");
    list_file::create_path(m_png_path);
    string pngname = m_png_path + "bkghist.png";
    c1->Print(pngname.data());
    delete c1;

    string filename = "bkg.root";
    // string filename = det_target + ".root";
    TFile *f = new TFile(filename.data(), "recreate");
    hist_bkg->Write();
    delete f;

    return true;
}

bool info_calc::amp_eff_map()
{
    TH2F *amp_map[cDET_LAYER_MAX];
    TH2F *eff_map[cDET_LAYER_MAX];

    run_amp_sum_subfunc(amp_map);
    run_eff_calc_notuse_correct_combine_xy_subfunc(eff_map);

    int array_size[cDET_LAYER_MAX] = {0};
    double array_amp[cDET_LAYER_MAX][10000];
    double array_eff[cDET_LAYER_MAX][10000];

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 2; j <= amp_map[i]->GetNbinsX() - 1; j++)
        {
            for (int k = 2; k <= amp_map[i]->GetNbinsY() - 1; k++)
            {
                array_amp[i][array_size[i]] = amp_map[i]->GetBinContent(j, k);
                array_eff[i][array_size[i]] = eff_map[i]->GetBinContent(j, k);
                if (amp_map[i]->GetBinContent(j, k) > 0 && eff_map[i]->GetBinContent(j, k) > 0)
                {
                    array_size[i]++;
                }
            }
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TGraph *g1 = new TGraph(array_size[i], array_amp[i], array_eff[i]);
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);
        g1->Draw("AP*");
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "amp_eff_map_" + to_string(i) + ".png";
        c1->Print(pngname1.data());
        delete c1;
        delete g1;
    }

    return true;
}

bool info_calc::show_data()
{
    int show_data_number = 10000;
    for (int evt = 0; evt < entries && evt < show_data_number; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run show of event: " << evt << endl;
        }

        bool b_is_show = false;
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1)
            {
                b_is_show = true;
                break;
            }
        }

        if (b_is_show)
        {
            cout << "-------------------------------------------------" << endl;
            cout << "Event: " << evt << endl;
            cout << "x: ";
            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig_x != 0)
                    cout << setw(8) << fixed << setprecision(2) << detector_data[i]->x;
                else
                    cout << setw(8) << fixed << setprecision(2) << 0;
            }
            cout << endl;

            cout << "y: ";
            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig_y != 0)
                    cout << setw(8) << fixed << setprecision(2) << detector_data[i]->y;
                else
                    cout << setw(8) << fixed << setprecision(2) << 0;
            }
            cout << endl;

            cout << "z: ";
            for (int i = 0; i < cDET_LAYER; i++)
            {
                if (detector_data[i]->sig_x != 0 || detector_data[i]->sig_y != 0)
                    cout << setw(8) << fixed << setprecision(2) << detector_data[i]->z;
                else
                    cout << setw(8) << fixed << setprecision(2) << 0;
            }
            cout << endl;
            cout << endl;
        }
    }

    return true;
}

bool info_calc::run_amp_sum_subfunc(TH2F **amp_map)
{
    double cDET_IGNORE_SIZE = 10.;  // this is not a global var

    const int det_divide_bin_num = 10;
    // double det_divide_bin_width = (cDET_SIZE - cDET_IGNORE_SIZE) / det_divide_bin_num;

    // save total amp map of detector
    // TH2F *amp_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string ampmapname = "amp-map_" + to_string(i);
        // amp_map[i] = new TH2F(ampmapname.data(), ampmapname.data(),
        //                       det_divide_bin_num, cDET_IGNORE_SIZE, cDET_SIZE-cDET_IGNORE_SIZE,  // 去掉边上5mm
        //                       det_divide_bin_num, cDET_IGNORE_SIZE, cDET_SIZE-cDET_IGNORE_SIZE); // unit: mm
        amp_map[i] = new TH2F(ampmapname.data(), ampmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                              det_divide_bin_num, 0, cDET_SIZE[i]);
    }

    // init amp hist
    TH1F *amp_spectrum[cDET_LAYER_MAX][det_divide_bin_num][det_divide_bin_num];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < det_divide_bin_num; j++)
        {
            for (int k = 0; k < det_divide_bin_num; k++)
            {
                string spectrumname = "amp-spectrum_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k);
                amp_spectrum[i][j][k] = new TH1F(spectrumname.data(), spectrumname.data(), 100, 0, 600);
            }
        }
    }

    // fill amp data
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run amplitude of event: " << evt << endl;
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1 && detector_data[i]->x > 0 && detector_data[i]->y > 0)
            {
                double x = detector_data[i]->x;
                double y = detector_data[i]->y;
                double z = detector_data[i]->z;
                // m_correct->alignment_data(x, y, z, i);

                double det_divide_bin_width = cDET_SIZE[i] / det_divide_bin_num;
                int xbin = (int)((x) / det_divide_bin_width);
                int ybin = (int)((y) / det_divide_bin_width);
                if (xbin < 0 || xbin >= det_divide_bin_num)
                    continue;
                if (ybin < 0 || ybin >= det_divide_bin_num)
                    continue;

                double amp_x = detector_data[i]->hit_amp_total_x;
                double amp_y = detector_data[i]->hit_amp_total_y;
                double charge = (amp_x + amp_y) / 28;  // fC
                // amp_map[i]->Fill(x, y);
                amp_spectrum[i][xbin][ybin]->Fill(charge);
            }
        }
    }

    // fit amp data and save to map
    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < det_divide_bin_num; j++)
        {
            for (int k = 0; k < det_divide_bin_num; k++)
            {
                // if (amp_spectrum[i][j][k]->GetEntries() < 1000)
                // {
                //     continue;
                // }

                // if (!(i==0&&j==0&&k==2)) continue;
                Double_t fr[2];
                Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
                double mean = amp_spectrum[i][j][k]->GetMean();
                double dev = amp_spectrum[i][j][k]->GetStdDev();
                double max = amp_spectrum[i][j][k]->GetMaximum();
                int max_bin = amp_spectrum[i][j][k]->GetMaximumBin();
                double max_val = amp_spectrum[i][j][k]->GetBinCenter(max_bin);
                // cout << "Mean: " << mean << " StdDev: " << dev << " Max Bin: " << max_val << " Max: " << max << endl;
                fr[0] = 0.3 * max_val;
                fr[1] = 3.0 * mean;

                // sv[0]=0.2*dev; sv[1]=0.4*mean; sv[2]=dev*max; sv[3]=0.05*dev;
                sv[0] = 0.2 * dev;
                sv[1] = max_val;
                sv[2] = dev * max;
                sv[3] = 0.05 * dev;
                pllo[0] = 0.5;
                pllo[1] = 5.0;
                pllo[2] = 1.0;
                pllo[3] = 0;
                plhi[0] = 5000000.0;
                plhi[1] = 1e7;
                plhi[2] = 1e9;
                plhi[3] = 5000000.0;

                Double_t chisqr;
                Int_t ndf;
                TF1 *fitsnr =
                    global_function::langaufit(amp_spectrum[i][j][k], fr, sv, pllo, plhi, fp, fpe, &chisqr, &ndf);

                double det_divide_bin_width = cDET_SIZE[i] / det_divide_bin_num;
                // save amp fit data to detector map
                amp_map[i]->Fill(j * det_divide_bin_width, k * det_divide_bin_width, fp[1]);
                // amp_map[i]->Fill(j*det_divide_bin_width+cDET_IGNORE_SIZE,
                //                  k*det_divide_bin_width+cDET_IGNORE_SIZE,
                //                  mean);

                TCanvas *c = new TCanvas("c", "c", 800, 600);
                amp_spectrum[i][j][k]->Draw();
                fitsnr->Draw("same");
                list_file::create_path(m_png_path);
                string pngname =
                    m_png_path + "amp-spectrum_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k) + ".png";
                c->Print(pngname.data());
                delete c;
            }
        }
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c = new TCanvas("c", "c", 800, 600);
        amp_map[i]->SetStats(kFALSE);
        amp_map[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        string pngname = m_png_path + "amp-sum_" + to_string(i) + ".png";
        c->Print(pngname.data());
        delete c;

        // 均匀性RMS/Mean
        double mean = 0;
        double rms = 0;
        int nbins = 0;
        for (int x = 1; x < amp_map[i]->GetNbinsX() + 1; x++)
        {
            for (int y = 1; y < amp_map[i]->GetNbinsY() + 1; y++)
            {
                double amp = amp_map[i]->GetBinContent(x, y);
                mean += amp;
                nbins++;
            }
        }
        mean = mean / nbins;
        for (int x = 1; x < amp_map[i]->GetNbinsX() + 1; x++)
        {
            for (int y = 1; y < amp_map[i]->GetNbinsY() + 1; y++)
            {
                double amp = amp_map[i]->GetBinContent(x, y);
                rms += (amp - mean) * (amp - mean);
            }
        }
        rms = sqrt(rms / nbins);
        cout << "det-" << i << " rms: " << rms << " mean: " << mean << " rms/mean: " << rms / mean << endl;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        for (int j = 0; j < det_divide_bin_num; j++)
        {
            for (int k = 0; k < det_divide_bin_num; k++)
            {
                if (amp_spectrum[i][j][k] != nullptr)
                {
                    delete amp_spectrum[i][j][k];
                    amp_spectrum[i][j][k] = nullptr;
                }
            }
        }
    }

    // for (int i=0; i<cDET_LAYER; i++)
    // {
    //     if (amp_map[i] != nullptr) {
    //         delete amp_map[i];
    //         amp_map[i] = nullptr;
    //     }
    // }

    return true;
}

bool info_calc::run_eff_calc_notuse_correct_combine_xy_subfunc(TH2F **eff_map)
{
    // settings
    const int det_divide_bin_num = 10;
    // double det_divide_bin_width = cDET_SIZE / det_divide_bin_num;

    // save detector hit counter
    TH2F *hits_exp_map[cDET_LAYER_MAX];
    TH2F *hits_real_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string hitsexpmapname = "hits-exp-map_" + to_string(i);
        hits_exp_map[i] = new TH2F(hitsexpmapname.data(), hitsexpmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                                   det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
        string hitsrealmapname = "hits-real-map_" + to_string(i);
        hits_real_map[i] = new TH2F(hitsrealmapname.data(), hitsrealmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                                    det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    // fill hit counter
    for (int evt = 0; evt < entries; evt++)
    {
        hit_data_tree->GetEntry(evt);
        if (evt % 10000 == 0)
        {
            cout << "Run efficient of event: " << evt << endl;
        }

        double x[cDET_LAYER_MAX] = {0};
        double y[cDET_LAYER_MAX] = {0};
        double z[cDET_LAYER_MAX] = {0};
        for (int i = 0; i < cDET_LAYER; i++)
        {
            if (detector_data[i]->sig == 1)
            {
                x[i] = detector_data[i]->x;
                y[i] = detector_data[i]->y;
                z[i] = detector_data[i]->z;

                // m_correct->alignment_data(x[i], y[i], z[i], i);
            }
        }

        for (int i = 0; i < cDET_LAYER; i++)
        {
            int ref_cnt_x = 0;
            double ref_zxx[cDET_LAYER_MAX] = {0};
            double ref_zxz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (x[j] > 0)
                {
                    ref_zxx[ref_cnt_x] = x[j];
                    ref_zxz[ref_cnt_x] = z[j];
                    ref_cnt_x++;
                }
            }

            int ref_cnt_y = 0;
            double ref_zyy[cDET_LAYER_MAX] = {0};
            double ref_zyz[cDET_LAYER_MAX] = {0};
            for (int j = 0; j < cDET_LAYER; j++)
            {
                if (i == j)
                {
                    continue;
                }

                if (y[j] > 0)
                {
                    ref_zyy[ref_cnt_y] = y[j];
                    ref_zyz[ref_cnt_y] = z[j];
                    ref_cnt_y++;
                }
            }

            if (ref_cnt_x == cDET_LAYER - 1 && ref_cnt_y == cDET_LAYER - 1)
            {
                double kzx, bzx, rmse_zx, R2_zx;
                hit_correct::least_square_fit(ref_zxz, ref_zxx, ref_cnt_x, kzx, bzx, rmse_zx, R2_zx);
                double kzy, bzy, rmse_zy, R2_zy;
                hit_correct::least_square_fit(ref_zyz, ref_zyy, ref_cnt_y, kzy, bzy, rmse_zy, R2_zy);
                if (rmse_zx < 1 && rmse_zy < 1)
                {
                    // fix position use alignment
                    double fit_x = kzx * cDET_Z[i] + bzx;
                    double fit_y = kzy * cDET_Z[i] + bzy;
                    double fit_z = cDET_Z[i];
                    m_correct->alignment_data(fit_x, fit_y, fit_z, i);
                    fit_x = kzx * fit_z + bzx;
                    fit_y = kzy * fit_z + bzy;

                    if (fit_x > 0 && fit_x < cDET_SIZE[i] && fit_y > 0 && fit_y < cDET_SIZE[i])
                    {
                        hits_exp_map[i]->Fill(fit_x, fit_y);

                        if (x[i] > 0 && y[i] > 0 && abs(x[i] - fit_x) < 5 && abs(y[i] - fit_y) < 5)
                        {
                            hits_real_map[i]->Fill(fit_x, fit_y);
                        }
                    }
                }
            }
        }
    }

    // save detector efficient
    // TH2F *eff_map[cDET_LAYER_MAX];
    for (int i = 0; i < cDET_LAYER; i++)
    {
        string effmapname = "eff-map_" + to_string(i);
        eff_map[i] = new TH2F(effmapname.data(), effmapname.data(), det_divide_bin_num, 0, cDET_SIZE[i],
                              det_divide_bin_num, 0, cDET_SIZE[i]);  // unit: mm
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        double mean = 0;
        int nbins = 0;
        for (int j = 1; j <= det_divide_bin_num; j++)
        {
            for (int k = 1; k <= det_divide_bin_num; k++)
            {
                int hits_exp = hits_exp_map[i]->GetBinContent(j, k);
                int hits_real = hits_real_map[i]->GetBinContent(j, k);
                if (hits_exp > 0)
                // if (hits_exp > 0 && hits_exp > 100) // 限制统计量
                {
                    eff_map[i]->SetBinContent(j, k, (double)hits_real / (double)hits_exp);
                }

                mean += (double)hits_real / (double)hits_exp;
                nbins++;
            }
        }
        mean = mean / nbins;

        cout << "det-" << i << " eff: " << mean << endl;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        eff_map[i]->SetStats(kFALSE);
        eff_map[i]->Draw("colztext");
        list_file::create_path(m_png_path);
        string pngname1 = m_png_path + "eff_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        delete c1;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        hits_exp_map[i]->SetStats(kFALSE);
        hits_exp_map[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_exp_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        delete c1;
    }
    for (int i = 0; i < cDET_LAYER; i++)
    {
        TCanvas *c1 = new TCanvas("c", "c", 800, 600);

        hits_real_map[i]->SetStats(kFALSE);
        hits_real_map[i]->Draw("colztext");
        string pngname1 = m_png_path + "hits_real_map" + to_string(i) + ".png";
        c1->Print(pngname1.data());

        delete c1;
    }

    for (int i = 0; i < cDET_LAYER; i++)
    {
        delete hits_exp_map[i];
        delete hits_real_map[i];
    }

    return true;
}

*/
