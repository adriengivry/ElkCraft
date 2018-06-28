#include "stdafx.h"

#include "ElkCraft/System/GameStateManager.h"

ElkCraft::System::GameStateManager::GameState	ElkCraft::System::GameStateManager::__CURRENT_STATE;
bool											ElkCraft::System::GameStateManager::__GAME_STATE_CHANGED;

void ElkCraft::System::GameStateManager::SetCurrentState(GameState p_state)
{
	__CURRENT_STATE = p_state;
	__GAME_STATE_CHANGED = true;
}

ElkCraft::System::GameStateManager::GameState ElkCraft::System::GameStateManager::GetCurrentState()
{
	return __CURRENT_STATE;
}

bool ElkCraft::System::GameStateManager::IsGameStateChanged()
{
	return __GAME_STATE_CHANGED;
}

void ElkCraft::System::GameStateManager::ResetGameStateChange()
{
	__GAME_STATE_CHANGED = false;
}
