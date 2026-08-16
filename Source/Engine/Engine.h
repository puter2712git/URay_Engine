#pragma once

#include "Scene/SceneType.h"

#include "Core/Input/InputManager.h"

#include <string>
#include <vector>

namespace URay
{

class Scene;
class CameraComponent;
class GizmoComponent;
class Unit;

class Window;
class Timer;
class VirtualFilesystem;

class MaterialManager;
class MeshManager;
class TextureManager;
class FontManager;
class ObjImporter;
class TextureImporter;
class AssetPipeline;

namespace RHI
{
class ShaderManager;
class GPUResourceManager;
class Renderer;
class RenderPipeline;
} // namespace RHI

class Editor;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

public:
    bool Initialize(const std::string& projectPath);
    void Finalize();

    void Update();
    void PrepareRender();
    void Render();

    void SpawnUnit(Unit* unit);

    void AddScene(Scene* scene);
    void SetGameScene(Scene* gameScene);

    void GetWindowSize(int& width, int& height) const;
    void GetFramebufferSize(int& width, int& height) const;

    Window* GetWindow() const { return window; }

    InputManager& GetInputManager() { return inputManager; }

    MeshManager* GetMeshManager() const { return meshManager; }

    TextureManager* GetTextureManager() const { return textureManager; }

    MaterialManager* GetMaterialManager() const { return materialManager; }

    FontManager* GetFontManager() const { return fontManager; }

    RHI::Renderer* GetRenderer() const { return renderer; }

    RHI::GPUResourceManager* GetGPUResourceManager() const;

    Scene* GetSceneByType(SceneType type) const;

    Timer* GetTimer() const { return timer; }

    VirtualFilesystem* GetFilesystem() const { return filesystem; }

private:
    Window* window = nullptr;

    std::vector<Scene*> scenes;

    RHI::Renderer* renderer = nullptr;
    RHI::RenderPipeline* renderPipeline = nullptr;

    Timer* timer = nullptr;

    VirtualFilesystem* filesystem = nullptr;

    InputManager inputManager;

    RHI::ShaderManager* shaderManager = nullptr;
    MaterialManager* materialManager = nullptr;
    MeshManager* meshManager = nullptr;
    TextureManager* textureManager = nullptr;
    FontManager* fontManager = nullptr;

    ObjImporter* objImporter = nullptr;

    AssetPipeline* assetPipeline = nullptr;
};

extern Engine* gEngine;

} // namespace URay
