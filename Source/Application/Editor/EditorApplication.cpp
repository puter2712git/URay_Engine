#include "EditorApplication.h"

#include "Editor/Editor.h"
#include "Engine/Engine.h"

#include "Platform/Window.h"

namespace URay
{

EditorApplication::EditorApplication() = default;
EditorApplication::~EditorApplication() = default;

bool EditorApplication::Initialize(const std::string& projectPath)
{
    engine = std::make_unique<Engine>();
    if (!engine->Initialize(projectPath))
        return false;

    editor = std::make_unique<Editor>(*engine);
    if (!editor->Initialize())
        return false;

    return true;
}

void EditorApplication::Run()
{
    while (!glfwWindowShouldClose(engine->GetWindow()->GetGLFWWindow()))
    {
        engine->Update();
        editor->Update();

        engine->BeginRender();
        engine->PrepareRender();
        editor->PrepareRender();

        engine->Render();
        engine->EndRender();
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
