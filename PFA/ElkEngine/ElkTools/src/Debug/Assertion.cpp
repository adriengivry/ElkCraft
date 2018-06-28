#include "stdafx.h"

#include "ElkTools/Debug/Assertion.h"
#include "ElkTools/Debug/Log.h"
#include "ElkTools/Utils/IniLoader.h"

using namespace ElkTools::Debug;
using namespace ElkTools;

void ElkTools::Debug::Assert(bool p_condition, const std::string& p_message, const char* p_file, int p_line)
{
	#ifdef _DEBUG
	if (p_condition)
	{
		const std::string file(p_file);
		const std::string line(std::to_string(p_line));
		const std::string finalMessage(p_message + "(File: " + file + " on line " + line + ")");

		Log::Process(finalMessage, Log::LogLevel::LOG_ERROR);

		system("PAUSE");
		std::exit(EXIT_FAILURE);
	}
	#endif
}
