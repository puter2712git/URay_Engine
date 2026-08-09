#include "EditorApplication.h"

using namespace URay;

int main()
{
    EditorApplication app;

    if (!app.Initialize())
        return EXIT_FAILURE;

    app.Run();
    app.Finalize();

    return 0;
}
