#include "utils.hpp"

#include <dirent.h>

using namespace std;

std::string FindDirContaining(const std::string& sParent, const std::string& sName){
	DIR* dir = opendir(sParent.c_str());
	if(dir==nullptr)
		return "";

	struct dirent* file;
	while ((file = readdir(dir)) != nullptr){
		if(string(file->d_name).find(sName)!=string::npos)
			return sParent+"/"+file->d_name;
	}
	closedir(dir);
	return "";
}
