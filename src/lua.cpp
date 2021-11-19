#include <lua.h>
#define SOL_ALL_SAFETIES_ON 1
#include <lua/lua.hpp>
#include <sol/sol.hpp>

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

#include <render.h>
#include <input.h>
#include <cartridge.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>  
#include <time.h>  

#include <assert.h>

using namespace poco::lua;

namespace
{
    sol::state luaState;

    sol::protected_function initFunc;// = lua["_update"];
    sol::protected_function updateFunc;// = lua["_update"];
    sol::protected_function drawFunc;// = lua["_draw"];


    glm::vec3 colourmap[] =
    {
        glm::vec3(0, 0, 0),
        glm::vec3(0.113f, 0.168f, 0.325f),//
        glm::vec3(0.494f, 0.145f, 0.325f),//
        glm::vec3(0, 0.529f, 0.317f),//3
        glm::vec3(0.670f, 0.321f, 0.211f),//4
        glm::vec3(0.37f, 0.35f, 0.33f),//5
        glm::vec3(0.760f, 0.764f, 0.780f),//6
        glm::vec3(1, 0.945f, 0.909f),//7
        glm::vec3(1.f, 0.f, 0.3f),//8
        glm::vec3(1, 0.639f, 0),//9
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f),//10
        glm::vec3(1, 0.925f, 0.152f)//10
    };

    poco::Cartridge* cartridge;
}
                                                                         
namespace poco::lua::wrapper
{
    //GFX
    void rectfill(float x0, float y0, float x1, float y1, int col)
    {
        render::rect(glm::vec2((int)x0, (int)y0), glm::vec2((int)x1 - (int)x0, (int)y1 - (int)y0), colourmap[col]);
    }
    void circfill(float x, float y, float r, int col)
    {
        render::circle(glm::vec2((int)x, (int)y), (int)r, colourmap[col]);
    }
    void ovalfill(float x, float y, float x1, float y1, int col)
    {
        render::oval(glm::vec2((int)x, (int)y), glm::vec2((int)x1, (int)y1), colourmap[col]);
    }
    void circ(float x, float y, float r, int col = 8)
    {
        render::circleOutline(glm::vec2((int)x, (int)y), (int)r, colourmap[col]);
    }

    void line(float startx, float starty, float endx, float endy, int col) 
    {
        render::line(glm::vec2(startx, starty), glm::vec2(endx, endy), colourmap[col]);
    }

    void spr(float index, float xpos, float ypos, float width, float height, bool flipX, bool flipY)
    {
        render::spritesheet(cartridge->m_Spritesheet, (int)index, 
                            glm::vec2((int)xpos, (int)ypos), 
                            glm::vec2((int)width, (int)height), 
                            flipX, flipY);
    }
    //Default parameters need to be manually overloaded since they don't exist past compile time
    void spr2(float index, float xpos, float ypos) { spr(index, xpos, ypos, 1, 1, false, false); }

    bool fget(float index, float flag) { return cartridge->ReadFlag((int)index, (int)flag); }
    int mget(float x, float y) { return cartridge->ReadMap((int)x, (int)y); }

    void map(int cellx, int celly, int xpos, int ypos, int width, int height, int flag) {
        for (int i = cellx; i < cellx + width; i++)
        {
            for (int j = celly; j < celly + height; j++)
            {
                uint8_t SpriteIndex = cartridge->ReadMap(i, j);
                if (flag < 0 || cartridge->ReadFlag(SpriteIndex) & flag)
                {
                    glm::vec2 spritesize = cartridge->m_Spritesheet->getSpriteSize();
                    wrapper::spr2(SpriteIndex, xpos + (i - cellx) * spritesize.x, ypos + (j-celly) * spritesize.y);
                }
            }
        }
    }
    void map2(int cellx, int celly, int xpos, int ypos, int width, int height) { map(cellx, celly, xpos, ypos, width, height, -1); }

    //Input
    bool btn(int button)
    {
        return input::button_state(static_cast<input::input>(button));
    }

    void no() {};
    void no2(int i) {};
    void no3(int i, int b) {};
    void no4(int a, int b, int c) {};
    void no5(int a, int b, int c, int d) {};
    void no6(int a, int b, int c, int d, int e) {};

    //Math
    float cos(float angle) {return glm::cos(angle * 2 * glm::pi<double>()); }
    float sin(float angle) {return -glm::sin(angle * 2 * glm::pi<double>()); }

    float rnd(float val) { return rand() % (int)glm::max(val,1.0f); }
    float max(float a, float b) { return glm::max(a, b); }
    float min(float a, float b) { return glm::min(a, b); }
    float sqrt(float val) { return glm::sqrt(val); }
    int flr(float val) { return glm::floor(val); }
    float abs(float val) { return glm::abs(val); }
}

void poco::lua::init(std::string source) 
{
    cartridge = new Cartridge(source);
    luaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);

    //To-do move this to config and parse lua code from loaded cartridge through filesystem
    std::string path = "resources/" + source;

    srand(time(NULL));

    //Input
    luaState.set_function("btn", &wrapper::btn);
    luaState.set_function("btnp", &wrapper::btn);


    //GFX
    luaState.set_function("rectfill", &wrapper::rectfill);
    luaState.set_function("circfill", &wrapper::circfill);
    luaState.set_function("ovalfill", &wrapper::ovalfill);
    luaState.set_function("circ", &wrapper::circ);
    luaState.set_function("line", &wrapper::line);

    luaState.set_function("spr", sol::overload(&wrapper::spr, &wrapper::spr2));
    luaState.set_function("map", sol::overload(&wrapper::map, &wrapper::map2));

    luaState.set_function("sfx", sol::overload(&wrapper::no, &wrapper::no2, &wrapper::no3, &wrapper::no4, &wrapper::no5, &wrapper::no6));
    luaState.set_function("pal", sol::overload(&wrapper::no, &wrapper::no2, &wrapper::no3, &wrapper::no4, &wrapper::no5, &wrapper::no6));
    luaState.set_function("music", sol::overload(&wrapper::no, &wrapper::no2, &wrapper::no3, &wrapper::no4, &wrapper::no5, &wrapper::no6));

    luaState.set_function("fget", &wrapper::fget);
    luaState.set_function("mget", &wrapper::mget);

    //Math funcs (try to implement these into Lua lib)
    luaState.set_function("cos", &wrapper::cos);
    luaState.set_function("sin", &wrapper::sin);
    luaState.set_function("rnd", &wrapper::rnd);
    luaState.set_function("max", &wrapper::max);
    luaState.set_function("min", &wrapper::min);
    luaState.set_function("sqrt", &wrapper::sqrt);
    luaState.set_function("flr", &wrapper::flr);
    luaState.set_function("abs", &wrapper::abs);
    
    auto result2 = luaState.safe_script(cartridge->code, [](lua_State*, sol::protected_function_result pfr) {
        sol::error err = pfr;
        std::cout << "An error occurred: " << err.what() << std::endl;
        //assert(false);
        return pfr;
        });

    //Lua functions:
    initFunc = luaState["_init"];
    updateFunc = luaState["_update"];
    drawFunc = luaState["_draw"];



    initFunc();
}

void poco::lua::update()
{

    updateFunc();
    drawFunc();
}


void poco::lua::shutdown()
{



}