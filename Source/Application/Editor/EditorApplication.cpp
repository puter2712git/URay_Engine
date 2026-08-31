#include "EditorApplication.h"

#include "Core/Performance/PerformanceAnalytics.h"
#include "Editor/Editor.h"
#include "Engine/Engine.h"

#include "Platform/Window/Window.h"

namespace URay
{

EditorApplication::EditorApplication() = default;
EditorApplication::~EditorApplication() = default;

bool EditorApplication::Initialize(
    const std::string& enginePath,
    const std::string& projectPath)
{
    engine = std::make_unique<Engine>();
    if (!engine->Initialize(enginePath, projectPath))
        return false;

    editor = std::make_unique<Editor>(*engine);
    if (!editor->Initialize())
        return false;

    return true;
}

void EditorApplication::Run()
{
    PerformanceAnalytics& analytics = engine->GetPerformanceAnalytics();

    while (!glfwWindowShouldClose(engine->GetWindow().GetGLFWWindow()))
    {
        analytics.BeginFrame();

        engine->Update();
        editor->Update();

        engine->BeginRender();
        editor->PrepareRender();

        engine->Render();

        analytics.EndFrame();
    }
}

void EditorApplication::Finalize()
{
    if (editor)
    {
        editor->Finalize();
        editor.reset();
    }

    if (engine)
    {
        engine->Finalize();
        engine.reset();
    }
}

} // namespace URay
