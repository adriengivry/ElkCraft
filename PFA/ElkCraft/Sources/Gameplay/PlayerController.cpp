#include "stdafx.h"

#include "ElkCraft/Gameplay/PlayerController.h"

using namespace ElkTools::Utils;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Gameplay;

PlayerController::PlayerController(ElkCraft::Terrain::World* p_world)
	: m_inputManager(ManagerLocator::Get<ElkGameEngine::Managers::InputManager>()),
	m_audioManager(ManagerLocator::Get<ElkTools::Managers::AudioManager>()),
	m_camera(ManagerLocator::Get<ElkRendering::Managers::RenderingManager>().GetCamera()),
	m_world(p_world),
	m_movementMode(MovementMode::DEFAULT),
	m_mouseSensitivity(IniLoader::GetFloat("player.ini", "mouse_sensitivity")),
	m_jumpStrength(IniLoader::GetFloat("player.ini", "jump_strength")),
	m_firstMouse(true),
	m_lastX(0.f),
	m_lastY(0.f),
	m_transform(nullptr),
	m_rigidbody(nullptr),
	m_enablePositionLog(false),
	m_positionLogFrequency(IniLoader::GetFloat("player.ini", "position_log_frequency")),
	m_stepSoundFrequency(IniLoader::GetFloat("player.ini", "step_sound_frequency"))
{
	Setup();

	m_inputManager.LockCursor();
	m_cursorIsFree = false;
	m_lockMovement = false;
}

PlayerController::~PlayerController()
{
	m_inputManager.FreeCursor();
}

void PlayerController::Setup()
{
	m_runningDoubleTapTimer = 0.0f;
	m_positionLogTimer = 0.0f;
	m_firstRunningTap = false;
	m_running = false;
	m_stepSoundTimer = 0.0f;
	m_wasSwimming = false;
	m_swimDuration = 0.0f;

	m_defaultModeParameters.walkSpeed = IniLoader::GetFloat("player.ini", "default_walk_speed");
	m_defaultModeParameters.runSpeed = IniLoader::GetFloat("player.ini", "default_run_speed");

	m_flyModeParameters.walkSpeed = IniLoader::GetFloat("player.ini", "fly_walk_speed");
	m_flyModeParameters.runSpeed = IniLoader::GetFloat("player.ini", "fly_run_speed");
}

void PlayerController::ToggleMovementMode()
{
	m_movementMode = m_movementMode == MovementMode::FLYING ? MovementMode::DEFAULT : MovementMode::FLYING;

	switch (m_movementMode)
	{
	case MovementMode::DEFAULT:
		m_rigidbody->SetKinematic(false);
		break;

	case MovementMode::FLYING:
		m_rigidbody->SetKinematic(true);
		break;
	}
}

void PlayerController::Update()
{
	if (!m_rigidbody)
		m_rigidbody = m_owner->GetComponent<Rigidbody>();

	if (!m_transform)
		m_transform = m_owner->transform;

	if (!m_lockMovement)
	{
		HandleRun();
		HandleCamera();
		HandleMovement();
		HandleModeSwitch();
	}

	HandleCursorLock();
	LogPosition();
	HandleStepSounds();
	HandleEnvironmentSounds();

	CalculateNearestBlocks();
}

void PlayerController::HandleRun()
{
	if (!m_canRun)
	{
		m_running = false;
		return;
	}

	if (m_running)
	{
		if (m_movementMode == MovementMode::DEFAULT)
		{
			if (abs(m_rigidbody->GetVelocity().x) + abs(m_rigidbody->GetVelocity().z) < m_defaultModeParameters.walkSpeed)
			{
				m_running = false;
			}
		}
		else
		{
			if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_W, ElkGameEngine::Managers::InputManager::KeyState::KEY_UP))
			{
				m_running = false;
			}
		}
	}

	if (m_firstRunningTap)
	{
		m_runningDoubleTapTimer += Time::GetDeltaTime();
		if (m_runningDoubleTapTimer >= IniLoader::GetFloat("player.ini", "double_tap_timer_to_run"))
		{
			m_runningDoubleTapTimer = 0.0f;
			m_firstRunningTap = false;
		}
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_LEFT_CONTROL, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_running = true;

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_W, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		if (!m_firstRunningTap)
		{
			m_firstRunningTap = true;
			m_runningDoubleTapTimer = 0.0f;
		}
		else
		{
			m_running = true;
		}
	}
}

