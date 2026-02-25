#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H

#include <Core/Component.h>

class TestComponent : public URay::Component
{
public:
	TestComponent() = default;
	~TestComponent() override = default;

public:
	void Initialize() override;
	void Update() override;
	void Render() override;
	void Finalize() override;
};

#endif
