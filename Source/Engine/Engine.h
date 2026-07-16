#pragma once

namespace URay
{

class Scene;
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

private:
    Window* window = nullptr;
    Scene* scene = nullptr;

    Renderer* renderer = nullptr;
    RenderPipeline* renderPipeline = nullptr;

    Timer* timer = nullptr;

    MeshManager* meshManager = nullptr;
};

} // namespace URay
