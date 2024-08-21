#ifndef unpack_process_H
#define unpack_process_H 1 

struct unpack_process{


 
private:
  typedef std::vector<std::vector<uint32_t>> aarray_t;
  typedef std::vector<uint32_t> array_t;
  int asic_num;
  double LSB;
  std::vector<aarray_t> ch;
  std::vector<aarray_t> amp;

  aarray_t data;
  aarray_t dataidx;
  array_t initial_data;

  std::string save_path;
  std::string file_write;

private:
  bool is_read_success_flag;
  aarray_t start_index;
  array_t end_index;



 


  
};

#endif
