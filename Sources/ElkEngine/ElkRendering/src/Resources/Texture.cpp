#include "stdafx.h"

#include <ElkTools\Debug\Log.h>

#include "ElkRendering/Debug/GLDebug.h"

/*
 * Macro required to get stb_image working (Needs to be defined before #include <stb_image.h>
 */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ElkRendering/Resources/Texture.h"

using namespace ElkRendering::Resources;

Texture::Texture()
	: m_textureId(0), m_width(0), m_height(0), m_bitsPerPixel(0)
{
}

Texture::Texture(const std::string& p_type)
	: Texture()
{
	m_type = p_type;
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_textureId));
}

bool Texture::LoadFromFile(const std::string& p_path)
{
	m_path = p_path;

	GLCall(glGenTextures(1, &m_textureId));

	stbi_set_flip_vertically_on_load(true);
	unsigned char* dataBuffer = stbi_load(p_path.c_str(), &m_width, &m_height, &m_bitsPerPixel, 0);

	if (dataBuffer)
	{
		GLenum format;
		if (m_bitsPerPixel == 1)
			format = GL_RED;
		else if (m_bitsPerPixel == 3)
			format = GL_RGB;
		else if (m_bitsPerPixel == 4)
			format = GL_RGBA;

		GLCall(glBindTexture(GL_TEXTURE_2D, m_textureId));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, dataBuffer));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		stbi_image_free(dataBuffer);
	}
	else
		ElkTools::Debug::Log::Process("Texture failed to load at path: " + p_path, ElkTools::Debug::Log::LogLevel::LOG_ERROR);

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	return m_textureId != 0;
}

void Texture::Bind(const unsigned& p_slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + p_slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureId));
}

void Texture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::SetType(const std::string& p_type)
{
	m_type = p_type;
}

const std::string& Texture::GetType() const
{
	return m_type;
}

const unsigned& Texture::GetId() const
{
	return m_textureId;
}
