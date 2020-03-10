/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:07:19
*/

#pragma once

#include <SDL2/SDL_events.h> // for SDL_Event, SDL_EventType
#include <SDL2/SDL_stdinc.h> // for Uint32
#include <map> // for map
#include <set>
#include <vector>
#include <memory>

#define EVENT_REFRESH 0

class InputDevice;

//typedef std::function t_callback;
//typedef void (*t_callback)(SDL_Event* event, void *arg);
//typedef void(*t_callback)();

class Callback;

/** Handles InputDevice and events generated by the Engine
*/
class Events {
public:
    static void Add(InputDevice* device, SDL_EventType event_type);
    static void remove(InputDevice* device, SDL_EventType event_type);
    static int filter(void* e, SDL_Event* event);
    static int refresh(void);
    static void AddRefreshCallback(std::shared_ptr<Callback> callback);
    static void RemoveRefreshCallback(std::shared_ptr<Callback> callback);
    /** Add a refresh callback to the list, returns the index */
    //template<typename F, typename T>
    //static size_t AddRefreshCallback(const F &callback, const T &argument = nullptr);
    //template<typename F>
    //static void RemoveRefreshCallback(const F &callback);
    //template<typename F, typename T>
    //static void RemoveRefreshCallback(const F &callback, const T &argument);
    /** Remove the callback at the index */
    //static void RemoveRefreshCallback(size_t index);
    static double delta_time();

private:
    static Events& _get();
    static Events* _instance;
    double _delta_time { 0 };
    /** pair of std::bind result and a the function address */
    std::vector<std::shared_ptr<Callback>> _rcallbacks;
    std::map<Uint32, std::set<InputDevice*>> _input_devices;
    Events();
};

/*
#include <iostream>

template<typename F, typename T>
size_t Events::AddRefreshCallback(const F &callback, const T &argument)
{
    std::cout << __FUNCTION__ << " " << (void*&)callback << " " << argument << std::endl;
    _get()._rcallbacks.push_back(std::pair(std::bind(callback, argument), std::pair((void*&)callback, (void*)&argument)));
    //_get()._rcallbacks.push_back(std::pair([callback, argument] { callback(argument); }, (void*&)callback));
    return _get()._rcallbacks.size() - 1;
}

template<typename F>
void Events::RemoveRefreshCallback(const F &callback)
{
    std::cout << __FUNCTION__ << " " << (void*&)callback << std::endl;
    _get()._rcallbacks.erase(
        std::remove_if(
            _get()._rcallbacks.begin(),
            _get()._rcallbacks.end(),
            [callback](const auto &it) {
                if (it.second.first == (void*&)callback)
                    std::cout << "FOUND FUNCTION !" << std::endl;
                return it.second.first == (void*&)callback;
            }),
        _get()._rcallbacks.end());
    //TODO : Enable this when C++20 is out
    //std::erase_if(_get()._rcallbacks, [callback](auto it) { return it->second == reinterpret_cast<size_t>(&callback); });
}

template<typename F, typename T>
void Events::RemoveRefreshCallback(const F &callback, const T &argument)
{
    std::cout << __FUNCTION__ << " " << (void*&)callback << " " << argument << std::endl;
    _get()._rcallbacks.erase(
        std::remove_if(
            _get()._rcallbacks.begin(),
            _get()._rcallbacks.end(),
            [callback, argument](const auto &it) {
                if (it.second.first == (void*&)callback && it.second.second == (void*)&argument)
                    std::cout << "FOUND FUNCTION !" << std::endl;
                return it.second.first == (void*&)callback;
            }),
        _get()._rcallbacks.end());
    //TODO : Enable this when C++20 is out
    //std::erase_if(_get()._rcallbacks, [callback](auto it) { return it->second == reinterpret_cast<size_t>(&callback); });
}
*/
