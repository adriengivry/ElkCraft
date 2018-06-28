#pragma once

#include <ElkRendering/Resources/RenderSprite.h>
#include <ElkRendering/Resources/Texture.h>

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Components/Behaviours/ABehaviour.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		namespace Components
		{
			namespace Behaviours
			{
				class ELK_GAMEENGINE_EXPORT Sprite : public Components::Behaviours::ABehaviour
				{
				public:
					Sprite();
					virtual ~Sprite() = default;

					ElkRendering::Resources::RenderSprite& GetRenderSprite();

					ElkRendering::Resources::Texture* GetTexture();
					void SetTexture(ElkRendering::Resources::Texture& p_texture);

					void SetRenderLayer(uint16_t p_renderLayer);
					uint16_t GetRenderLayer();

					void SetColor(const glm::vec4& p_color);
					const glm::vec4& GetColor();

					void Update() override {}

				private:
					ElkRendering::Resources::RenderSprite m_renderSprite;
					uint16_t m_renderLayer;
					glm::vec4 m_color;
				};
			}
		}
	}
}
