#pragma once

#include <ElkAPI\IContext.h>

#include "ElkRendering/export.h"

namespace ElkRendering
{
	namespace Context
	{
		/*
		* The Driver context handle the OpenGL Context. Actually it uses GLEW Libarary
		*/
		class ELK_RENDERING_EXPORT Driver : public ElkAPI::IContext
		{
		public:
			Driver();
			virtual ~Driver() = default;
			void Setup() override;
			void Close() override {}
			bool IsActive() override;

			void DisplayDriverInfo();

			void ClearScreen(float p_coeff = 1.f);

		private:
			float m_clearRed;
			float m_clearGreen;
			float m_clearBlue;
			float m_clearAlpha;

			bool m_active;
		};
	}
}
