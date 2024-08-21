#include "list_file.hh"

using namespace std;

list_file::list_file(string dir_name)
{
	string null_str = "";
	path = dir_name;
	file_lists = get_file(dir_name, null_str.c_str());
}

list_file::list_file(string dir_name, const char* filter)
{
	file_lists = get_file(dir_name, filter);
	path = dir_name;
	//cout << file_lists[1] << endl;
}


vector<string> list_file::get_file(string cate_dir, const char* suffix)
{
	vector<string> files;
	DIR* dir;
	struct dirent* ptr;
	char* name_filter;
	char* name_filter_last;
	string filename;

	if ((dir = opendir(cate_dir.c_str())) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}

	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
			continue;
		else if (ptr->d_type == 8)    ///file
		{
			name_filter = strtok(ptr->d_name, ".");
			name_filter = strtok(NULL, ".");
			// Check if the . is the last . in the filename. Other wise, the vim
			// swap file will create the .filename.dat.swp file
			name_filter_last = strtok(NULL, ".");
			while (name_filter_last != NULL)
			{
				name_filter = name_filter_last;
				name_filter_last = strtok(NULL, ".");
			}
			if (name_filter != NULL && strcmp(name_filter, suffix) == 0)
			{
				//cout << ptr->d_name << endl;
				filename = ptr->d_name + string(".") + string(suffix);
				//cout << " " << filename << endl;
				files.push_back(filename);
			}
			
		}
		else if (ptr->d_type == 10)    ///link file
			continue;
		else if (ptr->d_type == 4)    ///dir
		{
			//files.push_back(ptr->d_name);
			continue;

		}
	}
	closedir(dir);

	sort(files.begin(), files.end());
	//cout << "---" << endl;
	//cout << files[1] << endl;
	file_lists_contain = files;
	file_lists_exclude = files;
	file_lists_con_exc = files;
	return files;

}

void list_file::filter_file(string str_con, string str_exc)
{
	int list_sz = file_lists.size();

	bool is_contain;
	bool is_exclude;
	int con_idx = 0;
	int exc_idx = 0;
	int con_exc_idx = 0;
	string filename;
	for (int i = 0; i < list_sz; i++)
	{
		filename = file_lists[i];
		is_contain = (filename.find(str_con) != string::npos);
		is_exclude = (filename.find(str_exc) != string::npos);
		if (is_contain)
		{
			con_idx++;
		}
		else
		{
			file_lists_contain.erase(file_lists_contain.begin() + con_idx);
		}

		if (!is_exclude)
		{
			exc_idx++;
		}
		else
		{
			file_lists_exclude.erase(file_lists_exclude.begin() + exc_idx);
		}

		if (is_contain && !is_exclude)
		{
			con_exc_idx++;
		}
		else
		{
			file_lists_con_exc.erase(file_lists_con_exc.begin() + con_exc_idx);
		}
	}
}

bool list_file::check_path_exist(const char* filename)
{
	struct stat info;
	if (stat(filename, &info) != 0)
	{
		cout << filename << " cannot open!" << endl;
		return false;
	}
	else if (info.st_mode & S_IFDIR)
	{
		return true;
	}
	else
	{
		cout <<"Path "<< filename << " does not exist!" << endl;
		return false;
	}
}

void list_file::show(int show_type)
{
	bool show_origin = ((show_type & 0x1) == 1);
	bool show_contain = ((show_type & 0x2) == 2);
	bool show_exclude = ((show_type & 0x4) == 4);
	bool show_con_exc = ((show_type & 0x8) == 8);
	if (show_origin)
	{
		cout << "All file: " << endl;
		show(file_lists);
	}
	if (show_contain)
	{
		cout << "Contain file: " << endl;
		show(file_lists_contain);
	}
	if (show_exclude)
	{
		cout << "Exclude file: " << endl;
		show(file_lists_exclude);
	}
	if (show_con_exc)
	{
		cout << "Contain & exclude file: " << endl;
		show(file_lists_con_exc);
	}
}

void list_file::show(vector<string> str_vec)
{
	for (int i = 0; i < str_vec.size(); i++)
	{
		cout << str_vec[i] << endl;
	}
}

void list_file::filter_reset()
{
	file_lists_contain = file_lists;
	file_lists_exclude = file_lists;
	file_lists_con_exc = file_lists;
}

// Push 
vector<string> list_file::split_string(const string& str, string delims, string str2push)
{
	std::vector<std::string> result;
	size_t i = str.rfind(delims, str.length());
	if (i == str.length() - 1 || i == string::npos)
	{
		result.push_back(str);
		result.push_back(str2push);
	}
	else
	{
		result.push_back(str.substr(i + 1, str.length() - 1));
		result.push_back(str.substr(0, i));
	}
	return result;
}


