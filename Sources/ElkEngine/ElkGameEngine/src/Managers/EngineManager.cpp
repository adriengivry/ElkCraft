#include "stdafx.h"

#include "ElkGameEngine/Managers/EngineManager.h"

using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkGameEngine::Managers;
using namespace ElkRendering::Managers;
using namespace ElkPhysics::Managers;
using namespace ElkTools::Utils;
using namespace ElkTools::Managers;

void ElkGameEngine::Managers::EngineManager::Close() {}

void ElkGameEngine::Managers::EngineManager::Quit()
{
	m_quit = true;
}

bool ElkGameEngine::Managers::EngineManager::IsRunning() const
{
	return m_quit ? false : ManagerLocator::Get<WindowManager>().GetDriver().IsActive() && ManagerLocator::Get<WindowManager>().GetDevice().IsActive();
}

void ElkGameEngine::Managers::EngineManager::PreUpdate()
{
	PROFILER_SPY("EngineManager::PreUpdate");

	m_time.Update();
	m_benchmark.Update();
	m_renderingTimer += Time::GetDeltaTime();

	if (m_showFramerate)
		ShowFramerate();

	if (m_enablePhysics)
		UpdatePhysics();

	if (m_camera)
	{
		m_renderingManager->SetCamera(m_camera->transform->GetPosition(), m_camera->transform->GetForward(), m_camera->transform->GetRight(), m_camera->transform->GetUp());
		m_cameraPosition = m_camera->transform->GetPosition();
	}

	if (m_UICamera)
	{
		m_renderingManager->SetUICamera(m_UICamera->transform->GetPosition(), m_UICamera->transform->GetForward(), m_UICamera->transform->GetRight(), m_UICamera->transform->GetUp());
	}

	if (m_audioListener)
	{
		glm::vec3 position = m_audioListener->transform->GetPosition();
		glm::vec3 forward = m_audioListener->transform->GetForward();
		m_audioManager->SetListenerPosition(position.x, position.y, position.z, forward.x, forward.y, forward.z);
	}
}

void ElkGameEngine::Managers::EngineManager::PostUpdate()
{
	{
		PROFILER_SPY("EngineManager::PostUpdate");

		if (m_profiler.__RUNNING || m_enableRendering && m_renderingTimer >= 1.0f / m_renderingFramerate)
		{
			m_renderingTimer = 0.0f;
			HandleRendering();
		}

		HandleInputs();
	}
	m_profiler.Log();
}

void ElkGameEngine::Managers::EngineManager::ApplyCulling()
{
	PROFILER_SPY("EngineManager::ApplyCulling");

	m_chunksToDraw.clear();

	UpdateCullingPlanes();

	for (auto& chunk : m_chunks)
	{
		glm::vec3 position = chunk.second.get().transform->GetPosition();
		if (InRenderingDistance(position) && InFrustum(position))
			m_chunksToDraw.insert(std::pair<uint16_t, std::reference_wrapper<Objects::GameObject>>(static_cast<uint16_t>(glm::distance(m_cameraPosition, position)), chunk.second));
	}
}

