#include <glfw3/glfw3.h>
#include <map>

#include <input.h>
#include <device_pc.h>
using namespace poco::input;

namespace
{
    std::map<input, int> keyboardDictionary = {
            {input::INPUT_UP, GLFW_KEY_UP},
            {input::INPUT_LEFT, GLFW_KEY_LEFT},
            {input::INPUT_DOWN, GLFW_KEY_DOWN},
            {input::INPUT_RIGHT, GLFW_KEY_RIGHT},
            {input::INPUT_CONFIRM, GLFW_KEY_C},
            {input::INPUT_CANCEL, GLFW_KEY_X},
            {input::INPUT_RESET, GLFW_KEY_R}
    };
}

void poco::input::init()
{

}

void poco::input::shutdown()
{

}

bool poco::input::button_state(input Input)
{
    std::map<input, int>::const_iterator found = keyboardDictionary.find(Input);
    if (found == keyboardDictionary.end())
    {
        //assert
        return false;
    }
    else
    {
        return glfwGetKey(device::get_window(), found->second) == GLFW_PRESS;
    }
}

bool poco::input::button_pressed(input Input)
{
    return false;
}