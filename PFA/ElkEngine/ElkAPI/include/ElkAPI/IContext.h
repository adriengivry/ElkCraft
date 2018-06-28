#pragma once

namespace ElkAPI
{
	/*
	* Context interface to indicate how to implement a Context-based class
	*/
	class IContext
	{
	public:
		virtual ~IContext() = default;
		virtual void Setup() = 0;
		virtual void Close() = 0;
		virtual bool IsActive() = 0;
	};
}