void ElkGameEngine::Managers::EngineManager::UpdateCullingPlanes()
{
	glm::mat4 proj = m_renderingManager->CalculateProjectionperspective();
	glm::mat4 modl = m_renderingManager->CalculateView();
	glm::mat4 mvp = proj * modl;
	GLfloat planesLength[6];

	/* Left plane */
	m_leftPlane[0] = mvp[0][3] + mvp[0][0];
	m_leftPlane[1] = mvp[1][3] + mvp[1][0];
	m_leftPlane[2] = mvp[2][3] + mvp[2][0];
	m_leftPlane[3] = mvp[3][3] + mvp[3][0];
	planesLength[0] = sqrtf(m_leftPlane[0] * m_leftPlane[0] + m_leftPlane[1] * m_leftPlane[1] + m_leftPlane[2] * m_leftPlane[2]);

	/* Right plane */
	m_rightPlane[0] = mvp[0][3] - mvp[0][0];
	m_rightPlane[1] = mvp[1][3] - mvp[1][0];
	m_rightPlane[2] = mvp[2][3] - mvp[2][0];
	m_rightPlane[3] = mvp[3][3] - mvp[3][0];
	planesLength[1] = sqrtf(m_rightPlane[0] * m_rightPlane[0] + m_rightPlane[1] * m_rightPlane[1] + m_rightPlane[2] * m_rightPlane[2]);

	/* Top plane */
	m_topPlane[0] = mvp[0][3] - mvp[0][1];
	m_topPlane[1] = mvp[1][3] - mvp[1][1];
	m_topPlane[2] = mvp[2][3] - mvp[2][1];
	m_topPlane[3] = mvp[3][3] - mvp[3][1];
	planesLength[2] = sqrtf(m_topPlane[0] * m_topPlane[0] + m_topPlane[1] * m_topPlane[1] + m_topPlane[2] * m_topPlane[2]);

	/* Bottom plane */
	m_bottomPlane[0] = mvp[0][3] + mvp[0][1];
	m_bottomPlane[1] = mvp[1][3] + mvp[1][1];
	m_bottomPlane[2] = mvp[2][3] + mvp[2][1];
	m_bottomPlane[3] = mvp[3][3] + mvp[3][1];
	planesLength[3] = sqrtf(m_bottomPlane[0] * m_bottomPlane[0] + m_bottomPlane[1] * m_bottomPlane[1] + m_bottomPlane[2] * m_bottomPlane[2]);

	/* Near plane */
	m_nearPlane[0] = mvp[0][3] + mvp[0][2];
	m_nearPlane[1] = mvp[1][3] + mvp[1][2];
	m_nearPlane[2] = mvp[2][3] + mvp[2][2];
	m_nearPlane[3] = mvp[3][3] + mvp[3][2];
	planesLength[4] = sqrtf(m_nearPlane[0] * m_nearPlane[0] + m_nearPlane[1] * m_nearPlane[1] + m_nearPlane[2] * m_nearPlane[2]);

	/* Far plane */
	m_farPlane[0] = mvp[0][3] - mvp[0][2];
	m_farPlane[1] = mvp[1][3] - mvp[1][2];
	m_farPlane[2] = mvp[2][3] - mvp[2][2];
	m_farPlane[3] = mvp[3][3] - mvp[3][2];
	planesLength[5] = sqrtf(m_farPlane[0] * m_farPlane[0] + m_farPlane[1] * m_farPlane[1] + m_farPlane[2] * m_farPlane[2]);

	
	for (int i = 0; i < 4; ++i)
	{
		m_leftPlane[i]		/= planesLength[0];
		m_rightPlane[i]		/= planesLength[1];
		m_topPlane[i]		/= planesLength[2];
		m_bottomPlane[i]	/= planesLength[3];
		m_nearPlane[i]		/= planesLength[4];
		m_farPlane[i]		/= planesLength[5];
	}
	
}

void ElkGameEngine::Managers::EngineManager::FindSpritesToDraw()
{
	m_spritesToDraw.clear();

	for (auto& sprite : m_sprites)
	{
		auto spriteComp = sprite.second.get().GetComponent<Sprite>();

		if (spriteComp->IsActive())
			m_spritesToDraw.insert(std::pair<uint16_t, std::reference_wrapper<Objects::GameObject>>(spriteComp->GetRenderLayer(), sprite.second));
	}
}

void ElkGameEngine::Managers::EngineManager::UpdatePhysics()
{
	PROFILER_SPY("EngineManager::UpdatePhysics");

	ManagerLocator::Get<PhysicsManager>().ClearPhysicsEntities();

	for (auto& entity : m_physicsEntities)
	{
		auto transform = entity.second.get().transform;
		auto rigidbody = entity.second.get().GetComponent<Rigidbody>();

		if (rigidbody->IsActive())
		{
			ManagerLocator::Get<PhysicsManager>().AddEntity(transform->GetTransformData(), rigidbody->GetRigidbodyData());
		}
	}

	ManagerLocator::Get<PhysicsManager>().ApplyPhysics();
}

