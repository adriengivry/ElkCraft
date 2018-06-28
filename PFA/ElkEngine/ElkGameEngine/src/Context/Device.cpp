#include "stdafx.h"

#include <ElkTools/Debug/Assertion.h>

#include "ElkGameEngine/Context/Device.h"

using namespace ElkGameEngine;

void Context::ErrorCallback(int, const char* p_err_str)
{
	ElkTools::Debug::Assert(true, "GLFW Error: " + std::string(p_err_str), __FILE_INFO__);
}

void Context::KeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_action == GLFW_PRESS)
		Managers::InputManager::occuringKeyEvents[static_cast<Managers::InputManager::Key>(p_key)] = Managers::InputManager::KeyState::KEY_DOWN;

	if (p_action == GLFW_RELEASE)
		Managers::InputManager::occuringKeyEvents[static_cast<Managers::InputManager::Key>(p_key)] = Managers::InputManager::KeyState::KEY_UP;
}

void Context::MouseCallback(GLFWwindow* p_window, int p_mouseButton, int p_action, int p_mods)
{
	if (p_action == GLFW_PRESS)
		Managers::InputManager::occuringMouseEvents[static_cast<Managers::InputManager::MouseButton>(p_mouseButton)] = Managers::InputManager::MouseButtonState::MOUSE_DOWN;

	if (p_action == GLFW_RELEASE)
		Managers::InputManager::occuringMouseEvents[static_cast<Managers::InputManager::MouseButton>(p_mouseButton)] = Managers::InputManager::MouseButtonState::MOUSE_UP;
}

void Context::ScrollCallback(GLFWwindow * p_window, double p_xOffset, double p_yOffset)
{
	Managers::InputManager::scrollData.x = static_cast<float>(p_xOffset);
	Managers::InputManager::scrollData.y = static_cast<float>(p_yOffset);
}

Context::Device::Device()
{
	m_width = 0;
	m_height = 0;
}

void Context::Device::Setup()
{
	glfwSetErrorCallback(ErrorCallback);

	const int result = glfwInit();

	ElkTools::Debug::Assert(result == GL_FALSE, "GLFW Error!", __FILE_INFO__);

	//- Setting to core profile mode ------
	// In Core profile mode, we need to explicit a VAO (no need in Compatibility mode)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor* monitor = nullptr;
	int monitorCounter = 0;

	if (ElkTools::Utils::IniLoader::GetBool("window.ini", "fullscreen"))
	{
		int monitorToUse = ElkTools::Utils::IniLoader::GetInt("window.ini", "fullscreen_monitor");
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCounter);

		if (!monitorToUse || monitorToUse > monitorCounter)
			monitor = glfwGetPrimaryMonitor();
		else
		{
			monitor = monitors[monitorToUse - 1];
		}
	}

	m_glfwWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), monitor, nullptr);
	
	ElkTools::Debug::Assert(!m_glfwWindow, "GLFW can't create window", __FILE_INFO__);

	glfwMakeContextCurrent(m_glfwWindow);

	glfwSetKeyCallback(m_glfwWindow, KeyCallback);
	glfwSetMouseButtonCallback(m_glfwWindow, MouseCallback);
	glfwSetScrollCallback(m_glfwWindow, ScrollCallback);

	if (!ElkTools::Utils::IniLoader::GetBool("window.ini", "vertical_sync"))
		glfwSwapInterval(0);
}

void Context::Device::Close()
{
	// TODO: Delete window before terminate
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}

bool Context::Device::IsActive() { return !glfwWindowShouldClose(m_glfwWindow); }

int Context::Device::GetKey(const int p_key) const
{
	return glfwGetKey(m_glfwWindow, p_key);
}

int Context::Device::GetMouseButton(const int p_mouseButton) const
{
	return glfwGetMouseButton(m_glfwWindow, p_mouseButton);
}

std::pair<double, double>& Context::Device::GetCursorPosition()
{
	glfwGetCursorPos(m_glfwWindow, &m_cursorPosition.first, &m_cursorPosition.second);
	return m_cursorPosition;
}

void ElkGameEngine::Context::Device::LockCursor()
{
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void ElkGameEngine::Context::Device::FreeCursor()
{
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int Context::Device::GetPressState()
{
	return GLFW_PRESS;
}

int Context::Device::GetReleaseState()
{
	return GLFW_RELEASE;
}

void Context::Device::PollEvents()
{
	glfwPollEvents();
}

void Context::Device::SwapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}
