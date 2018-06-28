#include "stdafx.h"

#include <ElkRendering/Context/Driver.h>

#include "ElkGameEngine/Managers/WindowManager.h"

ElkGameEngine::Managers::WindowManager::WindowManager()
{
	m_device = std::make_unique<Context::Device>();
	m_driver = std::make_unique<ElkRendering::Context::Driver>();

	this->Setup();
}

ElkGameEngine::Managers::WindowManager::~WindowManager()
{
	WindowManager::Close();
}

void ElkGameEngine::Managers::WindowManager::Update() {}

void ElkGameEngine::Managers::WindowManager::Close() 
{
	m_driver->Close();
	m_device->Close();
}

void ElkGameEngine::Managers::WindowManager::Setup()
{
	m_width		= ElkTools::Utils::IniLoader::GetInt("window.ini", "width");
	m_height	= ElkTools::Utils::IniLoader::GetInt("window.ini", "height");
	m_title		= ElkTools::Utils::IniLoader::GetString("window.ini", "title");

	SendDataToDevice();

	m_device->Setup();
	m_driver->Setup();
}

void ElkGameEngine::Managers::WindowManager::SendDataToDevice() const
{
	m_device->SetWidth(m_width);
	m_device->SetHeight(m_height);
	m_device->SetTitle(m_title);
}

ElkGameEngine::Context::Device& ElkGameEngine::Managers::WindowManager::GetDevice() const { return *m_device.get();  }
ElkRendering::Context::Driver& ElkGameEngine::Managers::WindowManager::GetDriver() const { return *m_driver.get(); }
bool ElkGameEngine::Managers::WindowManager::IsOpen() const { return m_device->IsActive(); }
