#pragma once

namespace ElkCraft::System
{
	class GameStateManager
	{
	public:
		enum class GameState
		{
			PLAY_STATE,
			CONTINUE_STATE,
			MENU_STATE,
			EXIT_STATE
		};

		static void SetCurrentState(GameState p_state);
		static GameState GetCurrentState();
		static bool IsGameStateChanged();
		static void ResetGameStateChange();

	private:
		static GameState __CURRENT_STATE;
		static bool __GAME_STATE_CHANGED;
	};
}