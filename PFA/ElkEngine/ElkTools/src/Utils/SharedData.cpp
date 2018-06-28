#include "stdafx.h"

#include "ElkTools/Utils/SharedData.h"

using namespace ElkTools::Utils;

std::string SharedData::__CONFIG_FOLDER_PATH = "Config/";
std::string SharedData::__RESOURCE_MANAGERS_FOLDER_PATH = "Config/";

void ElkTools::Utils::SharedData::Setup()
{
	__CONFIG_FOLDER_PATH = "Config/";
	__RESOURCE_MANAGERS_FOLDER_PATH = "Config/";
}
