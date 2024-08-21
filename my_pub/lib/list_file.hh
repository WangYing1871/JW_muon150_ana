#ifndef LIST_FILE_HH
#define LIST_FILE_HH
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <cstring>
#include <string>
// #include <unistd.h>

#ifdef __linux__
#include <dirent.h>
#endif

// #include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
class list_file
{
public:
	list_file(string);
	list_file(string, const char *);
	~list_file(){};
	// void get_file_lists(string cate_dir);
	void filter_file(string str_con = "", string str_exc = "");
	static bool check_path_exist(const char *filename);

	// Show type: lower 4 bits indicate which lists will be display.
	// 'b0001: file_lists
	// 'b0010: file_lists_contain
	// 'b0100: file_lists_exclude
	// 'b1000: file_lists_con_exc
	void show(int show_type = 0xF);
	static void show(vector<string> str_vec);
	void filter_reset();
	string path;
	vector<string> file_lists;
	vector<string> file_lists_exclude;
	vector<string> file_lists_contain;
	vector<string> file_lists_con_exc;

	static vector<string> split_string(const string &str, string delims, string str2push);
	static vector<string> split_string(const string &str, string delims, int inc = 0);

private:
	vector<string> get_file(string cate_dir, const char *);
};
#endif // !LIST_FILE_HH
