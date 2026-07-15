#pragma once

namespace URay
{

class Scene;
class Renderer;
class Window;
class Timer;
class MeshManager;

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
    Renderer* renderer = nullptr;
    Scene* scene = nullptr;

    Timer* timer = nullptr;

    MeshManager* meshManager = nullptr;
};

} // namespace URay
