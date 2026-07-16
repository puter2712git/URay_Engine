#pragma once

#include "Core/Input/InputManager.h"

namespace URay
{

class Scene;
class CameraComponent;

class Window;
class Timer;
class MeshManager;

class Renderer;
class RenderPipeline;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

public:
    bool Initialize();
    void Run();
    void Finalize();

    InputManager& GetInputManager()
    {
        return inputManager;
    }

private:
    void UpdateCameraMovement(float deltaTime);

private:
    Window* window = nullptr;
    Scene* scene = nullptr;
    CameraComponent* camera = nullptr;

    Renderer* renderer = nullptr;
    RenderPipeline* renderPipeline = nullptr;

    Timer* timer = nullptr;

    InputManager inputManager;

    MeshManager* meshManager = nullptr;
};

extern Engine* gEngine;

} // namespace URay
