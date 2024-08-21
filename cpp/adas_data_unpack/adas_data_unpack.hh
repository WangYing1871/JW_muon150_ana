// or project specific include files.

#pragma once

#include <iostream>
#include <TFile.h>
#include "TROOT.h"
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include "unpack_process.hh"
#include "constant.hh"
#include "getopt.hh"

namespace fs = std::filesystem;

// TODO: Reference additional headers your program requires here.
int c = 0;
extern int optind, opterr, optopt;
extern char *optarg;
void show_info(char *name);
struct stat info;
bool read_file(string filename, vector<uint32_t> &initial_data);
void write_file(string filename, vector<uint32_t> &initial_data);
vector<string> split_string(const string &str, string delims, int inc = 0);
