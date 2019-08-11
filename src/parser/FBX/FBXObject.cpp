/*
* @Author: gpinchon
* @Date:   2019-08-10 20:03:15
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 20:49:46
*/

#include "parser/FBX/FBXObject.hpp"

using namespace FBX;

std::map<int64_t, std::shared_ptr<Object>> Object::s_objects;

std::string Object::Name() const
{
    return _name;
}

void Object::SetName(const std::string& name)
{
    _name = name;
}

void Object::AddChild(std::shared_ptr<Object> child)
{
    children.push_back(child);
}

std::shared_ptr<Object> Object::Get(int64_t id)
{
    return s_objects[id];
}

void Object::Add(int64_t id, std::shared_ptr<Object> object)
{
    s_objects[id] = object;
}