#pragma once

#include "ElkRendering/export.h"

namespace ElkRendering
{
	namespace Resources
	{
		/*
		 * Texture is a class that encapsulate stb_image texture
		 */
		class ELK_RENDERING_EXPORT Texture
		{
		private:
			unsigned int m_textureId;
			std::string m_type;
			std::string m_path;

			int m_width;
			int m_height;
			int m_bitsPerPixel;

		public:
			Texture();
			explicit Texture(const std::string& p_type);
			~Texture();

			bool LoadFromFile(const std::string& p_path);

			void Bind(const unsigned& p_slot = 0) const;
			static void Unbind();

			void SetType(const std::string& p_type);

			const std::string& GetType() const;
			const unsigned& GetId() const;
		};
	}
}
