#include "InputManager.h"

#include <cstring>

namespace URay
{

void InputManager::Update()
{
    std::memcpy(prevKeys, currKeys, sizeof(currKeys));
    std::memcpy(prevMouse, currMouse, sizeof(currMouse));

    mouseDeltaX = mouseX - prevMouseX;
    mouseDeltaY = mouseY - prevMouseY;

    prevMouseX = mouseX;
    prevMouseY = mouseY;

    scrollX = 0.0;
    scrollY = 0.0;
}

} // namespace URay
