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
#include "BallUnit.h"

void CreateResources()
{
	URay::Engine* engine = URay::Engine::GetInstance();
	URay::Renderer* renderer = engine->GetRenderer();

	auto defaultShader = renderer->CreateShader(L"D:/repos/URay/Clicker/TestShader.hlsl");
	auto spriteShader = renderer->CreateShader(L"D:/repos/URay/Clicker/SpriteShader.hlsl");
	auto ballTexture = std::make_unique<URay::Texture>();
	ballTexture->Create(L"D:/repos/URay/Clicker/ball.png");

	URay::ResourceManager* resourceManager = engine->GetResourceManager();
	resourceManager->AddShader("default", defaultShader);
	resourceManager->AddShader("sprite", spriteShader);
	resourceManager->AddTexture("ball", ballTexture);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int cmdShow)
{
	URay::Engine engine;
	if (!engine.Initialize(hInstance))
	{
		return -1;
	}

	CreateResources();

	URay::Scene testScene;

	URay::Unit* cameraUnit = testScene.AddUnit();
	URay::Transform* cameraTransform = cameraUnit->AddComponent<URay::Transform>();
	URay::Camera* camera = cameraUnit->AddComponent<URay::Camera>();
	cameraTransform->SetPosition(URay::Vector3(0.0f, 0.0f, -10.0f));
	testScene.SetMainCamera(camera);

	BallUnit* ballUnit = new BallUnit();
	testScene.AddUnit(ballUnit);

	engine.SetCurrentScene(&testScene);
	engine.Run();
	engine.Finalize();

	return 0;
}
