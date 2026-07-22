#pragma once

#include "Core/Input/InputManager.h"

namespace URay
{

class Scene;
class CameraComponent;
class GizmoComponent;
class Unit;

class Window;
class Timer;
class MeshManager;

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

    InputManager& GetInputManager()
    {
        return inputManager;
    }

    MeshManager* GetMeshManager() const
    {
        return meshManager;
    }

    MaterialManager* GetMaterialManager() const;

    Scene* GetScene() const
    {
        return scene;
    }

    Editor* GetEditor() const
    {
        return editor;
    }

    Timer* GetTimer() const
    {
        return timer;
    }

    GizmoComponent* GetGizmo() const
    {
        return gizmo;
    }

    uint32_t GetObjectId() const
    {
        return objectId;
    }
    void SetObjectId(uint32_t objId)
    {
        objectId = objId;
    }

private:
    void UpdateCameraMovement(float deltaTime);
    void UpdateCameraRotation(float deltaTime);

    void UpdatePick();

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

    uint32_t objectId = 0;
};

extern Engine* gEngine;

} // namespace URay
