#include <windows.h>
#include <Engine.h>
#include <Core/Scene.h>
#include <Component/Transform.h>
#include <Component/Camera.h>
#include <Component/PrimitiveRenderer.h>
#include <Component/SpriteRenderer.h>
#include <Resource/Shader.h>
#include <Resource/Texture.h>
#include "TestComponent.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int cmdShow)
{
	URay::Engine engine;
	if (!engine.Initialize(hInstance))
	{
		return -1;
	}

	URay::Renderer* renderer = engine.GetRenderer();

	auto spriteShader = renderer->CreateShader(L"D:/repos/URay/Clicker/SpriteShader.hlsl");
	auto ballTexture = std::make_unique<URay::Texture>();
	ballTexture->Create(L"D:/repos/URay/Clicker/ball.png");

	URay::ResourceManager* resourceManager = engine.GetResourceManager();
	resourceManager->AddShader("sprite", spriteShader);

	URay::Scene testScene;

	URay::Unit* cameraUnit = testScene.AddUnit();
	URay::Transform* cameraTransform = cameraUnit->AddComponent<URay::Transform>();
	URay::Camera* camera = cameraUnit->AddComponent<URay::Camera>();
	cameraTransform->SetPosition(URay::Vector3(0.0f, 0.0f, -10.0f));
	testScene.SetMainCamera(camera);

	//URay::Unit* triangleUnit = testScene.AddUnit();
	//URay::Transform* triangleTransform = triangleUnit->AddComponent<URay::Transform>();
	//URay::PrimitiveRenderer* triangleRenderer = triangleUnit->AddComponent<URay::PrimitiveRenderer>();
	//TestComponent* testComp = triangleUnit->AddComponent<TestComponent>();
	//triangleTransform->SetScale(URay::Vector3(100.0f, 100.0f, 100.0f));
	//triangleRenderer->SetPrimitiveType(URay::PrimitiveType::Triangle);

	URay::Unit* ballUnit = testScene.AddUnit();
	URay::Transform* ballTransform = ballUnit->AddComponent<URay::Transform>();
	URay::SpriteRenderer* ballRenderer = ballUnit->AddComponent<URay::SpriteRenderer>();
	TestComponent* testComp = ballUnit->AddComponent<TestComponent>();
	ballTransform->SetScale(URay::Vector3(200.0f, 200.0f, 200.0f));
	ballRenderer->SetTexture(ballTexture.get());

	engine.SetCurrentScene(&testScene);
	engine.Run();
	engine.Finalize();

	return 0;
}
