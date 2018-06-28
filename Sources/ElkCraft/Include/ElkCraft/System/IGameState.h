#pragma once

namespace ElkCraft::System
{
	class IGameState
	{
	public:
		virtual void Update() = 0;
		virtual ~IGameState() = default;
	};
}