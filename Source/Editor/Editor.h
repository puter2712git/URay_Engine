#pragma once

namespace URay
{

class Engine;
class Renderer;

class Editor
{
public:
    Editor(Engine& engine, Renderer& renderer);
    ~Editor() = default;

public:
    bool Initialize();
    void Finalize();

    void Update(float deltaTime);
    void Render();

private:
    Engine& engine;
    Renderer& renderer;
};

} // namespace URay
