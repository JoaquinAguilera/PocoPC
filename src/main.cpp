#include <glew/glew.h>
#include <glfw3/glfw3.h>

#include <cstdio>
#include <chrono>
#include <thread>

#include <render.h>
#include <device.h>
#include <fileloading.h>
#include <input.h>
#include <lua.h>
#include <util.h>
#include <cartridge.h>


//delete
#include <iostream>

namespace poco
{
    int main();
}

#if defined(PLATFORM_PC)
int main()
{
    poco::main();
}
#endif

int poco::main()
{
    std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

    device::init();
    input::init();
    file::init();
    render::init();
    lua::init("celeste.p8");

    //TODO Revise FPS Lock
    int targetMS = 21;

    render::clear();
    while (!device::shouldClose()) {
        a = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = a - b;

        if (work_time.count() < targetMS)
        {
            std::chrono::duration<double, std::milli> delta_ms(targetMS - work_time.count());
            auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
        }

        b = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> sleep_time = b - a;
        
        if (input::button_state(input::INPUT_RESET)) {
            lua::init("celeste.p8");
        }

        lua::update();

        render::render(0.f);
        device::swapbuffers();
    }
    lua::shutdown();
    render::shutdown();
    file::shutdown();
    input::shutdown();
    device::shutdown();

    return 1;
}