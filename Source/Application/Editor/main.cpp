#include "EditorApplication.h"

#include <string>

using namespace URay;

int main(int argc, char** argv)
{
    EditorApplication app;

    std::string projectPath;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--project" && i + 1 < argc)
        {
            projectPath = argv[++i];
        }
    }

    if (!app.Initialize(projectPath))
        return EXIT_FAILURE;

    app.Run();
    app.Finalize();

    return 0;
}
