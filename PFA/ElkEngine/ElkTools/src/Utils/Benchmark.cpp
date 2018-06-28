#include "stdafx.h"

#include "ElkTools/Utils/Benchmark.h"
#include "ElkTools/Utils/IniLoader.h"

ElkTools::Utils::Benchmark::Benchmark(bool & p_rendering, bool & p_physics)
	: m_gameEngineState(p_rendering, p_physics)
{
	m_running = false;
}

void ElkTools::Utils::Benchmark::Start()
{
	if (!m_running)
	{
		m_running = true;

		Debug::Log::Process("Start benchmarking...", Debug::Log::LogLevel::LOG_INFO);

		m_results.Reset();

		m_emptyDuration = Utils::IniLoader::GetFloat("benchmark.ini", "empty_duration");
		m_renderingDuration = Utils::IniLoader::GetFloat("benchmark.ini", "rendering_duration");
		m_physicsDuration = Utils::IniLoader::GetFloat("benchmark.ini", "physics_duration");
		m_fullDuration = Utils::IniLoader::GetFloat("benchmark.ini", "full_duration");

		m_currentState = BenchmarkState::EMPTY;

		m_startTime = std::chrono::high_resolution_clock::now();
		m_stateStartTime = m_startTime;
		m_currentTime = m_startTime;
		m_lastTime = m_currentTime;
	}
}

void ElkTools::Utils::Benchmark::Update()
{
	if (m_running)
	{
		m_currentTime = std::chrono::high_resolution_clock::now();
		m_elapsed = m_currentTime - m_lastTime;
		m_lastTime = m_currentTime;
		m_timer = m_currentTime - m_stateStartTime;

		switch (m_currentState)
		{
		case BenchmarkState::EMPTY:
			m_gameEngineState.rendering = false;
			m_gameEngineState.physics = false;
			break;

		case BenchmarkState::RENDERING:
			m_gameEngineState.rendering = true;
			m_gameEngineState.physics = false;
			break;

		case BenchmarkState::PHYSICS:
			m_gameEngineState.rendering = false;
			m_gameEngineState.physics = true;
			break;

		case BenchmarkState::FULL:
			m_gameEngineState.rendering = true;
			m_gameEngineState.physics = true;
			break;
		}

		if (m_timer.count() > GetCurrentStateDuration())
			GoToNextState();
		else
			GetCurrentStateData()->framerateHistory.push_back(1.0 / m_elapsed.count());
	}
}

void ElkTools::Utils::Benchmark::End()
{
	m_running = false;

	m_results.Calculate();
	m_results.Log();
}

ElkTools::Utils::Benchmark::StateData* ElkTools::Utils::Benchmark::GetCurrentStateData()
{
	switch (m_currentState)
	{
	default:
	case BenchmarkState::EMPTY:		return &m_results.empty;
	case BenchmarkState::RENDERING: return &m_results.rendering;
	case BenchmarkState::PHYSICS:	return &m_results.physics;
	case BenchmarkState::FULL:		return &m_results.full;
	}
}

double ElkTools::Utils::Benchmark::GetCurrentStateDuration()
{
	switch (m_currentState)
	{
	default:
	case BenchmarkState::EMPTY:		return m_emptyDuration;
	case BenchmarkState::RENDERING: return m_renderingDuration;
	case BenchmarkState::PHYSICS:	return m_physicsDuration;
	case BenchmarkState::FULL:		return m_fullDuration;
	}
}

void ElkTools::Utils::Benchmark::GoToNextState()
{
	GetCurrentStateData()->duration = m_timer.count();

	if (m_currentState == BenchmarkState::LAST)
	{
		End();
	}
	else
	{
		m_currentState = static_cast<BenchmarkState>(static_cast<uint8_t>(m_currentState) + 1);
		m_stateStartTime = std::chrono::high_resolution_clock::now();
		Debug::Log::Process("Switching to next benchmark state...", Debug::Log::LogLevel::LOG_INFO);
	}
}
