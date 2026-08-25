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
class AssetSystem;

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
    Timer* GetTimer() const { return timer; }
    InputManager& GetInputManager() { return inputManager; }

    Render::Renderer* GetRenderer() const { return renderer; }
    Render::RenderPipeline* GetRenderPipeline() const { return renderPipeline; }
    Render::GPUResourceManager* GetGPUResourceManager() const;

    Scene* GetSceneByType(SceneType type) const;

    PerformanceAnalytics* GetPerformanceAnalytics() const { return performanceAnalytics.get(); }

    AssetSystem& GetAssetSystem() const { return *assetSystem; }

private:
    Window* window = nullptr;

    std::vector<Scene*> scenes;

    Render::Renderer* renderer = nullptr;
    Render::RenderPipeline* renderPipeline = nullptr;

    Timer* timer = nullptr;
    InputManager inputManager;

    std::unique_ptr<PerformanceAnalytics> performanceAnalytics = nullptr;

    std::unique_ptr<AssetSystem> assetSystem = nullptr;
};

extern Engine* gEngine;

#define URAY_PROFILE_SCOPE(name) \
    ::URay::ScopeTimer _urayScopeTimer(*gEngine->GetPerformanceAnalytics(), name);

} // namespace URay
