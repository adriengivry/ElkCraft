#pragma once

#include "ElkRendering/export.h"

#include "ElkRendering/Resources/Texture.h"
 
namespace ElkRendering
{
	namespace Resources
	{
		class ELK_RENDERING_EXPORT RenderSprite
		{
		private:
			struct ELK_RENDERING_EXPORT PositionAndTexture
			{
				float position[3];
				float textureCoordinates[2];
			};

		public:
			RenderSprite();
			~RenderSprite() = default;

			void Setup();
			void Draw() const;
			void Update();

			void SetToDraw(bool p_value);

			Texture* GetTexture();
			void SetTexture(Texture& p_texture);

			glm::vec3 GetCoordinateMin() const;
			glm::vec3 GetCoordinateMax() const;

		private:
			void InitBuffers();
			void InitSpritePositions();
			void InitSpriteTextureCoordinates();
			void InitSpriteIndices();

			ElkRendering::Resources::Texture* m_texture;

			/* Buffers */
			unsigned int m_vao;
			unsigned int m_vbo;
			unsigned int m_ebo;
			
			PositionAndTexture m_positionAndTexture[4];

			unsigned int m_indices[6];

			bool m_toDraw;
		};
	}
}
