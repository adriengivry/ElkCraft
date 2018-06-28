#pragma once

namespace ElkAPI
{
	/*
	* IManager is an interface that provide directive to every childs
	* A manager is a class that can be setup and closed during runtime, without destructing it.
	* It can handle things, such as events, rendering, etc...
	*/
	class IManager
	{
	public:
		virtual ~IManager() = default;

		virtual void Setup() = 0;
		virtual void Close() = 0;
	};
}
