#include "util.hpp"
namespace util{

void swap(unsigned char* v) {
  unsigned char v0 = v[0]<<(__CHAR_BIT__/2); v[0] = v0+(v[0]>>(__CHAR_BIT__/2));}

void swap(void*) {}

void trim_space(std::string& s){
  for (auto iter = s.begin(); iter != s.end(); ++iter)
    if (*iter != ' ' && *iter != '\t')
      for(auto riter = s.rbegin(); riter != s.rend(); ++riter)
        if (*riter!= ' ' && *riter!='\t'){
          s = s.substr(
              std::distance(s.begin(),iter)
              ,std::distance(riter,s.rend())-std::distance(s.begin(),iter)
              ); return; }
  s.clear(); }

}