void PlayerController::HandleMovement()
{
	m_rigidbody->SetVelocity(m_movementMode == MovementMode::DEFAULT ? CalculateDefaultMovement() : CalculateFlyMovement());
}

glm::vec3 ElkCraft::Gameplay::PlayerController::CalculateDefaultMovement()
{
	glm::vec3 fakeForward = CalculateFakeForward();
	float currentSpeed = m_running && !IsSwimming() ? m_defaultModeParameters.runSpeed : m_defaultModeParameters.walkSpeed;
	float jumpStrength = m_jumpStrength;
	glm::vec3 velocity = glm::vec3(0.f, m_rigidbody->GetVelocity().y, 0.f);

	if (IsHavingFeetInLiquid())
	{
		currentSpeed *= 0.75f;
	}

	if (IsSwimming())
	{
		currentSpeed *= 0.75f;
		m_swimDuration += ElkTools::Utils::Time::GetDeltaTime();
		m_wasSwimming = true;
		m_rigidbody->SetGravityScale(0.02f);
		jumpStrength *= 0.5f;
		if (velocity.y > 1.0f)
			velocity.y = 1.0f;

		if (velocity.y < -2.0f)
			velocity.y = -2.0f;
	}
	else
	{
		m_rigidbody->SetGravityScale(1.0f);
		m_swimDuration = 0.0f;
	}

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_A))
		velocity += -m_transform->GetRight() * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_D))
		velocity += m_transform->GetRight() * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_S))
		velocity += -fakeForward * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_W))
		velocity += fakeForward * currentSpeed;

	if (IsSwimming())
	{
		if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_SPACE) && m_swimDuration >= 0.3f)
		{
			m_rigidbody->SetGravityScale(0.f);
			velocity += glm::vec3(0.f, 1.f, 0.f) * jumpStrength * ElkTools::Utils::Time::GetDeltaTime();
		}
	}
	else
	{
		if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_SPACE) && (IsGrounded() && m_rigidbody->GetVelocity().y == 0.0f)|| IsSwimming() || m_wasSwimming)
			velocity += glm::vec3(0.f, 1.f, 0.f) * jumpStrength;

		m_wasSwimming = false;
	}

	return velocity;
}

glm::vec3 ElkCraft::Gameplay::PlayerController::CalculateFlyMovement()
{
	m_rigidbody->SetGravityScale(0.f);

	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);;
	float currentSpeed = m_running ? m_flyModeParameters.runSpeed : m_flyModeParameters.walkSpeed;
	glm::vec3 fakeForward = CalculateFakeForward();

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_A))
		velocity += -m_transform->GetRight() * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_D))
		velocity += m_transform->GetRight() * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_S))
		velocity += -fakeForward * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_W))
		velocity += fakeForward * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_LEFT_SHIFT))
		velocity += -glm::vec3(0.f, 1.f, 0.f) * currentSpeed;

	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_SPACE))
		velocity += glm::vec3(0.f, 1.f, 0.f) * currentSpeed;

	return velocity;
}

void PlayerController::HandleCamera()
{
	if (!m_cursorIsFree)
	{
		if (m_firstMouse)
		{
			m_lastX = m_inputManager.GetCursorPosition().x;
			m_lastY = m_inputManager.GetCursorPosition().y;
			m_firstMouse = false;
		}

		const float xOffset = (m_inputManager.GetCursorPosition().x - m_lastX) * m_mouseSensitivity;
		const float yOffset = (m_lastY - m_inputManager.GetCursorPosition().y) * m_mouseSensitivity;

		m_lastX = m_inputManager.GetCursorPosition().x;
		m_lastY = m_inputManager.GetCursorPosition().y;

		m_transform->Rotate(glm::vec3(yOffset, xOffset, 0.0f));
	}

	if (m_running)
	{
		float lerpTime = Time::GetDeltaTime() * 20.0f;
		m_camera.SetZoom(ElkTools::Utils::Math::Lerp(m_camera.GetZoom(), IniLoader::GetFloat("player.ini", "run_fov"), lerpTime > 1.0f ? 1.0f : lerpTime));
	}
	else
	{
		float lerpTime = Time::GetDeltaTime() * 20.0f;
		m_camera.SetZoom(ElkTools::Utils::Math::Lerp(m_camera.GetZoom(), IniLoader::GetFloat("player.ini", "walk_fov"), lerpTime > 1.0f ? 1.0f : lerpTime));
	}
}

