#pragma once

namespace URay
{

class Font;
class Mesh;
class Material;
class Texture;

struct DefaultAssets
{
    Font* font = nullptr;
    Mesh* quadMesh = nullptr;
    Mesh* cubeMesh = nullptr;
    Mesh* arrowMesh = nullptr;
    Mesh* rotationGizmoMesh = nullptr;
    Mesh* scaleGizmoMesh = nullptr;
    Material* spriteMaterial = nullptr;
    Material* meshMaterial = nullptr;
    Material* decalMaterial = nullptr;
    Texture* whiteTexture = nullptr;
    Texture* fontTexture = nullptr;
    Texture* decalTexture = nullptr;
};

} // namespace URay
