#include <windows.h>
#include <Engine.h>
#include <Core/Scene.h>
#include <Component/Transform.h>
#include <Component/Camera.h>
#include <Component/PrimitiveRenderer.h>
#include <Resource/Shader.h>
#include "TestComponent.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int cmdShow)
{
	URay::Engine engine;
	if (!engine.Initialize(hInstance))
	{
		return -1;
	}

	URay::ResourceManager* resourceManager = engine.GetResourceManager();
	resourceManager->AddShader("default", L"D:/repos/URay/Clicker/TestShader.hlsl");

	URay::Scene testScene;

	URay::Unit* cameraUnit = testScene.AddUnit();
	URay::Transform* cameraTransform = cameraUnit->AddComponent<URay::Transform>();
	URay::Camera* camera = cameraUnit->AddComponent<URay::Camera>();
	cameraTransform->SetPosition(URay::Vector3(0.0f, 0.0f, -10.0f));
	testScene.SetMainCamera(camera);

	URay::Unit* triangleUnit = testScene.AddUnit();
	URay::Transform* triangleTransform = triangleUnit->AddComponent<URay::Transform>();
	URay::PrimitiveRenderer* triangleRenderer = triangleUnit->AddComponent<URay::PrimitiveRenderer>();
	triangleTransform->SetScale(URay::Vector3(100.0f, 100.0f, 100.0f));
	triangleRenderer->SetPrimitiveType(URay::PrimitiveType::Square);

	engine.SetCurrentScene(&testScene);
	engine.Run();
	engine.Finalize();

	return 0;
}