void ElkGameEngine::Managers::EngineManager::RenderScene()
{
	PROFILER_SPY("EngineManager::RenderScene");

	auto& renderingManager = ManagerLocator::Get<RenderingManager>();

	/* Draw Chunks */
	renderingManager.PrepareChunkDraw();
	for (auto& chunk : m_chunksToDraw)
		renderingManager.DrawChunk(chunk.second.get().GetComponent<Chunk>()->GetRenderChunk(), chunk.second.get().transform->GetPosition());
	renderingManager.EndChunkDraw();

	/* Draw Sprites */
	renderingManager.PrepareSpriteDraw();
	for (auto& sprite : m_spritesToDraw)
	{
		Sprite& spriteComponent = *sprite.second.get().GetComponent<Sprite>();
		Transform& transform = *sprite.second.get().transform;
		renderingManager.DrawSprite(spriteComponent.GetRenderSprite(), transform.GetPosition(), transform.GetScale(), spriteComponent.GetColor());
	}
	m_spritesToDraw.clear();
	renderingManager.EndSpriteDraw();

	/* Draw Texts */
	renderingManager.PrepareStringDraw();
	for (auto& text : m_texts)
	{
		Text& textComponent = *text.second.get().GetComponent<Text>();
		if (textComponent.IsActive())
		{
			Transform& transform = *text.second.get().transform;

			glm::vec3 position = transform.GetPosition() + textComponent.GetPositionOffset();
			glm::vec4 color = textComponent.GetFontColor();
			float spacing = textComponent.GetSpacing();
			glm::vec3 scale;

			switch (textComponent.GetScalingMode())
			{
			case Text::ScalingMode::GAMEOBJECT_TRANSFORM_AND_FONT_SIZE:
				scale = textComponent.GetFontSize() * transform.GetScale();
				break;

			case Text::ScalingMode::FONT_SIZE_ONLY:
				scale = glm::vec3(1.f) * textComponent.GetFontSize();
				break;

			case Text::ScalingMode::GAMEOBJECT_TRANSFORM_ONY:
				scale = transform.GetScale();
				break;

			case Text::ScalingMode::NO_SCALE:
				scale = glm::vec3(1.f);
				break;
			}

			renderingManager.DrawString(textComponent.GetRenderText(), position, scale, spacing, color);
		}
	}
	renderingManager.EndStringDraw();
}

void ElkGameEngine::Managers::EngineManager::ParseScene()
{
	PROFILER_SPY("EngineManager::ParseScene");

	m_camera = nullptr;
	m_UICamera = nullptr;
	m_audioListener = nullptr;

	m_chunks.clear();
	m_sprites.clear();
	m_texts.clear();
	m_physicsEntities.clear();

	for (auto current : m_sceneManager->GetCurrentScene().GetGameObjects())
	{
		ParseGameObject(current.second.get());
	}
}

void ElkGameEngine::Managers::EngineManager::ParseGameObject(Objects::GameObject* p_gameObject)
{
	auto transform = p_gameObject->GetComponent<Transform>();
	auto chunk = p_gameObject->GetComponent<Chunk>();
	auto camera = p_gameObject->GetComponent<Camera>();
	auto audioListener = p_gameObject->GetComponent<AudioListener>();
	auto UICamera = p_gameObject->GetComponent<Behaviours::UICamera>();
	auto rigidbody = p_gameObject->GetComponent<Rigidbody>();
	auto sprite = p_gameObject->GetComponent<Sprite>();
	auto text = p_gameObject->GetComponent<Text>();

	if (camera)
		m_camera = p_gameObject;

	if (UICamera)
		m_UICamera = p_gameObject;

	if (audioListener)
		m_audioListener = p_gameObject;

	if (chunk)
		m_chunks.insert(std::pair<uint64_t, std::reference_wrapper<Objects::GameObject>>(p_gameObject->GetInstanceID(), *p_gameObject));

	if (rigidbody)
		m_physicsEntities.insert(std::pair<uint64_t, std::reference_wrapper<Objects::GameObject>>(p_gameObject->GetInstanceID(), *p_gameObject));

	if (sprite)
		m_sprites.insert(std::pair<uint64_t, std::reference_wrapper<Objects::GameObject>>(p_gameObject->GetInstanceID(), *p_gameObject));

	if (text)
		m_texts.insert(std::pair<uint64_t, std::reference_wrapper<Objects::GameObject>>(p_gameObject->GetInstanceID(), *p_gameObject));
}

