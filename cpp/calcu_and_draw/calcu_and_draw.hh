// calcu_and_draw.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <TFile.h>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
namespace fs = std::filesystem;

#include "getopt.hh"
#include "constant.hh"
#include "info_calc.hh"
int c = 0;
extern int optind, opterr, optopt;
extern char* optarg;
void show_info(char* name);
vector<string> split_string(const string& str, string delims, int inc = 0);
struct stat info;


