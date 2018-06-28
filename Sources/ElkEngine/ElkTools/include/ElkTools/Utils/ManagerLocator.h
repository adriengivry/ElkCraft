#pragma once

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

#include <ElkAPI/IManager.h>

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		class ELK_TOOLS_EXPORT ManagerLocator
		{
		public:
			template<typename T>
			static void Provide(T& p_service)
			{
				m_managers[typeid(T).hash_code()] = &p_service;
			}

			template<typename T>
			static T& Get()
			{
				return *static_cast<T*>(m_managers[typeid(T).hash_code()]);
			}

		private:
			static std::unordered_map<size_t, ElkAPI::IManager*> m_managers;
		};
	}
}
