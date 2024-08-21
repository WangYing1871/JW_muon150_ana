#pragma once

#include <iostream>
#include <TFile.h>

#include "getopt.hh"
#include "constant.hh"
#include "hit_reconstruct.hh"

int c = 0;
extern int optind, opterr, optopt;
extern char* optarg;
void show_info(char* name);
vector<string> split_string(const string& str, string delims, int inc = 0);
