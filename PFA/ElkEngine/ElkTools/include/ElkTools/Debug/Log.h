#pragma once

#include "ElkTools/export.h"

#include <string>
#include <iostream>
#include <fstream>

namespace ElkTools
{
	namespace Debug
	{
		/*
		* Static class to display error messages on the console or a file
		*/
		class ELK_TOOLS_EXPORT Log
		{
		public:
			enum class LogLevel
			{
				LOG_DEFAULT,
				LOG_INFO,
				LOG_WARNING,
				LOG_ERROR
			};

			enum class LogColor
			{
				DEFAULT,
				BLUE,
				RED,
				GREEN,
				YELLOW,
				WHITE
			};

			enum class LogMode
			{
				DEFAULT,
				LOG_TO_CONSOLE,
				LOG_TO_FILE,
				LOG_TO_ALL
			};


			static const std::string DEFAULT_HEADER;
			static const std::string INFO_HEADER;
			static const std::string WARNING_HEADER;
			static const std::string ERROR_HEADER;
			static const std::string COMPILE_DATE;
			static const std::string LOG_EXTENSION;

			static void Process(const std::string& p_message, LogLevel p_logLevel = LogLevel::LOG_DEFAULT, LogColor p_logColor = LogColor::DEFAULT, LogMode p_logMode = LogMode::DEFAULT);
			static void LineBreak(uint8_t p_breaks = 1, LogMode p_logMode = LogMode::DEFAULT);

		private:
			static void PrintToConsole(const std::string& p_message, LogLevel p_logLevel, LogColor p_logColor);
			static void PrintToFile(const std::string& p_message, LogLevel p_logLevel);
			static std::string GetLogHeader(LogLevel p_logLevel);

			static std::ofstream __OUTPUT_FILE;
		};
	}
}