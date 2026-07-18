#pragma once

namespace URay
{

class Renderer;

class Editor
{
public:
    Editor(Renderer& renderer);
    ~Editor() = default;

public:
    bool Initialize();
    void Finalize();

    void Update(float deltaTime);
    void Render();

private:
    Renderer& renderer;
};

} // namespace URay
