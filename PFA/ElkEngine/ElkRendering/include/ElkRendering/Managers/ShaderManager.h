#pragma once

#include <ElkTools/Managers/AResourceManager.h>

#include "ElkRendering/export.h"
#include "ElkRendering/Resources/Shader.h"

namespace ElkRendering
{
	namespace Managers
	{
		class ELK_RENDERING_EXPORT ShaderManager : public ElkTools::Managers::AResourceManager<Resources::Shader>
		{
		public:
			ShaderManager() : AResourceManager(ElkTools::Utils::SharedData::__RESOURCE_MANAGERS_FOLDER_PATH + "shaders.cfg") {}

			Resources::Shader* Load(const std::string& p_path) override
			{
				Resources::Shader* shader = new Resources::Shader();
				 if (!shader->LoadFromFile(p_path))
				 {
				 	delete shader;
				 	shader = nullptr;
				 	ElkTools::Debug::Log::Process("Failed to load shader: " + p_path, ElkTools::Debug::Log::LogLevel::LOG_ERROR);
				 }

				return shader;
			}

			bool RecompileShaders()
			{
				auto resources = GetEveryResources();
				bool success = false;
				bool hasFailed = false;
				std::string shaderName;

				for (auto& shader : resources)
				{
					success = shader.second.first->Recompile();

					if (!success)
						hasFailed = true;
				}

				if (hasFailed)
					ElkTools::Debug::Log::Process("Shaders re-compilation failed", ElkTools::Debug::Log::LogLevel::LOG_ERROR);
				else
					ElkTools::Debug::Log::Process("Shaders re-compilation completed", ElkTools::Debug::Log::LogLevel::LOG_INFO);

				return !hasFailed;
			}
		};
	}
}
