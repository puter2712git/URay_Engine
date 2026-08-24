#pragma once

#include "Scene/SceneType.h"

#include "Core/Input/InputManager.h"
#include "Core/Performance/ScopeTimer.h"

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
class PerformanceAnalytics;

class MaterialManager;
class MeshManager;
class TextureManager;
class FontManager;
class ObjImporter;
class TextureImporter;
class AssetPipeline;

namespace Render
{
class ShaderManager;
class GPUResourceManager;
class Renderer;
class RenderPipeline;
} // namespace Render

class Editor;

class Engine
{
public:
    Engine();
    ~Engine();

public:
    bool Initialize(const std::string& projectPath);
    void Finalize();

    void Update();

    void BeginRender();
    void PrepareRender();
    void Render();
    void EndRender();

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

    Render::Renderer* GetRenderer() const { return renderer; }

    Render::RenderPipeline* GetRenderPipeline() const { return renderPipeline; }

    Render::GPUResourceManager* GetGPUResourceManager() const;

    Scene* GetSceneByType(SceneType type) const;

    Timer* GetTimer() const { return timer; }

    VirtualFilesystem* GetFilesystem() const { return filesystem; }

    PerformanceAnalytics* GetPerformanceAnalytics() const { return performanceAnalytics.get(); }

private:
    Window* window = nullptr;

    std::vector<Scene*> scenes;

    Render::Renderer* renderer = nullptr;
    Render::RenderPipeline* renderPipeline = nullptr;

    Timer* timer = nullptr;

    VirtualFilesystem* filesystem = nullptr;

    InputManager inputManager;

    std::unique_ptr<PerformanceAnalytics> performanceAnalytics = nullptr;

    Render::ShaderManager* shaderManager = nullptr;
    MaterialManager* materialManager = nullptr;
    MeshManager* meshManager = nullptr;
    TextureManager* textureManager = nullptr;
    FontManager* fontManager = nullptr;

    ObjImporter* objImporter = nullptr;

    AssetPipeline* assetPipeline = nullptr;
};

extern Engine* gEngine;

#define URAY_PROFILE_SCOPE(name) \
    ::URay::ScopeTimer _urayScopeTimer(*gEngine->GetPerformanceAnalytics(), name);

} // namespace URay
