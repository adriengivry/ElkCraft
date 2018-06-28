#include "stdafx.h"

#include <ElkTools\Debug\Log.h>
#include <ElkTools\Utils\IniLoader.h>

#include "ElkRendering/Debug/GLDebug.h"

using namespace ElkRendering::Debug;
using namespace ElkTools::Utils;

void CALLBACK GLDebug::DebugCallbackError(const GLenum p_source,
                                           const GLenum p_type,
                                           const GLuint p_id,
                                           const GLenum p_severity,
                                           const GLsizei p_length,
                                           const GLchar* p_msg,
                                           const void* p_data)
{
	if (Filter(p_severity))
		return;

	ElkTools::Debug::Log::LineBreak();
	ElkTools::Debug::Log::Process("**********CoreDebug Output**************", ElkTools::Debug::Log::LogLevel::LOG_DEFAULT, ElkTools::Debug::Log::LogColor::YELLOW);
	ElkTools::Debug::Log::Process("source " + GetStringForSource(p_source), ElkTools::Debug::Log::LogLevel::LOG_DEFAULT);
	ElkTools::Debug::Log::Process("type: " + GetStringForType(p_type), ElkTools::Debug::Log::LogLevel::LOG_DEFAULT);
	ElkTools::Debug::Log::Process("id: " + std::to_string(p_id), ElkTools::Debug::Log::LogLevel::LOG_DEFAULT);
	ElkTools::Debug::Log::Process("severity: " + GetStringForSeverity(p_severity), ElkTools::Debug::Log::LogLevel::LOG_DEFAULT);
	ElkTools::Debug::Log::Process("debug call: " + std::string(p_msg), ElkTools::Debug::Log::LogLevel::LOG_DEFAULT);
	ElkTools::Debug::Log::Process("**************************************", ElkTools::Debug::Log::LogLevel::LOG_DEFAULT, ElkTools::Debug::Log::LogColor::YELLOW);
	ElkTools::Debug::Log::LineBreak();
}

void GLDebug::GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLDebug::GLLogCall(const char* function, const char* file, int line)
{
	GLenum error;
	while ((error = glGetError()))
	{
		std::cout << "[OpenGL Error] (" << error << ") " << function << " " << file << ": " << line;
		return false;
	}

	return true;
}

bool ElkRendering::Debug::GLDebug::Filter(const GLenum p_severity)
{
	if (p_severity == GL_DEBUG_SEVERITY_NOTIFICATION && !IniLoader::GetBool("logs.ini", "log_gl_notification"))
		return true;

	if (p_severity == GL_DEBUG_SEVERITY_LOW && !IniLoader::GetBool("logs.ini", "log_gl_low"))
		return true;

	if (p_severity == GL_DEBUG_SEVERITY_MEDIUM && !IniLoader::GetBool("logs.ini", "log_gl_medium"))
		return true;

	if (p_severity == GL_DEBUG_SEVERITY_HIGH && !IniLoader::GetBool("logs.ini", "log_gl_high"))
		return true;

	return false;
}

std::string GLDebug::GetStringForSource(const GLenum p_source)
{
	switch (p_source)
	{
	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window system";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third party";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		__debugbreak();
		return "";
	}
}

std::string GLDebug::GetStringForType(const GLenum p_type)
{
	switch (p_type)
	{
	case GL_DEBUG_TYPE_ERROR:
		return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined behavior";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability issue";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance issue";
	case GL_DEBUG_TYPE_MARKER:
		return "Stream annotation";
	case GL_DEBUG_TYPE_OTHER_ARB:
		return "Other";
	default:
		__debugbreak();
		return "";
	}
}


std::string GLDebug::GetStringForSeverity(const GLenum p_severity)
{
	switch (p_severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		return "High";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "Medium";
	case GL_DEBUG_SEVERITY_LOW:
		return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "Notification";
	default:
		__debugbreak();
		return ("");
	}
}
