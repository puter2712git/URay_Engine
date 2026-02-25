#include "Core/ResourceManager.h"
#include "Engine.h"
#include <iostream>

namespace URay
{
	void ResourceManager::AddShader(const std::string& name, const wchar_t* shaderPath)
	{
		if (_shaders.find(name) != _shaders.end())
		{
			return;
		}

		Engine* engine = Engine::GetInstance();
		Renderer* renderer = engine->GetRenderer();

		auto shader = renderer->CreateShader(shaderPath);
		_shaders[name] = std::move(shader);
	}

	Shader* ResourceManager::GetShader(const std::string& name) const
	{
		auto it = _shaders.find(name);
		if (it != _shaders.end())
		{
			return it->second.get();
		}
		else
		{
			std::cout << "Shader not found: " << name << std::endl;
			return nullptr;
		}
	}
}
