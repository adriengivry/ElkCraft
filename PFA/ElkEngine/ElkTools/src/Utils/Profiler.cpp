#include "stdafx.h"

#include "ElkTools/Utils/Profiler.h"
#include "ElkTools/Debug/Log.h"
#include "ElkTools/Utils/IniLoader.h"

#include <iomanip>

using namespace ElkTools::Debug;

bool ElkTools::Utils::Profiler::__WAIT_NEXT_FRAME = false;
bool ElkTools::Utils::Profiler::__EXECUTE_ONCE = false;
bool ElkTools::Utils::Profiler::__RUNNING = false;
std::unordered_map<std::string, double> ElkTools::Utils::Profiler::__ELPASED_HISTORY;
std::unordered_map<std::string, uint64_t> ElkTools::Utils::Profiler::__CALLS_COUNTER;

void ElkTools::Utils::Profiler::Start()
{
	__RUNNING = true;
	m_logFrequency = static_cast<double>(Utils::IniLoader::GetFloat("profiler.ini", "log_frequency"));
	m_currentTime = std::chrono::high_resolution_clock::now();
	m_timer = std::chrono::duration<double>(0.0);
	__ELPASED_HISTORY.clear();
	Debug::Log::Process("Profiler : ON", ElkTools::Debug::Log::LogLevel::LOG_INFO);
}

void ElkTools::Utils::Profiler::Stop()
{
	__RUNNING = false;
	__ELPASED_HISTORY.clear();
	Debug::Log::Process("Profiler : OFF", ElkTools::Debug::Log::LogLevel::LOG_INFO);
}

void ElkTools::Utils::Profiler::Toggle()
{
	if (__RUNNING)	Stop();
	else			Start();
}

void ElkTools::Utils::Profiler::Execute()
{
	__EXECUTE_ONCE = true;
	__WAIT_NEXT_FRAME = false;
	Start();
}

void ElkTools::Utils::Profiler::Log()
{
	if (__RUNNING)
	{
		m_lastTime = m_currentTime;
		m_currentTime = std::chrono::high_resolution_clock::now();
		m_elapsed = m_currentTime - m_lastTime;

		m_timer += m_elapsed;

		if (m_timer.count() > 1.0 || (__EXECUTE_ONCE && __WAIT_NEXT_FRAME))
		{
			ElkTools::Debug::Log::Process("---- [Profiler Stats] ----");

			m_sortedHistory.clear();

			for (auto& data : __ELPASED_HISTORY)
			{
				m_sortedHistory.insert(std::pair<double, std::string>(data.second, data.first));
			}

			for (auto& data : m_sortedHistory)
			{
				std::string action = data.second;
				std::string duration = std::to_string(data.first) + "s";
				std::string percentage = std::to_string((data.first / m_timer.count()) * 100.0) + "%";
				std::string calls = std::to_string(__CALLS_COUNTER[data.second]);
				ElkTools::Debug::Log::Process("METHOD:" + action, Log::LogLevel::LOG_DEFAULT, Log::LogColor::BLUE);
				ElkTools::Debug::Log::Process("    DURATION:" + duration);
				ElkTools::Debug::Log::Process("    PERCENTAGE:" + percentage);
				ElkTools::Debug::Log::Process("    CALLS:" + calls);
				ElkTools::Debug::Log::LineBreak();
			}

			if (__EXECUTE_ONCE)
			{
				Stop();
				__EXECUTE_ONCE = false;
			}

			__ELPASED_HISTORY.clear();
			__CALLS_COUNTER.clear();
			m_timer = std::chrono::duration<double>(0.0);
		}

		if (__EXECUTE_ONCE && !__WAIT_NEXT_FRAME)
			__WAIT_NEXT_FRAME = true;
	}
}

void ElkTools::Utils::Profiler::Save(Spy & p_spy)
{
	if (__ELPASED_HISTORY.find(p_spy.name) != __ELPASED_HISTORY.end())
	{
		__ELPASED_HISTORY[p_spy.name] += std::chrono::duration<double>(p_spy.end - p_spy.start).count();
	}
	else
	{
		__ELPASED_HISTORY[p_spy.name] = std::chrono::duration<double>(p_spy.end - p_spy.start).count();
	}

	if (__CALLS_COUNTER.find(p_spy.name) != __CALLS_COUNTER.end())
	{
		++__CALLS_COUNTER[p_spy.name];
	}
	else
	{
		__CALLS_COUNTER[p_spy.name] = 1;
	}
}
