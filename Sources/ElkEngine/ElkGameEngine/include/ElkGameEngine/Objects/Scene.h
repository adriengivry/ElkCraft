#pragma once

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/AObject.h"
#include "ElkGameEngine/Objects/GameObject.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		/*
		 * Class that contain a root gameObject which can have some childs
		 */
		class ELK_GAMEENGINE_EXPORT Scene : public AObject
		{
		public:
			explicit Scene();
			virtual ~Scene();

			std::map<uint64_t, std::shared_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }
			GameObject* GetGameObject(uint64_t p_instanceID);

			GameObject& CreateGameObject(const std::string& p_name);
			bool DestroyGameObject(uint64_t p_instanceID);

		private:
			std::map<uint64_t, std::shared_ptr<GameObject>> m_gameObjects;
		};
	}
}
