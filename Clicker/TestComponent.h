#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H

#include <Core/Component.h>
#include <Component/PrimitiveRenderer.h>

class TestComponent : public URay::Component
{
public:
	TestComponent() = default;
	~TestComponent() override = default;

public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;

private:
	URay::PrimitiveRenderer* _primitiveRenderer = nullptr;
};

#endif
