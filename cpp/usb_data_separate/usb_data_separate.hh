// usb_data_seperate.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <chrono>

#include <sys/types.h>
#include <sys/stat.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "constant.hh"
#include "getopt.hh"

int c = 0;
extern int optind, opterr, optopt;
extern char* optarg;

struct stat info;


// TODO: Reference additional headers your program requires here.

void show_info(char* name);
vector<string> split_string(const string& str, char delims, string str2push="");
