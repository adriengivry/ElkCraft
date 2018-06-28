#pragma once

#include <unordered_map>
#include <map>
#include <chrono>
#include <iostream>

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		class ELK_TOOLS_EXPORT Profiler
		{
		public:
			struct Spy
			{
				#define PROFILER_SPY(name)\
					std::unique_ptr<ElkTools::Utils::Profiler::Spy> __profiler_spy__ = \
					ElkTools::Utils::Profiler::__RUNNING ? std::make_unique<ElkTools::Utils::Profiler::Spy>(name) : nullptr

				Spy(const std::string& p_name) : name(p_name)
				{
					start = std::chrono::steady_clock::now();
				}

				~Spy() { Destroy(); }

				void Destroy()
				{
					end = std::chrono::steady_clock::now();
					Profiler::Save(*this);
				}

				std::string name;
				std::chrono::steady_clock::time_point start;
				std::chrono::steady_clock::time_point end;
			};

			void Start();
			void Stop();
			void Toggle();
			void Execute();
			void Log();

			static void Save(Spy& p_spy);

			std::chrono::steady_clock::time_point m_currentTime;
			std::chrono::steady_clock::time_point m_lastTime;
			std::chrono::duration<double> m_elapsed;
			std::chrono::duration<double> m_timer;
			double m_logFrequency;
			std::multimap<double, std::string> m_sortedHistory;

			static bool __RUNNING;
		private:
			static std::unordered_map<std::string, double> __ELPASED_HISTORY;
			static std::unordered_map<std::string, uint64_t> __CALLS_COUNTER;

			static bool __EXECUTE_ONCE;
			static bool __WAIT_NEXT_FRAME;
		};
	}
}