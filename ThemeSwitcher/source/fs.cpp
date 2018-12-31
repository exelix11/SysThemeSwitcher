#include "fs.hpp"
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>

using namespace std;

string CfwFolder = "";

vector<string> SearchCfwFolders()
{
	vector<string> res;
	DIR * dir = nullptr;
	#define CHECKFOLDER(f) dir = opendir(f); \
	if (dir) { res.push_back(f); closedir(dir); dir = nullptr;}
	CHECKFOLDER("sdmc:/atmosphere")
	CHECKFOLDER("sdmc:/reinx")
	CHECKFOLDER("sdmc:/sxos")
	#undef CHECKFOLDER
	if (res.size() == 1)
		CfwFolder = res[0];
	return res;
}

bool StrEndsWith(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void InstallShuffleTarget(const string &path)
{
	CreateThemeStructure("0100000000001000");
	for (auto p : filesystem::directory_iterator(path))
	{		
		if (p.is_regular_file() && StrEndsWith(p.path(), ".szs"))
		{
			string Target = CfwFolder + "/titles/0100000000001000/romfs/lyt/" + GetFileName(p.path());
			CopyFile(p.path(), Target);
		}
	}
}

vector<string> GetShuffleTargets()
{
	vector<string> res;
	
	{	
		DIR *dir = nullptr;
		if (dir = opendir("sdmc:/themes/shuffle"))
			closedir(dir);
		else
			return res;
	}
	
	for (auto p : filesystem::directory_iterator("sdmc:/themes/shuffle"))
	{
		if (p.is_directory())
		{			
			res.push_back(p.path());
		}
	}
	
	return res;	
}

void CopyFile(const std::string &original, const std::string &target)
{
	FILE* src = fopen(original.c_str(), "rb");
	FILE* dst = fopen(target.c_str(), "wb");
	
	fseek(src,0,SEEK_END);
	auto flen = ftell(src);
	rewind(src);
	
	#define BufSz 100
	u8 Buf[BufSz];
	while (flen != 0)
	{
		int toRead = flen < BufSz ? flen : BufSz;
		fread(Buf, 1, toRead, src);
		fwrite(Buf, 1, toRead, dst);
		flen -= toRead;
	}	
	#undef BufSz
	fflush(dst);
	fclose(dst);
	fclose(src);
}

/*
vector<u8> OpenFile(const string &name)
{
	FILE* f = fopen(name.c_str(),"rb");
	if (!f){
		// ErrorFatal("File open fail");
		throw (string)"File open fail";
	}
	fseek(f,0,SEEK_END);
	auto len = ftell(f);
	rewind(f);

	vector<u8> coll(len);
	fread(coll.data(), 1, len, f);
	fclose(f);
	return coll;
}
*/
void WriteFile(const string &name,const vector<u8> &data)
{
	if (filesystem::exists(name))
		remove(name.c_str());
	
	FILE* f = fopen(name.c_str(),"wb");
	if (!f)
	{
		throw (string)"File save fail";
	}
	fwrite(data.data(),1,data.size(),f);
	fflush(f);
	fclose(f);
}

void RecursiveDeleteFolder(const string &path)
{
	vector<string> toDelete;
	for (auto p : filesystem::directory_iterator(path))
	{
		if (p.is_directory())
		{
			toDelete.push_back(p.path());
		}
		else if (p.is_regular_file())
		{
			remove(p.path().c_str());
		}
	}
	for (auto p : toDelete)
	{
		RecursiveDeleteFolder(p);
		rmdir(p.c_str());
	}
}

void UninstallTheme()
{
	if (filesystem::exists(CfwFolder + "/titles/0100000000001000"))
		RecursiveDeleteFolder(CfwFolder + "/titles/0100000000001000");
    
	if (filesystem::exists(CfwFolder + "/titles/0100000000001013"))
		RecursiveDeleteFolder(CfwFolder + "/titles/0100000000001013");
}
#define ACCESSPERMS (S_IRWXU|S_IRWXG|S_IRWXO) 
void CreateThemeStructure(const string &tid)
{
	string path = CfwFolder + "/titles";
	mkdir(path.c_str(), ACCESSPERMS);
	path += "/" + tid;
	mkdir(path.c_str(), ACCESSPERMS);
	mkdir((path + "/romfs").c_str(), ACCESSPERMS);
	mkdir((path + "/romfs/lyt").c_str(), ACCESSPERMS);
	if (!filesystem::exists(path + "/fsmitm.flag"))
	{
		vector<u8> t; 
		WriteFile(path + "/fsmitm.flag", t);
	}		
}

bool CheckThemesFolder()
{
	if (!filesystem::exists("sdmc:/themes"))
		mkdir("sdmc:/themes", ACCESSPERMS);
	bool Result = filesystem::exists("sdmc:/themes/systemData");
	if (!Result)
		mkdir("sdmc:/themes/systemData", ACCESSPERMS);
	return Result;
}

string GetFileName(const string &path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

string GetPath(const string &path)
{
	return path.substr(0, path.find_last_of("/\\") + 1);
}

string GetParentDir(const string &path)
{
	string _path = path;
	if (StrEndsWith(_path,"/"))
		_path = _path.substr(0,_path.length() - 1);
	
	return _path.substr(0, _path.find_last_of("/\\") + 1);
}