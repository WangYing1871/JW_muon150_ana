#include <cmath>
#include <iostream>
#include "clusters.hpp"
#include "data_strcut_cint.h"

ClassImp(cluster)

cluster::cluster(cluster_<int> const& rhs){
  range.first = rhs.m_range.first;
  range.second = rhs.m_range.second;
  std::copy(rhs.m_data.begin(),rhs.m_data.end(),std::back_inserter(adc_amp));
  for (auto&& x : adc_amp) x>0 ? strips++ : holes++;
}

double cluster::cog() const{
  long tot_amp=0;
  long tot_pos=0;
  std::vector<int> position;
  for (int i=range.first; i != range.second; ++i) position.emplace_back(i);
  for (size_t index=0; auto&& x : adc_amp){
    if (x==0) {index++ ;continue; }
    //std::cout<<"x: "<<x<<"position: "<<position[index]<<std::endl;
    tot_amp += std::abs(x); tot_pos += std::abs(x)*position[index++];
  }
  double rt = tot_pos/(double)tot_amp;
  if ( 
      /*!util::is_same_smaple(std::begin(adc_amp),std::end(adc_amp)) || */
      (rt!=rt && rt<=0 || rt>=384)){ std::cout<<"XXX==>";
  std::cout<<"rt: "<<rt<<"\n";
  for (auto&& x : position) std::cout<<x<<" ";
  std::cout<<std::endl;
  for (auto&& x : adc_amp) std::cout<<x<<" ";
  std::cout<<std::endl;
  }

  if (std::abs((double)tot_amp) <= 1e-3) { std::cout<<"XXXx\n"; return -999.;}
  //std::cout<<"rt-: "<<rt <<std::endl;
  return rt; }

void cluster::correct(std::vector<cluster>& arr) const{
  if (adc_amp.size()<=2) return;
  //TODO
  





}

ClassImp(hit_position)

hit_position::hit_position(int l):
  layer_id(l){

}

hit_position::hit_position(int l, TClonesArray* x, TClonesArray* y):
  layer_id(l){
  if (!x || !y) return;
  for (long long i=0; i<x->GetEntriesFast(); ++i){
    cluster* cluster_ptr = static_cast<cluster*>(x->At(i));
    double pos = cluster_ptr->cog();
    if (pos!=-999. && pos>=0) position_x.emplace_back(pos);
  }
  for (long long i=0; i<y->GetEntriesFast(); ++i){
    cluster* cluster_ptr = static_cast<cluster*>(y->At(i));
    double pos = cluster_ptr->cog();
    if(pos!=-999. && pos>=0) position_y.emplace_back(pos);
  }

}

hit_position::~hit_position(){
}
