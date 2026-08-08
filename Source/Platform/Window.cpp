#include "Window.h"

namespace URay
{

bool Window::Initialize()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindow = glfwCreateWindow(mode->width, mode->height, "URay Engine", nullptr, nullptr);

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
