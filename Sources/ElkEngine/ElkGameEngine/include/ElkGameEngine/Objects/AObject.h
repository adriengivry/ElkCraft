#pragma once

#include <string>

#include "ElkGameEngine/export.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		/*
		 * AObject is the base class for every object instancied. It has a unique ID that can identify him
		 */
		class ELK_GAMEENGINE_EXPORT AObject
		{
			static uint32_t ID_INCREMENT;
		public:
			AObject();
			virtual ~AObject() = default;

			uint64_t GetInstanceID() const;
			virtual std::string GetName();

			void SetName(const std::string& p_name);

		protected:
			uint32_t m_instanceID = 0;
		};
	}
}
