#pragma once

namespace URay
{

class Unit;
class Renderer;
class Window;

class Engine
{
public:
    Engine() = default;
    ~Engine();

public:
    bool Initialize();
    void Run();
    void Finalize();

private:
    Window* window = nullptr;
    Renderer* renderer = nullptr;
    Unit* unit = nullptr;
};

} // namespace URay
