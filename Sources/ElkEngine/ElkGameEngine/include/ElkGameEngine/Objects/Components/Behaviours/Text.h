#pragma once

#include <ElkRendering/Resources/RenderText.h>

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
				/*
				* Camera component used to view the scene
				*/
				class ELK_GAMEENGINE_EXPORT Text : public ABehaviour
				{
				public:
					enum class ScalingMode
					{
						DEFAULT								= 0,
						GAMEOBJECT_TRANSFORM_AND_FONT_SIZE	= 0,
						FONT_SIZE_ONLY						= 1,
						GAMEOBJECT_TRANSFORM_ONY			= 2,
						NO_SCALE							= 3
					};

					Text();
					virtual ~Text() = default;

					void SetString(const std::string& p_string);
					std::string GetString();

					void SetSpacing(float p_spacing);
					float GetSpacing();

					void SetFontSize(float p_size);
					float GetFontSize();

					void SetPositionOffset(const glm::vec3& p_offset);
					const glm::vec3& GetPositionOffset();

					void SetFontColor(const glm::vec4& p_color);
					const glm::vec4& GetFontColor();

					void SetScalingMode(ScalingMode p_newScalingMode);
					ScalingMode GetScalingMode();

					ElkRendering::Resources::RenderText& GetRenderText();

				private:
					float m_spacing;
					float m_fontSize;
					glm::vec3 m_positionOffset;
					glm::vec4 m_fontColor;

					ScalingMode m_scalingMode;

					ElkRendering::Resources::RenderText m_renderText;
				};
			}
		}
	}
}