#pragma once

namespace URay
{

class Engine;
class Renderer;
class Unit;

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

    void SelectUnit(Unit* unit);

private:
    void ShowTestPanel() const;
    void ShowStatus() const;
    void ShowInspector() const;

private:
    Engine& engine;
    Renderer& renderer;

    Unit* selectedUnit = nullptr;
};

} // namespace URay
