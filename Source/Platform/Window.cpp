#include "Window.h"

namespace URay
{

bool Window::Initialize()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindow = glfwCreateWindow(800, 600, "URay Engine", nullptr, nullptr);

    return true;
}

void Window::Finalize()
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

} // namespace URay
