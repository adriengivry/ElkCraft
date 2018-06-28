#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <type_traits>
#include <typeinfo>  

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/AObject.h"
#include "ElkGameEngine/Objects/Components/AComponent.h"
#include "ElkGameEngine/Objects/Components/Transform.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		/*
		 * A GameObject is an Object that can be placed in the Scene. It contains differents components
		 */
		class ELK_GAMEENGINE_EXPORT GameObject : public AObject
		{
		public:
			GameObject(const std::string& p_name = "GameObject");
			virtual ~GameObject() = default;

			void UpdateComponents();

			template <typename Class, typename ... Args>
			std::shared_ptr<Class> AddComponent(Args... args);

			template<typename Class>
			std::shared_ptr<Class> GetComponent();
			std::vector<std::shared_ptr<Objects::Components::AComponent>>& GetComponents();

			std::shared_ptr<Objects::Components::Transform> transform;
			std::vector<std::shared_ptr<Objects::Components::AComponent>> m_components;
		};

		template <typename Class, typename ... Args>
		std::shared_ptr<Class> ElkGameEngine::Objects::GameObject::AddComponent(Args... args)
		{
			auto newComp = std::shared_ptr<Class>(nullptr);

			if (std::is_base_of<Objects::Components::AComponent, Class>::value)
			{
				newComp = std::make_shared<Class>(std::forward<Args>(args)...);
				newComp->SetOwner(*this);
				m_components.push_back(newComp);

				return newComp;
			}

			return newComp;
		}

		template <typename Class>
		std::shared_ptr<Class> GameObject::GetComponent()
		{
			std::shared_ptr<Class> result(nullptr);

			for (auto component : m_components)
			{
				result = std::dynamic_pointer_cast<Class>(component);
				if (result)
					break;
			}

			return result;
		}
	}
}
