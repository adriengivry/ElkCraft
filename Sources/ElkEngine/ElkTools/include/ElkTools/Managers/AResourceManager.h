#pragma once

#include <iostream>
#include <unordered_map>
#include <string>

#include <ElkAPI\IManager.h>
#include <ElkTools\Utils\SharedData.h>
#include <ElkTools\Debug\Log.h>

namespace ElkTools
{
	namespace Managers
	{
		/*
		 * Base class for every ResourceManager
		 */
		template <typename T>
		class AResourceManager : public ElkAPI::IManager
		{
		private:
			std::string m_pathsFile;

		public:
			explicit AResourceManager(const std::string& p_pathsFile)
			{
				m_pathsFile = p_pathsFile;
				Setup();
			}

			void Setup()
			{
				LoadPaths(m_pathsFile);
			}

			void Close()
			{
				PurgeResources();
			}

			virtual ~AResourceManager() { Close(); }

			std::unordered_map<std::string,
				std::pair<T*, unsigned int>>&GetEveryResources() { return m_resources; }

			T* RequireAndGet(const std::string& l_id)
			{
				T* result = GetResource(l_id);

				if (!result)
				{
					if (RequireResource(l_id))
						return GetResource(l_id);

					return nullptr;
				}

				return result;
			}

			T* GetResource(const std::string& l_id)
			{
				auto res = Find(l_id);
				return (res ? res->first : nullptr);
			}

			std::string GetPath(const std::string& l_id)
			{
				const auto path = m_paths.find(l_id);
				return (path != m_paths.end() ? path->second : "");
			}

			bool RequireResource(const std::string& l_id)
			{
				auto res = Find(l_id);
				if (res)
				{
					ElkTools::Debug::Log::Process(l_id + " required but already loaded", ElkTools::Debug::Log::LogLevel::LOG_WARNING);
					++res->second;
					return true;
				}
				const auto path = m_paths.find(l_id);
				if (path == m_paths.end()) { return false; }
				T* resource = Load(path->second);
				if (!resource) { return false; }
				m_resources.emplace(l_id, std::make_pair(resource, 1));
				ElkTools::Debug::Log::Process(l_id + " loaded", ElkTools::Debug::Log::LogLevel::LOG_INFO);
				return true;
			}

			bool ReleaseResource(const std::string& l_id)
			{
				auto res = Find(l_id);

				if (!res)
					return false;

				--res->second;

				if (!res->second)
					Unload(l_id);

				return true;
			}

			void PurgeResources()
			{
				ElkTools::Debug::Log::Process("Start purging all resources", ElkTools::Debug::Log::LogLevel::LOG_INFO);
				while (m_resources.begin() != m_resources.end())
				{
					ElkTools::Debug::Log::Process("Removing: " + m_resources.begin()->first, ElkTools::Debug::Log::LogLevel::LOG_INFO);
					delete m_resources.begin()->second.first;
					m_resources.erase(m_resources.begin());
				}
				ElkTools::Debug::Log::Process("Purging completed", ElkTools::Debug::Log::LogLevel::LOG_INFO);
			}

		protected:
			virtual T* Load(const std::string& l_path) = 0;

		private:
			std::pair<T*, unsigned int>* Find(const std::string& l_id)
			{
				auto itr = m_resources.find(l_id);
				return (itr != m_resources.end() ? &itr->second : nullptr);
			}

			bool Unload(const std::string& l_id)
			{
				auto itr = m_resources.find(l_id);
				if (itr == m_resources.end()) { return false; }
				delete itr->second.first;
				m_resources.erase(itr);
				return true;
			}

			void LoadPaths(const std::string& l_pathFile)
			{
				std::ifstream paths;
				paths.open(l_pathFile);
				if (paths.is_open())
				{
					std::string line;
					while (std::getline(paths, line))
					{
						std::stringstream keystream(line);
						std::string pathName;
						std::string path;
						keystream >> pathName;
						keystream >> path;
						m_paths.emplace(pathName, path);
					}
					paths.close();
					return;
				}
				ElkTools::Debug::Log::Process("Failed loading the path file: " + l_pathFile, ElkTools::Debug::Log::LogLevel::LOG_ERROR);
			}

			std::unordered_map<std::string,
			                   std::pair<T*, unsigned int>> m_resources;
			std::unordered_map<std::string, std::string> m_paths;
		};
	}
}
