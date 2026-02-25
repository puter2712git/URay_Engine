#include <windows.h>
#include <Engine.h>
#include <Core/Scene.h>
#include <Resource/Shader.h>
#include "TestComponent.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int cmdShow)
{
	URay::Engine engine;
	if (!engine.Initialize(hInstance))
	{
		return -1;
	}

	URay::Scene testScene;
	URay::Unit* testUnit = testScene.AddUnit();
	testUnit->AddComponent<TestComponent>();

	engine.SetCurrentScene(&testScene);

	engine.Run();
	engine.Finalize();

	return 0;
}