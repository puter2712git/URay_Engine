#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Resource/Shader.h"
#include "Resource/Texture.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace URay
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

	public:
		void AddShader(const std::string& name, std::unique_ptr<Shader>& shader);
		Shader* GetShader(const std::string& name) const;

		void AddTexture(const std::string& name, std::unique_ptr<Texture>& texture);
		Texture* GetTexture(const std::string& name) const;

	private:
		std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;
		std::unordered_map<std::string, std::unique_ptr<Texture>> _textures;
	};
}

#endif
