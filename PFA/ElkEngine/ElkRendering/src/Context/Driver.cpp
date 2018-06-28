#include "stdafx.h"

#include <ElkTools/Debug/Log.h>
#include <ElkTools/Utils/IniLoader.h>

#include "ElkRendering/Context/Driver.h"
#include "ElkRendering/Debug/GLDebug.h"

using namespace ElkRendering::Context;
using namespace ElkRendering::Debug;

Driver::Driver()
{
	m_active = false;
}

void Driver::Setup()
{
	const GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		ElkTools::Debug::Log::Process("Error init glew : " +
			std::string(reinterpret_cast<const char*>(glewGetErrorString(error))), ElkTools::Debug::Log::LogLevel::LOG_ERROR);
	}

	GLCall(glEnable(GL_DEBUG_OUTPUT));
	GLCall(glDebugMessageCallback(GLDebug::DebugCallbackError, nullptr));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA););

	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glCullFace(GL_BACK));

	m_clearRed		= ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "clear_red");
	m_clearGreen	= ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "clear_green");
	m_clearBlue		= ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "clear_blue");
	m_clearAlpha	= ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "clear_alpha");

	DisplayDriverInfo();

	m_active = true;

	GLuint copy_warning = 0x20072;
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE,
		GL_DONT_CARE, 1, &copy_warning, GL_FALSE);
}

bool Driver::IsActive() { return m_active; }

void ElkRendering::Context::Driver::DisplayDriverInfo()
{
	std::vector<std::string> openglInfo
	{
		std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))),
		"OpenGL version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))),
		"GLSL version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)))
	};

	for (auto& info : openglInfo)
		ElkTools::Debug::Log::Process(info, ElkTools::Debug::Log::LogLevel::LOG_DEFAULT, ElkTools::Debug::Log::LogColor::GREEN);
	ElkTools::Debug::Log::LineBreak(2);
}

void Driver::ClearScreen(float p_coeff)
{
	GLCall(glClearColor(static_cast<GLclampf>(m_clearRed * p_coeff), static_cast<GLclampf>(m_clearGreen * p_coeff), static_cast<GLclampf>(m_clearBlue * p_coeff), static_cast<GLclampf>(m_clearAlpha)));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}