#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Core/Component.h"
#include <string>

namespace URay
{
	class Shader;
	class Texture;
	struct Mesh;

	class SpriteRenderer : public Component
	{
	public:
		SpriteRenderer() = default;
		virtual ~SpriteRenderer() = default;

		virtual void Render() override;

		void SetTexture(Texture* texture) { _texture = texture; }
		Texture* GetTexture() const { return _texture;  }

	private:
		std::string _shaderName = "sprite";
		std::string _meshName = "quad";

		Shader* _cachedShader = nullptr;
		Mesh* _cachedMesh = nullptr;
		Texture* _texture = nullptr;
	};
}

#endif
