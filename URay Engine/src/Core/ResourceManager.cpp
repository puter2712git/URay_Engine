#include "Core/ResourceManager.h"
#include "Engine.h"
#include <iostream>
#include <memory>

namespace URay
{
	void ResourceManager::AddShader(const std::string& name, std::unique_ptr<Shader>& shader)
	{
		if (_shaders.find(name) != _shaders.end())
		{
			return;
		}

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

	void ResourceManager::AddTexture(const std::string& name, std::unique_ptr<Texture>& texture)
	{
		if (_textures.find(name) != _textures.end())
		{
			return;
		}
		_textures[name] = std::move(texture);
	}

	Texture* ResourceManager::GetTexture(const std::string& name) const
	{
		auto it = _textures.find(name);
		if (it != _textures.end())
		{
			return it->second.get();
		}
		else
		{
			std::cout << "Texture not found: " << name << std::endl;
			return nullptr;
		}
	}
}
