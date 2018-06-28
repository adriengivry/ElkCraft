#pragma once

#include <ElkAPI/IManager.h>

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Scene.h"
#include "ElkGameEngine/Objects/GameObject.h"

namespace ElkGameEngine
{
	namespace Managers
	{
		/*
		* Manager to manipulate scenes
		*/
		class ELK_GAMEENGINE_EXPORT SceneManager : public ElkAPI::IManager
		{
		public:
			explicit SceneManager();
			~SceneManager() = default;

			void Setup() override;
			void Close() override;

			void Update();

			void SetCurrentScene(const std::string& p_sceneName);
			ElkGameEngine::Objects::Scene& CreateScene(const std::string& p_sceneName, bool p_setCurrent = true);
			ElkGameEngine::Objects::Scene& GetCurrentScene() const;
			void RemoveScene(const std::string& p_sceneName);

			bool HasCurrentScene() const;

		private:
			std::map<std::string, std::shared_ptr<ElkGameEngine::Objects::Scene>>	m_scenes;
			std::shared_ptr<Objects::Scene>											m_currentScene;
		};
	}
}
