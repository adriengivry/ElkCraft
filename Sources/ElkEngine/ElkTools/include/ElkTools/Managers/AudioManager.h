#pragma once

#include <string>
#include <random>

#include <irrKlang.h>

#include <ElkAPI/IManager.h>
#include <ElkTools/Debug/Log.h>
#include <ElkTools/Utils/IniLoader.h>
#include <ElkTools/Utils/Time.h>

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Managers
	{
		class ELK_TOOLS_EXPORT AudioManager : public ElkAPI::IManager
		{
		public:
			AudioManager();
			~AudioManager();

			void Setup() override;
			void Close() override;

			void SetListenerPosition(float p_x, float p_y, float p_z, float p_forwardX, float p_forwardY, float p_forwardZ);

			void SetGlobalVolume(float p_volume);
			float GetGlobalVolume();

			std::string FindRandomSound(const std::string& p_folder, const std::string& p_soundName, const std::string& p_extension, uint8_t p_numberOfSounds);

			irrklang::ISound* Play2DSound(const std::string& p_toPlay, bool p_loop = false, float p_volume = 1.0f, float p_speed = 1.0f);
			irrklang::ISound* Play3DSound(const std::string& p_toPlay, float p_x, float p_y, float p_z, bool p_loop = false, float p_volume = 1.0f, float p_speed = 1.0f);

			void StopAllSounds();

			void SetSoundParameters(irrklang::ISound* p_sound, float p_volume = 1.0f, float p_speed = 1.0f);

		private:
			float m_volume;
			irrklang::ISoundEngine* m_audioEngine;
			std::random_device m_seed;
			std::mt19937 m_randomEngine;
		};
	}
}
