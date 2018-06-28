#include "stdafx.h"

#include "ElkTools/Utils/ManagerLocator.h"

std::unordered_map<size_t, ElkAPI::IManager*> ElkTools::Utils::ManagerLocator::m_managers;

