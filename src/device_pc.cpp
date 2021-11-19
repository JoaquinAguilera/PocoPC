#include <device.h>
#include <device_pc.h>
#include <glew/glew.h>

#include <glfw3/glfw3.h>
#include <cstdio>

namespace
{
    GLFWwindow* window;

}

using namespace poco::device;

void poco::device::init()
{
    /* Initialize the library */
    if (!glfwInit())
        return;

        //TODO Assert


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(720, 720, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        //TODO Assert
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    };
}

void poco::device::shutdown()
{
    glfwTerminate();

}

bool poco::device::shouldClose()
{
    return glfwWindowShouldClose(window);
}

void poco::device::swapbuffers()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

GLFWwindow* poco::device::get_window()
{
    return window;
}

glm::vec2 poco::device::getWindowSize() {
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    return glm::vec2(width, height);
}