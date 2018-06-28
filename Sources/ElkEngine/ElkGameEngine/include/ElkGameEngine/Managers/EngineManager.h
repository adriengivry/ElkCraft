#pragma once

#include <memory>

#include "ElkGameEngine/export.h"

#include "ElkGameEngine/Managers/WindowManager.h"
#include "ElkGameEngine/Managers/SceneManager.h"
#include "ElkGameEngine/Managers/InputManager.h"

#include "ElkGameEngine/Objects/GameObject.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Rigidbody.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Camera.h"
#include "ElkGameEngine/Objects/Components/Behaviours/AudioListener.h"
#include "ElkGameEngine/Objects/Components/Behaviours/UICamera.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Chunk.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Sprite.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Text.h"
#include "ElkGameEngine/Objects/Components/Transform.h"

#include <ElkTools/Debug/Log.h>
#include <ElkTools/Debug/Assertion.h>
#include <ElkTools/Utils/Time.h>
#include <ElkTools/Utils/Benchmark.h>
#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/Date.h>
#include <ElkTools/Utils/ManagerLocator.h>
#include <ElkTools/Managers/AudioManager.h>

#include <ElkRendering/Managers/RenderingManager.h>
#include <ElkPhysics/Managers/PhysicsManager.h>

namespace ElkGameEngine
{
	namespace Managers
	{
		/*
		* The Engine is the main class of the program, it calls everyone methods (Create Window, call Renderer update, etc...)
		*/
		class ELK_GAMEENGINE_EXPORT EngineManager : public ElkAPI::IManager
		{
		public:
			EngineManager() : m_benchmark(m_enableRendering, m_enablePhysics) { EngineManager::Setup(); }
			~EngineManager() { EngineManager::Close(); }

			void Quit();
			bool IsRunning() const;

			void PreUpdate();
			void PostUpdate();

			void ToggleFramerateDisplay();
			void ToggleRendering();
			void TogglePhysics();

			bool InRenderingDistance(const glm::vec3& p_entityPosition);
			bool InFrustum(const glm::vec3& p_entityPosition);

			ElkTools::Utils::Benchmark& GetBenchmark() { return m_benchmark; }
			ElkTools::Utils::Profiler& GetProfiler() { return m_profiler; }

			void SetClearScreenCoefficient(float p_value) { m_clearScreenCoeff = p_value; }

			void ParseScene();
			void ParseGameObject(Objects::GameObject* p_gameObject);
			void Consider(Objects::GameObject* p_toConsider);
			void Unconsider(Objects::GameObject* p_toUnconsider);

		private:
			void Setup() override;
			void Close() override;

			void ApplyCulling();
			void UpdateCullingPlanes();
			void FindSpritesToDraw();
			void UpdatePhysics();
			void RenderScene();
			void HandleRendering();
			void HandleInputs();
			void ShowFramerate();
			void HandleSystemInputs();

			std::string BooleanToToggleState(bool p_state);

			ElkTools::Utils::Benchmark m_benchmark;
			ElkTools::Utils::Profiler m_profiler;
			ElkTools::Utils::Time m_time;

			std::unique_ptr<WindowManager> m_windowManager;
			std::unique_ptr<SceneManager> m_sceneManager;
			std::unique_ptr<InputManager> m_inputManager;
			std::unique_ptr<ElkRendering::Managers::RenderingManager> m_renderingManager;
			std::unique_ptr<ElkPhysics::Managers::PhysicsManager> m_physicsManager;
			std::unique_ptr<ElkTools::Managers::AudioManager> m_audioManager;

			Objects::GameObject* m_camera = nullptr;
			Objects::GameObject* m_UICamera = nullptr;
			Objects::GameObject* m_audioListener = nullptr;
			glm::vec3 m_cameraPosition;

			/* Considered with scene parsing */
			std::map<uint64_t, std::reference_wrapper<Objects::GameObject>> m_physicsEntities;
			std::map<uint64_t, std::reference_wrapper<Objects::GameObject>> m_chunks;
			std::map<uint64_t, std::reference_wrapper<Objects::GameObject>> m_sprites;
			std::map<uint64_t, std::reference_wrapper<Objects::GameObject>> m_texts;

			std::multimap<uint16_t, std::reference_wrapper<Objects::GameObject>, std::greater<uint16_t>> m_spritesToDraw;
			std::multimap<uint16_t, std::reference_wrapper<Objects::GameObject>, std::greater<uint16_t>> m_chunksToDraw;

			bool m_quit;

			bool m_showFramerate = false;
			double m_timer = 0.0f;

			float m_realFPS;

			float m_renderingTimer = 0.f;
			float m_renderingFramerate;

			float m_renderingDistance;

			bool m_enableRendering = true;
			bool m_enablePhysics = true;

			float m_clearScreenCoeff;

			GLfloat m_leftPlane[4];
			GLfloat m_rightPlane[4];
			GLfloat m_topPlane[4];
			GLfloat m_bottomPlane[4];
			GLfloat m_nearPlane[4];
			GLfloat m_farPlane[4];
		};
	}
}
