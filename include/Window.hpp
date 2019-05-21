/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-21 13:55:02
*/

#pragma once

#include "GLIncludes.hpp"
#include "InputDevice.hpp"
#include <string>

class Window : InputDevice {
public:
    static void resize(const VEC2&);
    static VEC2 size();
    static void init(const std::string& name, VEC2 resolution);
    static void fullscreen(const bool& fullscreen);
    static void swap();
    static GLbitfield& clear_mask();
    static VEC4& clear_color();
    //static Framebuffer	&render_buffer();
    void process_event(SDL_Event*);
    static SDL_GLContext   context();
    static SDL_Window      *sdl_window();

private:
    static Window& _get();
    static Window* _instance;
    SDL_Window* _sdl_window{ nullptr };
    SDL_GLContext _gl_context{};
    VEC4 _clear_color{ 0, 0, 0, 0 };
    GLbitfield _clear_mask{};
    //Framebuffer		*_render_buffer{nullptr};
    Window();
};