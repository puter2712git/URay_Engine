#pragma once

#include "Core/Input/InputManager.h"
#include "Core/Math/Vector3.h"

namespace URay
{

class Scene;
class CameraComponent;
class GizmoComponent;
class Unit;

class Window;
class Timer;

class MeshManager;
class TextureManager;

class MaterialManager;
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

    void GetFramebufferSize(int& width, int& height) const;

    InputManager& GetInputManager() { return inputManager; }

    MeshManager* GetMeshManager() const { return meshManager; }

    TextureManager* GetTextureManager() const { return textureManager; }

    MaterialManager* GetMaterialManager() const;

    Scene* GetScene() const { return scene; }

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
    Scene* scene = nullptr;
    CameraComponent* camera = nullptr;
    GizmoComponent* gizmo = nullptr;

    Renderer* renderer = nullptr;
    RenderPipeline* renderPipeline = nullptr;

    Editor* editor = nullptr;

    Timer* timer = nullptr;

    InputManager inputManager;

    MeshManager* meshManager = nullptr;
    TextureManager* textureManager = nullptr;
};

extern Engine* gEngine;

} // namespace URay
