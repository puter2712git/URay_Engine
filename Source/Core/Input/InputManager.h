#pragma once

#include <GLFW/glfw3.h>

namespace URay
{

class InputManager
{
public:
    void Update();

    bool GetKey(int key) const
    {
        return currKeys[key];
    }
    bool GetKeyDown(int key) const
    {
        return currKeys[key] && !prevKeys[key];
    }
    bool GetKeyUp(int key) const
    {
        return !currKeys[key] && prevKeys[key];
    }

    bool GetMouse(int mouse) const
    {
        return currMouse[mouse];
    }
    bool GetMouseDown(int mouse) const
    {
        return currMouse[mouse] && !prevMouse[mouse];
    }
    bool GetMouseUp(int mouse) const
    {
        return !currMouse[mouse] && prevMouse[mouse];
    }

public:
    bool currKeys[GLFW_KEY_LAST + 1];
    bool prevKeys[GLFW_KEY_LAST + 1];

    bool currMouse[GLFW_MOUSE_BUTTON_LAST + 1];
    bool prevMouse[GLFW_MOUSE_BUTTON_LAST + 1];

    double mouseX = 0.0;
    double mouseY = 0.0;

    double prevMouseX = 0.0;
    double prevMouseY = 0.0;

    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;

    double scrollX = 0.0;
    double scrollY = 0.0;
};

} // namespace URay
