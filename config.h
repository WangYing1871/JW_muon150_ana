#ifndef config_H
#define config_H 1 

#include <string>
#include <array>
#include <unordered_map>
struct config_base{
};

struct config_e2d_table: public config_base{
  std::string m_fname;
  std::unordered_map<size_t,size_t> m_table;
  int config();
  void fname(std::string const& data) {m_fname = data;}
};

struct config_noise : public config_base{
  std::unordered_map<size_t,std::string> m_means;
  std::unordered_map<size_t,std::string> m_rms;

  typedef std::array<std::array<float,6>,128> noise_table_t;

  std::unordered_map<size_t,noise_table_t> m_mean_table;
  std::unordered_map<size_t,noise_table_t> m_rms_table;


  
  float get_mean(size_t l, size_t chip, size_t channel);
  float get_rms(size_t l, size_t chip, size_t channel);

  int config();
};

struct config_manager{
  std::unordered_map<std::string,config_base*> m_tables;
};

#endif
