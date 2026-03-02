#include "BallUnit.h"
#include <Component/Transform.h>
#include <Component/SpriteRenderer.h>
#include <Component/Rigidbody.h>
#include <Engine.h>
#include <Core/ResourceManager.h>

BallUnit::BallUnit()
{
	URay::Engine* engine = URay::Engine::GetInstance();
	URay::ResourceManager* resourceManager = engine->GetResourceManager();

	URay::Transform* transform = AddComponent<URay::Transform>();
	URay::SpriteRenderer* spriteRenderer = AddComponent<URay::SpriteRenderer>();
	URay::Rigidbody* rigidbody = AddComponent<URay::Rigidbody>();
	transform->SetScale(URay::Vector3(200.0f, 200.0f, 200.0f));

	URay::Texture* ballTexture = resourceManager->GetTexture("ball");
	spriteRenderer->SetTexture(ballTexture);

	rigidbody->SetVelocity(URay::Vector3(100.0f, 0.0f, 0.0f));
}

BallUnit::~BallUnit()
{
}
