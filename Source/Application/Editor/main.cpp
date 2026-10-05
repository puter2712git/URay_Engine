#include "EditorApplication.h"

#include "Application/CommandLine.h"

#include <iostream>
#include <string>

using namespace URay;

int main(int argc, char** argv)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    CommandLine commandLine(argc, argv);

    auto enginePath = commandLine.Get("--engine");
    auto projectPath = commandLine.Get("--project");

    if (!enginePath || !projectPath)
    {
        std::cerr
            << "Usage: URay_Engine.exe "
            << "--engine <engine-path> "
            << "--project <project-path>\n";

        return EXIT_FAILURE;
    }

    EditorApplication app;

    if (!app.Initialize(*enginePath, *projectPath))
        return EXIT_FAILURE;

    app.Run();
    app.Finalize();

    return 0;
}
