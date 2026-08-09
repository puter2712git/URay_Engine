#include "EditorApplication.h"

#include "Editor/Editor.h"
#include "Engine/Engine.h"

namespace URay
{

EditorApplication::EditorApplication() = default;
EditorApplication::~EditorApplication() = default;

bool EditorApplication::Initialize()
{
    engine = std::make_unique<Engine>();
    if (!engine->Initialize())
        return false;

    editor = std::make_unique<Editor>(*engine);
    if (!editor->Initialize())
        return false;

    return true;
}

void EditorApplication::Run()
{
    while (!shouldQuit)
    {
        engine->Update();
        editor->Update();

        engine->PrepareRender();
        editor->PrepareRender();

        engine->Render();
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
