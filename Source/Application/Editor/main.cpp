#include "EditorApplication.h"

#include <iostream>
#include <string>

using namespace URay;

int main(int argc, char** argv)
{
    EditorApplication app;

    std::string enginePath;
    std::string projectPath;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];

        if (arg == "--engine" && i + 1 < argc)
        {
            enginePath = argv[++i];
        }
        else if (arg == "--project" && i + 1 < argc)
        {
            projectPath = argv[++i];
        }
    }

    if (enginePath.empty() || projectPath.empty())
    {
        std::cerr
            << "Usage: URay_Engine.exe "
            << "--engine <engine-path> "
            << "--project <project-path>\n";

        return EXIT_FAILURE;
    }

    if (!app.Initialize(enginePath, projectPath))
        return EXIT_FAILURE;

    app.Run();
    app.Finalize();

    return 0;
}
