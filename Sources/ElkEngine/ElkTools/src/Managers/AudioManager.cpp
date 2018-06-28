#include "stdafx.h"
#include "ElkTools/Managers/AudioManager.h"

using namespace ElkTools::Managers;

ElkTools::Managers::AudioManager::AudioManager() : 
	m_randomEngine(m_seed())
{
	Setup();
}

ElkTools::Managers::AudioManager::~AudioManager()
{
	Close();
}

void ElkTools::Managers::AudioManager::Setup()
{
	m_audioEngine = irrklang::createIrrKlangDevice();

	if (!m_audioEngine)
		ElkTools::Debug::Log::Process("Creating IrrKlang audio engine failed", ElkTools::Debug::Log::LogLevel::LOG_ERROR);
	else
		SetGlobalVolume(ElkTools::Utils::IniLoader::GetFloat("audio.ini", "volume"));
}

void ElkTools::Managers::AudioManager::Close()
{
	m_audioEngine->drop();
}

void ElkTools::Managers::AudioManager::SetListenerPosition(float p_x, float p_y, float p_z, float p_forwardX, float p_forwardY, float p_forwardZ)
{
	m_audioEngine->setListenerPosition(irrklang::vec3df(p_x, p_y, p_z), irrklang::vec3df(p_forwardX, p_forwardY, p_forwardZ));
}

void ElkTools::Managers::AudioManager::SetGlobalVolume(float p_volume)
{
	m_volume = p_volume;
	m_audioEngine->setSoundVolume(m_volume);
}

float ElkTools::Managers::AudioManager::GetGlobalVolume()
{
	return m_volume;
}

std::string ElkTools::Managers::AudioManager::FindRandomSound(const std::string & p_folder, const std::string & p_soundName, const std::string & p_extension, uint8_t p_numberOfSounds)
{
	std::uniform_int_distribution<int> soundRandomDistribution(1, p_numberOfSounds);
	return p_folder + p_soundName + std::to_string(soundRandomDistribution(m_randomEngine)) + p_extension;
}

irrklang::ISound* ElkTools::Managers::AudioManager::Play2DSound(const std::string & p_toPlay, bool p_loop, float p_volume, float p_speed)
{
	auto sound = m_audioEngine->play2D(p_toPlay.c_str(), p_loop, true, true);

	if (sound)
	{
		SetSoundParameters(sound, p_volume, p_speed);
		sound->setIsPaused(false);
	}

	return sound;

	return nullptr;
}

irrklang::ISound* ElkTools::Managers::AudioManager::Play3DSound(const std::string & p_toPlay, float p_x, float p_y, float p_z, bool p_loop, float p_volume, float p_speed)
{
	auto sound = m_audioEngine->play3D(p_toPlay.c_str(), irrklang::vec3df(p_x, p_y, p_z), p_loop, true, true);

	if (sound)
	{
		SetSoundParameters(sound, p_volume, p_speed);
		sound->setIsPaused(false);
	}

	return sound;

	return nullptr;
}

void ElkTools::Managers::AudioManager::StopAllSounds()
{
	m_audioEngine->stopAllSounds();
}

void ElkTools::Managers::AudioManager::SetSoundParameters(irrklang::ISound * p_sound, float p_volume, float p_speed)
{
	if (p_sound)
	{
		p_sound->setVolume(p_volume);
		p_sound->setPlaybackSpeed(p_speed * ElkTools::Utils::Time::GetTimeScale());
		p_sound->setIsPaused(false);
	}
}