void PlayerController::HandleCursorLock()
{
	if (m_inputManager.IsKeyDown(ElkGameEngine::Managers::InputManager::Key::KEY_LEFT_ALT))
	{
		m_inputManager.FreeCursor();
		m_firstMouse = true;
		m_cursorIsFree = true;
	}
	else
	{
		m_inputManager.LockCursor();
		m_cursorIsFree = false;
	}
}

void PlayerController::HandleModeSwitch()
{
	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_M, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		ToggleMovementMode();
}

void PlayerController::LogPosition()
{
	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_P, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_enablePositionLog = !m_enablePositionLog;

	if (m_enablePositionLog)
	{
		m_positionLogTimer += Time::GetDeltaTime();

		if (m_positionLogTimer >= m_positionLogFrequency)
		{
			m_positionLogTimer = 0.0f;

			std::string position = "(" + std::to_string(m_transform->GetPosition().x) + "|";
			position += std::to_string(m_transform->GetPosition().y) + "|";
			position += std::to_string(m_transform->GetPosition().z) + "|";

			ElkTools::Debug::Log::Process("Player Position : " + position);
		}
	}
}

void ElkCraft::Gameplay::PlayerController::HandleEnvironmentSounds()
{
	bool nearestBlockFromPlayerFeetChanged = m_previousNearestBlockFromFeet != m_nearestBlockFromFeet;
	m_previousNearestBlockFromFeet = m_nearestBlockFromFeet;
	if (IsHavingFeetInLiquid() && nearestBlockFromPlayerFeetChanged)
	{
		if (static_cast<BlocksInfo::BlockType>(m_nearestBlockFromFeet) == BlocksInfo::BlockType::WATER)
		{
			std::string randomSound = "";
			randomSound = m_audioManager.FindRandomSound("../Resources/Audio/Sounds/liquid/", "splash", ".ogg", 2);
			m_audioManager.Play3DSound(randomSound, m_owner->transform->GetPosition().x, m_owner->transform->GetPosition().y, m_owner->transform->GetPosition().z);
		}
	}
}

void PlayerController::HandleStepSounds()
{
	if (m_movementMode == MovementMode::DEFAULT && IsMoving() && !IsSwimming())
	{
		m_stepSoundTimer += ElkTools::Utils::Time::GetDeltaTime();

		if (IsGrounded() && m_stepSoundTimer > 1.0f / GetGroundVelocity() * m_stepSoundFrequency)
		{
			PlayStepSound();
			m_stepSoundTimer = 0.0f;
		}
	}
}

void PlayerController::PlayStepSound()
{
	std::string randomSound;
	std::string soundsFolder;
	float volume = 0.5f;
	float speed = 1.0f;

	if (static_cast<BlocksInfo::BlockType>(m_nearestBlockFromFeet) == BlocksInfo::BlockType::WATER)
	{
		soundsFolder = "../Resources/Audio/Sounds/liquid/";
		randomSound = m_audioManager.FindRandomSound(soundsFolder, "swim", ".ogg", 4);
		speed = ElkTools::Utils::Random::Generate(70, 100) * 0.01f;
		volume = ElkTools::Utils::Random::Generate(25, 75) * 0.01f;
	}
	else
	{
		soundsFolder = "../Resources/Audio/Sounds/step/";

		switch (BlocksInfo::GetBlockCategory(GetBlockUnderFeet()))
		{
		case BlocksInfo::Category::GRASS:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "grass", ".ogg", 6);
			break;

		case BlocksInfo::Category::GRAVEL:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "gravel", ".ogg", 4);
			break;

		case BlocksInfo::Category::SAND:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "sand", ".ogg", 5);
			break;

		case BlocksInfo::Category::GLASS:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "cloth", ".ogg", 4);
			break;

		case BlocksInfo::Category::STONE:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "stone", ".ogg", 6);
			break;

		case BlocksInfo::Category::WOOD:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "wood", ".ogg", 6);
			break;
		}

	}

	glm::vec3 feetPosition = GetFeetPosition();
	m_audioManager.Play3DSound(randomSound, feetPosition.x, feetPosition.y, feetPosition.z, false, volume, speed);
}

