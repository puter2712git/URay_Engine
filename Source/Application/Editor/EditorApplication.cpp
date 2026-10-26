#include "EditorApplication.h"
#include <Core/Type/TypeAlias.h>

#include <Core/Performance/PerformanceAnalytics.h>
#include <Editor/Editor.h>
#include <Engine/Engine.h>
#include <Platform/Window/Window.h>

namespace URay
{

EditorApplication::EditorApplication() = default;
EditorApplication::~EditorApplication() = default;

bool EditorApplication::Initialize(
    const String& enginePath,
    const String& projectPath)
{
    engine = std::make_unique<Engine>();
    if (!engine->Initialize(enginePath, projectPath))
    {
        Finalize();
        return false;
    }

    editor = std::make_unique<Editor>();
    if (!editor->Initialize())
    {
        Finalize();
        return false;
    }

    return true;
}

void EditorApplication::Run()
{
    Window& window = engine->GetWindow();
    PerformanceAnalytics& analytics = engine->GetPerformanceAnalytics();

    while (!window.ShouldClose())
    {
        analytics.BeginFrame();

        engine->Update();

        editor->Update();

        engine->BeginRender();

        engine->Render(editor->BuildRenderRequest());

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
