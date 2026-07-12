#include "Component.h"

#include <iostream>

namespace URay
{

void Component::Update(float deltaTime)
{
    std::cout << "Component Update!" << std::endl;
}

} // namespace URay