void ElkCraft::Gameplay::PlayerController::CalculateNearestBlocks()
{
	glm::vec3 headPosition = GetHeadPosition();
	glm::vec3 feetPosition = GetFeetPosition();
	glm::vec3 underFeetPosition = GetBlockUnderFeetPosition();

	m_nearestBlockFromHead = m_world->GetBlockID(headPosition.x, headPosition.y, headPosition.z);
	m_nearestBlockFromFeet = m_world->GetBlockID(feetPosition.x, feetPosition.y, feetPosition.z);
	m_blockUnderFeet = m_world->GetBlockID(underFeetPosition.x, underFeetPosition.y, underFeetPosition.z);
}

glm::vec3 ElkCraft::Gameplay::PlayerController::CalculateFakeForward()
{
	return glm::cross(-m_owner->transform->GetRight(), glm::vec3(0.f, 1.f, 0.f));
}

bool PlayerController::IsMoving()
{
	return (abs(m_rigidbody->GetVelocity().x) + abs(m_rigidbody->GetVelocity().z)) / 2.f > 1.0f;
}

bool PlayerController::IsGrounded()
{
	glm::vec3 underFeetPosition = GetBlockUnderFeetPosition();

	return BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(+0.125f, 0.f, 0.f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(-0.125f, 0.f, 0.f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(0.f, 0.f, +0.125f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(0.f, 0.f, -0.125f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(+0.125f, 0.f, +0.125f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(+0.125f, 0.f, -0.125f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(-0.125f, 0.f, +0.125f)))
		|| BlocksInfo::IsCollidable(m_world->GetBlockID(underFeetPosition + glm::vec3(-0.125f, 0.f, -0.125f)));
}

float PlayerController::GetGroundVelocity()
{
	return abs(m_rigidbody->GetVelocity().x) + abs(m_rigidbody->GetVelocity().z);
}

void ElkCraft::Gameplay::PlayerController::SetCanRun(bool p_value)
{
	m_canRun = p_value;
}

void ElkCraft::Gameplay::PlayerController::LockMovement()
{
	m_firstMouse = true;

	switch (m_movementMode)
	{
	case MovementMode::DEFAULT:
		m_rigidbody->SetVelocity(m_rigidbody->GetVelocity() * glm::vec3(0.f, 1.f, 0.f));
		break;

	case MovementMode::FLYING:
		m_rigidbody->SetVelocity(m_rigidbody->GetVelocity() * glm::vec3(0.f, 0.f, 0.f));
		break;
	}
	
	m_lockMovement = true;
}

void ElkCraft::Gameplay::PlayerController::UnlockMovement()
{
	m_lockMovement = false;
}

bool PlayerController::IsSwimming()
{
	return BlocksInfo::IsLiquid(m_nearestBlockFromHead);
}

bool PlayerController::IsHavingFeetInLiquid()
{
	return BlocksInfo::IsLiquid(m_nearestBlockFromFeet);
}

glm::vec3 ElkCraft::Gameplay::PlayerController::GetHeadPosition()
{
	return m_transform->GetPosition();
}

glm::vec3 ElkCraft::Gameplay::PlayerController::GetFeetPosition()
{
	return m_transform->GetPosition() - glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 ElkCraft::Gameplay::PlayerController::GetBlockUnderFeetPosition()
{
	return m_transform->GetPosition() - glm::vec3(0.f, 1.75f, 0.f);
}

uint8_t ElkCraft::Gameplay::PlayerController::GetBlockNearFromHead()
{
	return m_nearestBlockFromHead;
}

uint8_t ElkCraft::Gameplay::PlayerController::GetBlockNearFromFeet()
{
	return m_nearestBlockFromFeet;
}

uint8_t PlayerController::GetBlockUnderFeet()
{
	return m_blockUnderFeet;
}
