#include "stdafx.h"

#include "ElkTools/Utils/IniLoader.h"
#include "ElkTools/Utils/SharedData.h"
#include "ElkTools/Utils/Date.h"
#include "ElkTools/Debug/Log.h"
#include "ElkTools/Debug/ConsoleColor.h"

using namespace ElkTools::Debug;

std::string const Log::DEFAULT_HEADER	= "";
std::string const Log::INFO_HEADER		= "[INFO] ";
std::string const Log::WARNING_HEADER	= "[WARNING] ";
std::string const Log::ERROR_HEADER		= "[ERROR] ";
std::string const Log::COMPILE_DATE		= ElkTools::Utils::Date::GetDate();
std::string const Log::LOG_EXTENSION	= ".elklog";

std::ofstream Log::__OUTPUT_FILE;

void Log::Process(const std::string& p_message, const LogLevel p_logLevel, const LogColor p_logColor, const LogMode p_logMode)
{
	if (!Utils::IniLoader::GetBool("logs.ini", "enable"))
		return;

	switch (p_logLevel)
	{
	case LogLevel::LOG_DEFAULT:
		if (!Utils::IniLoader::GetBool("logs.ini", "log_default"))
			return;
		break;
	case LogLevel::LOG_INFO:
		if (!Utils::IniLoader::GetBool("logs.ini", "log_info"))
			return;
		break;
	case LogLevel::LOG_WARNING:
		if (!Utils::IniLoader::GetBool("logs.ini", "log_warning"))
			return;
		break;
	case LogLevel::LOG_ERROR:
		if (!Utils::IniLoader::GetBool("logs.ini", "log_error"))
			return;
		break;
	}

	switch (p_logMode)
	{
	case LogMode::DEFAULT:
		if (Utils::IniLoader::GetBool("logs.ini", "log_to_console"))
			PrintToConsole(p_message, p_logLevel, p_logColor);

		if (Utils::IniLoader::GetBool("logs.ini", "log_to_file"))
			PrintToFile(p_message, p_logLevel);
		break;

	case LogMode::LOG_TO_CONSOLE:
		PrintToConsole(p_message, p_logLevel, p_logColor);
		break;

	case LogMode::LOG_TO_FILE:
		PrintToFile(p_message, p_logLevel);
		break;

	case LogMode::LOG_TO_ALL:
		PrintToConsole(p_message, p_logLevel, p_logColor);
		PrintToFile(p_message, p_logLevel);
		break;
	}
}

void ElkTools::Debug::Log::LineBreak(uint8_t p_breaks, LogMode p_logMode)
{
	for (uint8_t i = 0; i < p_breaks; ++i)
		Process("", LogLevel::LOG_DEFAULT, LogColor::DEFAULT, p_logMode);
}

void Log::PrintToConsole(const std::string& p_message, const LogLevel p_logLevel, const LogColor p_logColor)
{
	if (p_logColor == LogColor::DEFAULT)
	{
		switch (p_logLevel)
		{
		case LogLevel::LOG_DEFAULT:
			std::cout << COLOR_DEFAULT;
			break;
		case LogLevel::LOG_INFO:
			std::cout << COLOR_BLUE;
			break;
		case LogLevel::LOG_WARNING:
			std::cout << COLOR_YELLOW;
			break;
		case LogLevel::LOG_ERROR:
			std::cout << COLOR_RED;
			break;
		}
	}
	else
	{
		switch (p_logColor)
		{
		case LogColor::BLUE:
			std::cout << COLOR_BLUE;
			break;
		case LogColor::GREEN:
			std::cout << COLOR_GREEN;
			break;
		case LogColor::RED:
			std::cout << COLOR_RED;
			break;
		case LogColor::WHITE:
			std::cout << COLOR_WHITE;
			break;
		case LogColor::YELLOW:	
			std::cout << COLOR_YELLOW;
			break;
		}
	}

	std::ostream& output = p_logLevel == LogLevel::LOG_ERROR ? std::cerr : std::cout;

	output << GetLogHeader(p_logLevel) << p_message << std::endl;

	std::cout << COLOR_DEFAULT;
}

void Log::PrintToFile(const std::string& p_message, const LogLevel p_logLevel)
{
	if (!__OUTPUT_FILE.is_open())
	{
		__OUTPUT_FILE.open(ElkTools::Utils::IniLoader::GetString("logs.ini", "log_folder_path") + COMPILE_DATE + LOG_EXTENSION, std::ios_base::app);
	}
	
	if (__OUTPUT_FILE.is_open())
		__OUTPUT_FILE << GetLogHeader(p_logLevel) << p_message << std::endl;
	else
		Log::PrintToConsole("Unable to create log file", Log::LogLevel::LOG_ERROR, Log::LogColor::DEFAULT);
}

std::string Log::GetLogHeader(const LogLevel p_logLevel)
{
	switch (p_logLevel)
	{
	case LogLevel::LOG_DEFAULT:	return DEFAULT_HEADER;
	case LogLevel::LOG_INFO:	return INFO_HEADER;
	case LogLevel::LOG_WARNING:	return WARNING_HEADER;
	case LogLevel::LOG_ERROR:	return ERROR_HEADER;
	}

	return "";
}
