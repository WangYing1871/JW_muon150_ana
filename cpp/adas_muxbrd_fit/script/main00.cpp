//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: 2835516101@qq.com     
//^-^ Time: 2024-08-08 10:26:14   Posi: Hefei
//^-^ File: main00.cpp
//--------------------------------------------------------------------
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

#include "TF1.h"
#include "TH1F.h"
#include "TFile.h"

std::vector<float> get_y(TF1* f, float v, float step){
  std::vector<float> rt;
  float min = f->GetXmin();
  float max = f->GetXmax();
  float buf;
  info_out(min);
  info_out(max);
  for (float i=0; i<500; ++i) info_out(f->GetX(i));
  //for (float i = min; i<max; i+=step){
  //  float y = f->GetX(i)-v;
  //  if (y*buf<0) rt.emplace_back(i);
  //  buf = y;
  //}

  return rt;



}
using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){

  auto* fin = new TFile(argv[1]);
  std::vector<TF1*> funcs;
  for (size_t i=0; i<fin->GetListOfKeys()->GetSize(); ++i){
    //funcs.emplace_back(static_cast<TF1*>(fin->Get(fin->GetListOfKeys()->At(i)->GetName())));
    auto* his = static_cast<TH1F*>(fin->Get(fin->GetListOfKeys()->At(i)->GetName()));
    funcs.emplace_back(his->GetFunction("fitFcn"));
  }
  info_out(funcs.size());

  //for (auto&& x : funcs) if(x) get_y(x,,0,0);
  get_y(funcs[0],funcs[0]->GetMaximum(),0.01);









  return 0;
}