void ElkGameEngine::Managers::EngineManager::Consider(Objects::GameObject* p_toConsider)
{
	ParseGameObject(p_toConsider);
}

void ElkGameEngine::Managers::EngineManager::Unconsider(Objects::GameObject* p_toUnconsider)
{
	uint64_t instanceID = p_toUnconsider->GetInstanceID();

	if (m_chunks.find(instanceID) != m_chunks.end())
		m_chunks.erase(instanceID);

	if (m_physicsEntities.find(instanceID) != m_physicsEntities.end())
		m_physicsEntities.erase(instanceID);

	if (m_sprites.find(instanceID) != m_sprites.end())
		m_sprites.erase(instanceID);
}

void ElkGameEngine::Managers::EngineManager::HandleRendering()
{
	PROFILER_SPY("EngineManager::HandleRendering");

	m_renderingTimer = 0.0f;
	ApplyCulling();
	FindSpritesToDraw();
	RenderScene();
	ManagerLocator::Get<WindowManager>().GetDevice().SwapBuffers();
	ManagerLocator::Get<WindowManager>().GetDriver().ClearScreen(m_clearScreenCoeff);
}

void ElkGameEngine::Managers::EngineManager::HandleInputs()
{
	PROFILER_SPY("EngineManager::HandleInputs");

	HandleSystemInputs();
	ManagerLocator::Get<InputManager>().Update();
	Context::Device::PollEvents();
}

void ElkGameEngine::Managers::EngineManager::ShowFramerate()
{
	m_timer += ElkTools::Utils::Time::GetDeltaTime();
	if (m_timer >= 1.0f)
	{
		ElkTools::Debug::Log::Process("Framerate : " + std::to_string(std::min(ElkTools::Utils::Time::GetFramerate(), m_renderingFramerate)) + " FPS");
		m_timer = 0.0f;
	}
}

