#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <iostream>

#include "ElkTools/Debug/Log.h"
#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		/*
		* ConfigLoader is a static class which can extract data associated to an attribute in different config files
		*/

		class ELK_TOOLS_EXPORT IniLoader
		{
		public:
			static int GetInt(const std::string& p_file, const std::string& p_attribute);
			static float GetFloat(const std::string& p_file, const std::string& p_attribute);
			static std::string GetString(const std::string& p_file, const std::string& p_attribute);
			static bool GetBool(const std::string& p_file, const std::string& p_attribute);

		private:
			static std::string GetData(const std::string& p_file, const std::string& p_attribute);
			static std::string GetAttributeLine(const std::string& p_confPath, const std::string& p_attribute);
			static std::string GetAttributeValue(std::string p_line);
			static std::string GetAttributeName(std::string p_line);
			static void LoadEveryFiles();

			static bool __LOADED;
			static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> __DATA;
		};
	}
}