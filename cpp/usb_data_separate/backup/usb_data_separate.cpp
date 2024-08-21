#include <boost/program_options.hpp>
#include "usb_data_separate.h"

usb_data_separate::run(int argc, char* argv[]){
  namespace po = boost::program_options;
  std::string fname;
  po::options_description cmdline_options("generic options");
  cmdline_options.add_options()
    ("version,v","muon150 software @2024 version 0.0.0")
    ("help","help message")
    ("file,f",po:;value<std::string>(&fname),"input file");
  po::variables_map vm;
  po::store(po::command_line_parser(argc,argv).options(cmdline_options).run(),vm);
  notify(vm);

  
  
  
}