void ElkGameEngine::Managers::EngineManager::HandleSystemInputs()
{
	/* Toggle Framerate with F1 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F1, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		ToggleFramerateDisplay();

	/* Start benchmarking with F2 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F2, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_benchmark.Start();

	/* Toggle profiler with F3 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F3, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_profiler.Toggle();

	/* Toggle profiler with F4 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F4, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_profiler.Execute();

	/* Recompile shaders with F5 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F5, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		ManagerLocator::Get<ShaderManager>().RecompileShaders();

	/* Default rendering with F6 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F6, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_renderingManager->SetRenderMode(ElkRendering::Managers::RenderingManager::RenderMode::DEFAULT);

	/* Wireframe rendering with F7 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F7, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_renderingManager->SetRenderMode(ElkRendering::Managers::RenderingManager::RenderMode::WIREFRAME);

	/* Normals rendering with F8 */
	if (m_inputManager->IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_F8, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_renderingManager->SetRenderMode(ElkRendering::Managers::RenderingManager::RenderMode::NORMALS);
}

std::string ElkGameEngine::Managers::EngineManager::BooleanToToggleState(bool p_state)
{
	if (p_state)
		return "ON";
	else
		return "OFF";
}

void ElkGameEngine::Managers::EngineManager::ToggleFramerateDisplay()
{
	m_showFramerate = !m_showFramerate;
	m_timer = 0.0f;
	ElkTools::Debug::Log::Process("Framerate counter : " + BooleanToToggleState(m_showFramerate), ElkTools::Debug::Log::LogLevel::LOG_INFO);
}

void ElkGameEngine::Managers::EngineManager::ToggleRendering()
{
	m_enableRendering = !m_enableRendering;
	ElkTools::Debug::Log::Process("Rendering : " + BooleanToToggleState(m_enableRendering), ElkTools::Debug::Log::LogLevel::LOG_INFO);
}

void ElkGameEngine::Managers::EngineManager::TogglePhysics()
{
	m_enablePhysics = !m_enablePhysics;
	ElkTools::Debug::Log::Process("Physics : " + BooleanToToggleState(m_enablePhysics), ElkTools::Debug::Log::LogLevel::LOG_INFO);
}

bool ElkGameEngine::Managers::EngineManager::InRenderingDistance(const glm::vec3 & p_entityPosition)
{
	glm::vec3 p1(p_entityPosition.x, 0.0f, p_entityPosition.z);
	glm::vec3 p2(m_cameraPosition.x, 0.0f, m_cameraPosition.z);

	return (glm::distance(p1, p2) < m_renderingDistance);
}

bool ElkGameEngine::Managers::EngineManager::InFrustum(const glm::vec3 & p_entityPosition)
{
	float radius = 13.9f;

	if (m_leftPlane[0] * p_entityPosition.x + m_leftPlane[1] * p_entityPosition.y + m_leftPlane[2] * p_entityPosition.z + m_leftPlane[3] <= -radius)
		return false;

	if (m_rightPlane[0] * p_entityPosition.x + m_rightPlane[1] * p_entityPosition.y + m_rightPlane[2] * p_entityPosition.z + m_rightPlane[3] <= -radius)
		return false;

	if (m_topPlane[0] * p_entityPosition.x + m_topPlane[1] * p_entityPosition.y + m_topPlane[2] * p_entityPosition.z + m_topPlane[3] <= -radius)
		return false;
 
	if (m_bottomPlane[0] * p_entityPosition.x + m_bottomPlane[1] * p_entityPosition.y + m_bottomPlane[2] * p_entityPosition.z + m_bottomPlane[3] <= -radius)
		return false;

	if (m_nearPlane[0] * p_entityPosition.x + m_nearPlane[1] * p_entityPosition.y + m_nearPlane[2] * p_entityPosition.z + m_nearPlane[3] <= -radius)
		return false;

	if (m_farPlane[0] * p_entityPosition.x + m_farPlane[1] * p_entityPosition.y + m_farPlane[2] * p_entityPosition.z + m_farPlane[3] <= -radius)
		return false;

	return true;
}

void ElkGameEngine::Managers::EngineManager::Setup()
{
	m_quit = false;
	m_clearScreenCoeff = 1.f;
	m_renderingDistance = ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "rendering_distance");
	m_renderingFramerate = ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "max_framerate");

	m_windowManager = std::make_unique<WindowManager>();
	m_sceneManager = std::make_unique<SceneManager>();
	m_inputManager = std::make_unique<InputManager>();
	m_renderingManager = std::make_unique<RenderingManager>(m_windowManager->GetWidth(), m_windowManager->GetHeight());
	m_physicsManager = std::make_unique<PhysicsManager>();
	m_audioManager = std::make_unique<AudioManager>();

	ManagerLocator::Provide<WindowManager>(*m_windowManager);
	ManagerLocator::Provide<SceneManager>(*m_sceneManager);
	ManagerLocator::Provide<InputManager>(*m_inputManager);
	ManagerLocator::Provide<RenderingManager>(*m_renderingManager);
	ManagerLocator::Provide<PhysicsManager>(*m_physicsManager);
	ManagerLocator::Provide<AudioManager>(*m_audioManager);
	
	ElkTools::Debug::Log::Process("Engine setup completed", ElkTools::Debug::Log::LogLevel::LOG_INFO);
}
