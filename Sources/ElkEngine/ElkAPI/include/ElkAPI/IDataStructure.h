#pragma once

namespace ElkAPI
{
	/*
	* The IDataStructure is an interface that explain how a data container must be constructed
	*/
	struct IDataStructure
	{
		virtual void Setup() = 0;
	};
}