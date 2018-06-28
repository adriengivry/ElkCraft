#pragma once

#include <string>

#include <ElkAPI/IDataStructure.h>

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		struct ELK_TOOLS_EXPORT SharedData : ElkAPI::IDataStructure
		{
			static std::string __CONFIG_FOLDER_PATH;
			static std::string __RESOURCE_MANAGERS_FOLDER_PATH;

			static void Setup();
		};
	}
}
