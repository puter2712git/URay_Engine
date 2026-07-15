#include "Engine/Engine.h"

int main()
{
    using namespace URay;

    Engine* engine = new Engine();
    if (!engine->Initialize())
        return 1;

    engine->Run();

    engine->Finalize();

    delete engine;

    return 0;
}
