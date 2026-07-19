#include "Window.h"

namespace URay
{

bool Window::Initialize()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindow = glfwCreateWindow(800, 600, "URay Engine", nullptr, nullptr);

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    return true;
}

void Window::Finalize()
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

} // namespace URay
