#pragma once

#include "Scene/SceneType.h"

#include "Core/Input/InputManager.h"
#include "Core/Math/Vector3.h"

#include <vector>

namespace URay
{

class Scene;
class CameraComponent;
class GizmoComponent;
class Unit;

class Window;
class Timer;

class MaterialManager;
class MeshManager;
class TextureManager;
class FontManager;

class ShaderManager;
class GPUResourceManager;
class Renderer;
class RenderPipeline;

class Editor;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

public:
    bool Initialize();
    void Run();
    void Finalize();

    void SpawnUnit(Unit* unit);

    void SetGameScene(Scene* gameScene);

    void GetWindowSize(int& width, int& height) const;
    void GetFramebufferSize(int& width, int& height) const;

    InputManager& GetInputManager() { return inputManager; }

    MeshManager* GetMeshManager() const { return meshManager; }

    TextureManager* GetTextureManager() const { return textureManager; }

    MaterialManager* GetMaterialManager() const { return materialManager; }

    FontManager* GetFontManager() const { return fontManager; }

    GPUResourceManager* GetGPUResourceManager() const;

    Scene* GetSceneByType(SceneType type) const;

    Editor* GetEditor() const { return editor; }

    Timer* GetTimer() const { return timer; }

    GizmoComponent* GetGizmo() const { return gizmo; }

    CameraComponent* GetCamera() const { return camera; }

private:
    void UpdateCameraMovement(float deltaTime);
    void UpdateCameraRotation(float deltaTime);

    void UpdateHover();
    void UpdatePick();

    bool PickGizmo(const Vector3& start, const Vector3& dir, int& outAxis);

private:
    Window* window = nullptr;

    std::vector<Scene*> scenes;
    CameraComponent* camera = nullptr;
    GizmoComponent* gizmo = nullptr;

    Renderer* renderer = nullptr;
    RenderPipeline* renderPipeline = nullptr;

    Editor* editor = nullptr;

    Timer* timer = nullptr;

    InputManager inputManager;

    ShaderManager* shaderManager = nullptr;
    MaterialManager* materialManager = nullptr;
    MeshManager* meshManager = nullptr;
    TextureManager* textureManager = nullptr;
    FontManager* fontManager = nullptr;
};

extern Engine* gEngine;

} // namespace URay
