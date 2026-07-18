#pragma once

#include "Core/Input/InputManager.h"

namespace URay
{

class Scene;
class CameraComponent;
class Unit;

class Window;
class Timer;
class MeshManager;

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

    Scene* GetScene() const
    {
        return scene;
    }

    Editor* GetEditor() const
    {
        return editor;
    }

private:
    void UpdateCameraMovement(float deltaTime);
    void UpdateCameraRotation(float deltaTime);

    void UpdatePick();

private:
    Window* window = nullptr;
    Scene* scene = nullptr;
    CameraComponent* camera = nullptr;

    Renderer* renderer = nullptr;
    RenderPipeline* renderPipeline = nullptr;

    Editor* editor = nullptr;

    Timer* timer = nullptr;

    InputManager inputManager;

    MeshManager* meshManager = nullptr;
};

extern Engine* gEngine;

} // namespace URay
