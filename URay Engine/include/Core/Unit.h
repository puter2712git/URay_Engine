#ifndef UNIT_H
#define UNIT_H

#include "Core/Component.h"
#include <string>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace URay
{
	class Unit
	{
	public:
		Unit() = default;
		~Unit() = default;

	public:
		void Update();
		void Render();

		template <typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			auto newComp = std::make_unique<T>(std::forward<Args>(args)...);
			newComp->SetOwner(this);
			T* pComp = newComp.get();
			
			_components.push_back(std::move(newComp));
			_componentMap[typeid(T)] = pComp;
			pComp->Initialize();
			return pComp;
		}

		template <typename T>
		T* GetComponent()
		{
			auto it = _componentMap.find(typeid(T));
			if (it != _componentMap.end())
			{
				return static_cast<T*>(it->second);
			}
			return nullptr;
		}

	private:
		std::wstring _name;
		std::vector<std::unique_ptr<Component>> _components;
		std::unordered_map<std::type_index, Component*> _componentMap;
	};
}

#endif
