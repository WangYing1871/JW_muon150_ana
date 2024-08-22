#include <fstream>
#include <filesystem>
#include <sstream>
#include "util.hpp"
#include "config.h"

int config_e2d_table::config(){
  if(!std::filesystem::exists(m_fname)) return -1;
  std::ifstream fin(m_fname.c_str());
  while(!fin.eof()){
    std::string sbuf;
    std::getline(fin,sbuf);
    util::trim_space(sbuf);
    if(!sbuf.empty()){
      std::istringstream isstr(sbuf.c_str());
      size_t a,b;
      char _;
      isstr>>a>>_>>b; //TODO 
      if (isstr) m_table.emplace(a,b); } }
  fin.close();
  return 0; }


int config_noise::config(){
  auto const& read_table = [](std::string const& name, noise_table_t& table){
    std::ifstream fin(name.c_str());
    std::size_t index=0;
    while(!fin.eof()){
      std::string sbuf;
      std::getline(fin,sbuf);
      if (!sbuf.empty()){
        std::stringstream sstr(sbuf.c_str());
        sstr
          >>table[index][0]
          >>table[index][1]
          >>table[index][2]
          >>table[index][3]
          >>table[index][4]
          >>table[index][5]
          ;
        index++;
      }
    }
    fin.close();
  };
  for (auto&& [x,y] : m_means){
    m_mean_table[x];
    read_table(y,m_mean_table.at(x));
  }
  for (auto&& [x,y] : m_rms){
    m_rms_table[x];
    read_table(y,m_rms_table.at(x));
  }

  return 0;
  

  
}

float config_noise::get_mean(size_t l, size_t chip, size_t channel){
  return m_mean_table[l][channel][chip]; }
float config_noise::get_rms(size_t l, size_t chip, size_t channel){
  return m_rms_table[l][channel][chip]; }
