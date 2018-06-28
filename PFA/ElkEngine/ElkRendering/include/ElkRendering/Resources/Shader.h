#pragma once

#include <string>
#include <unordered_map>

#include <glm\glm.hpp>

namespace ElkRendering
{
	namespace Resources
	{
		struct ShaderProgramSource
		{
			std::string vertexSource;
			std::string geometrySource;
			std::string fragmentSource;
		};

		typedef unsigned int GLenum;
		typedef unsigned int GLuint;

		class Shader
		{
		private:
			static const std::string m_vertexShaderStart;
			static const std::string m_geometryShaderStart;
			static const std::string m_fragmentShaderStart;

		public:
			Shader() = default;
			~Shader();

			std::string& GetSourceFile() { return m_sourceFile; }
			bool LoadFromFile(const std::string& p_path);
			bool Recompile();
			
			void Bind() const;
			void Unbind() const;

			inline unsigned int GetRendererID() const { return m_rendererID; }

			void SetUniformIntPointer(const std::string& p_name, int* p_value, const int p_size);
			void SetUniformInt(const std::string& p_name, const int p_value);
			void SetUniformFloat(const std::string& p_name, const float p_value);
			void SetUniformVec2(const std::string& p_name, const glm::vec2& p_vec2);
			void SetUniformVec3(const std::string& p_name, const glm::vec3& p_vec3);
			void SetUniformVec4(const std::string& p_name, const glm::vec4& p_vec4);
			void SetUniformMat4(const std::string& p_name, const glm::mat4& p_mat4);

		private:
			ShaderProgramSource ParseShaderSource(const std::string&& p_filepath);

			bool CompileSources(const ShaderProgramSource& p_sources);
			bool CompileShader(const std::string& p_toCompile, const GLenum p_shaderType, GLuint& p_result);
			bool CatchCompilerError(const GLenum p_compilerAction, const GLuint p_compiledShader) const;
			int GetUniformLocation(const std::string& name);

			std::string m_sourceFile;
			std::unordered_map<std::string, int> m_uniformLocationCache;
			unsigned int m_rendererID = 0;
		};
	}
}
