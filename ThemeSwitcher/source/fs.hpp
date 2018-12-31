#pragma once
#include "MyTypes.h"
#include <switch.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <dirent.h>

extern std::string CfwFolder;
bool StrEndsWith(const std::string &str, const std::string &suffix);

void CopyFile(const std::string &original, const std::string &target);

bool CheckThemesFolder();
std::vector<std::string> SearchCfwFolders();
std::vector<std::string> GetShuffleTargets();
void InstallShuffleTarget(const std::string &path);

void UninstallTheme();
void CreateThemeStructure(const std::string &tid);

std::string GetFileName(const std::string &path);
std::string GetPath(const std::string &path);
std::string GetParentDir(const std::string &path);