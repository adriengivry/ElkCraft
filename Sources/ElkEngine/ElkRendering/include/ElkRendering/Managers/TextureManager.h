#pragma once

#include <ElkTools/Managers/AResourceManager.h>

#include "ElkRendering/export.h"
#include "ElkRendering/Resources/Texture.h"

namespace ElkRendering
{
	namespace Managers
	{
		/*
		 * Resource Manager that can handle textures
		 */
		class ELK_RENDERING_EXPORT TextureManager : public ElkTools::Managers::AResourceManager<Resources::Texture>
		{
		public:
			TextureManager() : AResourceManager(ElkTools::Utils::SharedData::__RESOURCE_MANAGERS_FOLDER_PATH + "textures.cfg") {}

			Resources::Texture* Load(const std::string& p_path) override
			{
				Resources::Texture* texture = new Resources::Texture();
				if (!texture->LoadFromFile(p_path))
				{
					delete texture;
					texture = nullptr;
					ElkTools::Debug::Log::Process("Failed to load texture: " + p_path, ElkTools::Debug::Log::LogLevel::LOG_ERROR);
				}

				return texture;
			}
		};
	}
}
