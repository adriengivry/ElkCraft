#include "stdafx.h"

#include <ElkTools/Debug/Log.h>
#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/IniLoader.h>

#include "ElkRendering/Resources/Shader.h"
#include "ElkRendering/Debug/GLDebug.h"

using namespace ElkRendering::Resources;

const std::string Shader::m_vertexShaderStart(ElkTools::Utils::IniLoader::GetString("shader.ini", "vertex_shader_start"));
const std::string Shader::m_geometryShaderStart(ElkTools::Utils::IniLoader::GetString("shader.ini", "geometry_shader_start"));
const std::string Shader::m_fragmentShaderStart(ElkTools::Utils::IniLoader::GetString("shader.ini", "fragment_shader_start"));

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_rendererID));
}

bool Shader::LoadFromFile(const std::string& p_path)
{
	PROFILER_SPY("Shader::LoadFromFile");

	m_sourceFile = p_path;
	const ShaderProgramSource sources = ParseShaderSource(std::move(p_path));
	bool result = CompileSources(sources);
	auto logLevel = result ? ElkTools::Debug::Log::LogLevel::LOG_INFO : ElkTools::Debug::Log::LogLevel::LOG_ERROR;
	ElkTools::Debug::Log::Process("Compiling " + m_sourceFile + ": " + (result ? "SUCCESS" : "FAILED"), logLevel);
	return result;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_rendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

bool ElkRendering::Resources::Shader::Recompile()
{
	return LoadFromFile(m_sourceFile);
}

ShaderProgramSource Shader::ParseShaderSource(const std::string&& p_filepath)
{
	ShaderProgramSource result;
	std::string line;
	std::string* targetSource = nullptr;

	std::ifstream shaderFile(p_filepath);

	if (!shaderFile.is_open())
		ElkTools::Debug::Log::Process("Error reading shader file : " + p_filepath, ElkTools::Debug::Log::LogLevel::LOG_ERROR);

	while (std::getline(shaderFile, line))
	{
		std::istringstream iss(line);

		if (line == m_vertexShaderStart)
			targetSource = &result.vertexSource;

		else if (line == m_geometryShaderStart)
			targetSource = &result.geometrySource;

		else if (line == m_fragmentShaderStart)
			targetSource = &result.fragmentSource;

		else if (targetSource != nullptr)
			*targetSource += line + "\n";
	}

	shaderFile.close();

	return result;
}

bool Shader::CompileSources(const ShaderProgramSource& p_sources)
{
	GLuint vertexShader;
	GLuint geometryShader;
	GLuint fragmentShader;

	bool vertexShaderSuccess = CompileShader(p_sources.vertexSource, GL_VERTEX_SHADER, vertexShader);
	bool geometryShaderSuccess = CompileShader(p_sources.geometrySource, GL_GEOMETRY_SHADER, geometryShader);
	bool fragmentShaderSuccess = CompileShader(p_sources.fragmentSource, GL_FRAGMENT_SHADER, fragmentShader);

	if (vertexShaderSuccess && fragmentShaderSuccess)
	{
		m_rendererID = glCreateProgram();

		GLCall(glAttachShader(m_rendererID, vertexShader));

		if (geometryShaderSuccess)
			GLCall(glAttachShader(m_rendererID, geometryShader));

		GLCall(glAttachShader(m_rendererID, fragmentShader));
		GLCall(glLinkProgram(m_rendererID));
		GLCall(glDeleteShader(vertexShader));

		if (geometryShaderSuccess)
			GLCall(glDeleteShader(geometryShader));

		GLCall(glDeleteShader(fragmentShader));

		return true;
	}

	return false;
}

bool ElkRendering::Resources::Shader::CompileShader(const std::string & p_toCompile, const GLenum p_shaderType, GLuint& p_result)
{
	if (p_toCompile.empty())
		return false;

	const char* shaderChar = p_toCompile.c_str();

	const GLuint shader = glCreateShader(p_shaderType);
	GLCall(glShaderSource(shader, 1, &shaderChar, nullptr));
	GLCall(glCompileShader(shader));

	if (CatchCompilerError(GL_COMPILE_STATUS, shader))
	{
		GLCall(glDeleteShader(shader));
		return false;
	}

	p_result = shader;
	return true;
}

bool Shader::CatchCompilerError(const GLenum p_compilerAction, const GLuint p_compiledShader) const
{
	int success;
	int length = 0;
	char* err = nullptr;

	GLCall(glGetShaderiv(p_compiledShader, p_compilerAction, &success));
	if (success == GL_FALSE)
	{
		GLCall(glGetShaderiv(p_compiledShader, GL_INFO_LOG_LENGTH, &length));
		err = static_cast<char*>(alloca(length));
		GLCall(glGetShaderInfoLog(p_compiledShader, length, nullptr, err));
		ElkTools::Debug::Log::Process("Shader " + std::string((p_compilerAction == GL_COMPILE_STATUS) ? "Compilation" : "Linking")
			+ " failed : " + err, ElkTools::Debug::Log::LogLevel::LOG_ERROR);

		return true;
	}

	return false;
}

int Shader::GetUniformLocation(const std::string& p_name)
{
	if (m_uniformLocationCache.find(p_name) != m_uniformLocationCache.end())
		return m_uniformLocationCache[p_name];

	GLCall(const int location = glGetUniformLocation(m_rendererID, p_name.c_str()));
	if (location == -1)
		ElkTools::Debug::Log::Process("Uniform " + p_name + " doesn't exist", ElkTools::Debug::Log::LogLevel::LOG_WARNING);

	m_uniformLocationCache[p_name] = location;

	return location;
}


void ElkRendering::Resources::Shader::SetUniformIntPointer(const std::string & p_name, int * p_value, const int p_size)
{
	GLCall(glUniform1iv(GetUniformLocation(p_name), p_size, p_value));
}

/* Uniforms */
void Shader::SetUniformInt(const std::string& p_name, const int p_value)
{
	GLCall(glUniform1i(GetUniformLocation(p_name), p_value));
}

void Shader::SetUniformFloat(const std::string& p_name, const float p_value)
{
	GLCall(glUniform1f(GetUniformLocation(p_name), p_value));
}

void Shader::SetUniformVec2(const std::string& p_name, const glm::vec2& p_vec2)
{
	GLCall(glUniform2f(GetUniformLocation(p_name), p_vec2.x, p_vec2.y));
}

void Shader::SetUniformVec3(const std::string& p_name, const glm::vec3& p_vec3)
{
	GLCall(glUniform3f(GetUniformLocation(p_name), p_vec3.x, p_vec3.y, p_vec3.z));
}

void Shader::SetUniformVec4(const std::string& p_name, const glm::vec4& p_vec4)
{
	GLCall(glUniform4f(GetUniformLocation(p_name), p_vec4.x, p_vec4.y, p_vec4.z, p_vec4.w));
}

void Shader::SetUniformMat4(const std::string& p_name, const glm::mat4& p_mat4)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(p_name), 1, GL_FALSE, &p_mat4[0][0]));
}