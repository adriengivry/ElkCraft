#pragma once

#include <string>
#include <memory>

#include <ElkAPI/IManager.h>
#include <ElkTools/Utils/IniLoader.h>
#include <ElkRendering/Context/Driver.h>

#include "ElkGameEngine/Context/Device.h"

namespace ElkGameEngine
{
	namespace Managers
	{
		/*
		* Window class manage the Device and Driver creation
		*/
		class WindowManager : public ElkAPI::IManager
		{
		public:
			explicit WindowManager();
			~WindowManager();

			void Setup() override;
			void Close() override;

			void Update();

			void SendDataToDevice() const;

			Context::Device& GetDevice() const;
			ElkRendering::Context::Driver& GetDriver() const;
			bool IsOpen() const;

			uint16_t GetWidth() { return static_cast<uint16_t>(m_width); }
			uint16_t GetHeight() { return static_cast<uint16_t>(m_height); }

		private:
			int												m_width;
			int												m_height;
			std::string										m_title;
			std::unique_ptr<Context::Device>				m_device;
			std::unique_ptr<ElkRendering::Context::Driver>	m_driver;
		};
	}
}
