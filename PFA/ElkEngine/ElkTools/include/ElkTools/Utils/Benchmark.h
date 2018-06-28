#pragma once

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <memory>

#include "ElkTools/export.h"
#include "ElkTools/Debug/Log.h"

namespace ElkTools
{
	namespace Utils
	{
		class ELK_TOOLS_EXPORT Benchmark
		{
		public:
			struct GameEngineState
			{
				GameEngineState(bool& p_rendering, bool& p_physics) : rendering(p_rendering), physics(p_physics) {}
				bool& rendering;
				bool& physics;
			};

			struct StateData
			{
				std::vector<double> framerateHistory;

				double duration;
				double minimumFrametate;
				double maximumFramerate;
				double medianFramerate;
				double sumFramerate;
				double averageFramerate;
				double score;

				void Calculate()
				{
					minimumFrametate = *std::min_element(framerateHistory.begin(), framerateHistory.end());
					maximumFramerate = *std::max_element(framerateHistory.begin(), framerateHistory.end());
					medianFramerate = (minimumFrametate + maximumFramerate) * 0.5;
					sumFramerate = std::accumulate(framerateHistory.begin(), framerateHistory.end(), 0.0);
					averageFramerate = framerateHistory.size() != 0 ? sumFramerate / framerateHistory.size() : 0;
					score = (sumFramerate / duration) / 100.0;
				}

				void Log()
				{
					Debug::Log::Process("Duration: " + std::to_string(duration) + " seconds");
					Debug::Log::Process("Minimum: " + std::to_string(minimumFrametate) + " FPS");
					Debug::Log::Process("Maximum: " + std::to_string(maximumFramerate) + " FPS");
					Debug::Log::Process("Median: " + std::to_string(medianFramerate) + " FPS");
					Debug::Log::Process("Average: " + std::to_string(averageFramerate) + " FPS");
					Debug::Log::Process("Score: " + std::to_string(static_cast<uint32_t>(score)));
				}

				void Reset()
				{
					framerateHistory.clear();
					minimumFrametate = 0.0;
					maximumFramerate = 0.0;
					medianFramerate = 0.0;
					sumFramerate = 0.0;
					averageFramerate = 0.0;
					score = 0.0;
				}
			};

			struct BenchmarkResults
			{
				void Calculate()
				{
					empty.Calculate();
					rendering.Calculate();
					physics.Calculate();
					full.Calculate();

					score = static_cast<uint32_t>((empty.score + rendering.score + physics.score + full.score) * 0.25);
				}

				void Log()
				{
					Debug::Log::Process("----- [Rendering: OFF | Physics: OFF] -----", Debug::Log::LogLevel::LOG_DEFAULT, Debug::Log::LogColor::BLUE);
					empty.Log();
					Debug::Log::Process("----- [Rendering: ON | Physics: OFF] -----", Debug::Log::LogLevel::LOG_DEFAULT, Debug::Log::LogColor::BLUE);
					rendering.Log();
					Debug::Log::Process("----- [Rendering: OFF | Physics: ON] -----", Debug::Log::LogLevel::LOG_DEFAULT, Debug::Log::LogColor::BLUE);
					physics.Log();
					Debug::Log::Process("----- [Rendering: ON | Physics: ON] -----", Debug::Log::LogLevel::LOG_DEFAULT, Debug::Log::LogColor::BLUE);
					full.Log();
					Debug::Log::Process("BENCHMARK SCORE: " + std::to_string(static_cast<uint32_t>(score)));
					
				}

				void Reset()
				{
					empty.Reset();
					rendering.Reset();
					physics.Reset();
					full.Reset();

					score = 0.0;
				}

				StateData empty;
				StateData rendering;
				StateData physics;
				StateData full;

				double score;
			};

			enum class BenchmarkState
			{
				EMPTY,
				RENDERING,
				PHYSICS,
				FULL,
				LAST = FULL
			};
		
			Benchmark(bool& p_rendering, bool& p_physics);

			void Start();
			void Update();
			void End();

			StateData* GetCurrentStateData();
			double GetCurrentStateDuration();
			void GoToNextState();

		private:
			GameEngineState m_gameEngineState;
			BenchmarkResults m_results;
			BenchmarkState m_currentState;

			bool m_running;

			float m_emptyDuration;
			float m_renderingDuration;
			float m_physicsDuration;
			float m_fullDuration;

			std::chrono::steady_clock::time_point m_startTime;
			std::chrono::steady_clock::time_point m_stateStartTime;
			std::chrono::steady_clock::time_point m_currentTime;
			std::chrono::steady_clock::time_point m_lastTime;
			std::chrono::duration<double> m_timer;
			std::chrono::duration<double> m_elapsed;
		};
	}
}