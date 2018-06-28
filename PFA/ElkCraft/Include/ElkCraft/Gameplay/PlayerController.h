#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Terrain/World.h"

namespace ElkCraft::Gameplay
{
	class PlayerController : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		struct MovementParameters
		{
			float walkSpeed;
			float runSpeed;
		};

		enum class MovementMode
		{
			DEFAULT,
			FLYING,
		};

		PlayerController(ElkCraft::Terrain::World* p_world);
		~PlayerController();

		void Setup();
		void ToggleMovementMode();

		void Update() override;
		void SetMovementMode(const MovementMode& p_mode) { m_movementMode = p_mode; }

		void HandleRun();
		void HandleMovement();
		glm::vec3 CalculateDefaultMovement();
		glm::vec3 CalculateFlyMovement();
		void HandleCamera();
		void HandleCursorLock();
		void HandleModeSwitch();
		void LogPosition();
		void HandleEnvironmentSounds();
		void HandleStepSounds();
		void PlayStepSound();

		void CalculateNearestBlocks();
		glm::vec3 CalculateFakeForward();

		bool IsMoving();
		bool IsGrounded();

		float GetGroundVelocity();

		void SetCanRun(bool p_value);

		void LockMovement();
		void UnlockMovement();

		bool IsSwimming();
		bool IsHavingFeetInLiquid();

		glm::vec3 GetHeadPosition();
		glm::vec3 GetFeetPosition();
		glm::vec3 GetBlockUnderFeetPosition();

		uint8_t GetBlockNearFromHead();
		uint8_t GetBlockNearFromFeet();
		uint8_t GetBlockUnderFeet();

	private:
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkTools::Managers::AudioManager& m_audioManager;
		ElkRendering::Data::CameraData& m_camera;

		ElkCraft::Terrain::World* m_world;

		std::shared_ptr<ElkGameEngine::Objects::Components::Transform> m_transform;
		std::shared_ptr<ElkGameEngine::Objects::Components::Behaviours::Rigidbody> m_rigidbody;

		uint8_t m_previousNearestBlockFromFeet;

		uint8_t m_nearestBlockFromHead;
		uint8_t m_nearestBlockFromFeet;
		uint8_t m_blockUnderFeet;

		MovementMode m_movementMode;
		MovementParameters m_defaultModeParameters;
		MovementParameters m_flyModeParameters;
		bool m_firstRunningTap;
		bool m_running;

		bool m_canRun;

		float m_runningDoubleTapTimer;
		float m_mouseSensitivity;
		float m_jumpStrength;
		bool m_wasSwimming;
		float m_swimDuration;

		bool m_cursorIsFree;
		bool m_firstMouse;
		float m_lastX;
		float m_lastY;

		bool m_lockMovement;

		bool m_enablePositionLog;
		float m_positionLogTimer;
		float m_positionLogFrequency;

		float m_stepSoundTimer;
		float m_stepSoundFrequency;
	};
}