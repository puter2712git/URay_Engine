#ifndef PRIMITIVE_RENDERER_H
#define PRIMITIVE_RENDERER_H

#include "Core/Component.h"
#include "Renderer.h"
#include "Resource/Shader.h"
#include <d3d11.h>
#include <string>

namespace URay
{
	enum class PrimitiveType
	{
		Triangle,
		Square
	};

	class PrimitiveRenderer : public Component
	{
	public:
		PrimitiveRenderer() = default;
		~PrimitiveRenderer() = default;

		void Initialize() override;
		void Render() override;

		PrimitiveType GetPrimitiveType() const { return _primitiveType; }
		void SetPrimitiveType(PrimitiveType primitiveType);

	private:
		PrimitiveType _primitiveType = PrimitiveType::Triangle;

		std::string _shaderName;
		std::string _meshName;

		Shader* _cachedShader = nullptr;
		Mesh* _cachedMesh = nullptr;
	};
}

#endif
