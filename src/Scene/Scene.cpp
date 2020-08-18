/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 20:57:52
*/

#include "Scene/Scene.hpp"
#include "Animation/Animation.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Engine.hpp"
#include "Light/Light.hpp"
#include "Node.hpp"
#include "Physics/RigidBody.hpp"
#include "Transform.hpp"
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <algorithm>
#include <iostream>

std::shared_ptr<Scene> Scene::Create(const std::string& name)
{
    return std::shared_ptr<Scene>(new Scene(name));
}

Scene::Scene(const std::string& name)
    : _name(name)
{
}

std::string Scene::Name() const
{
    return _name;
}

void Scene::SetName(const std::string& name)
{
    _name = name;
}

void Scene::_AddNodeChildren(std::shared_ptr<Node> node)
{
    for (auto index(0u); index < node->ChildCount(); ++index) {
        auto child(node->GetChild(index));
        if (std::find(_nodes.begin(), _nodes.end(), child) == _nodes.end())
            _nodes.push_back(child);
        else
            return;
        if (auto camera = std::dynamic_pointer_cast<Camera>(child); camera != nullptr)
            AddComponent(camera);
        else if (auto light = std::dynamic_pointer_cast<Light>(child); light != nullptr)
            AddComponent(light);
        _AddNodeChildren(child);
    }
}

void Scene::AddRootNode(std::shared_ptr<Node> node)
{
    _nodes.push_back(node);
    _rootNodes.push_back(node);
    _AddNodeChildren(node);
}

void Scene::Add(std::shared_ptr<RigidBody> rigidBody)
{
    _physicsEngine.AddRigidBody(rigidBody);
}

void Scene::Add(std::shared_ptr<Node> node)
{
    if (node == nullptr)
        return;
    if (std::find(_nodes.begin(), _nodes.end(), node) == _nodes.end()) {
        _nodes.push_back(node);
        _rootNodes.push_back(node);
    } else
        return;
    if (auto camera = std::dynamic_pointer_cast<Camera>(node); camera != nullptr)
        AddComponent(camera);
    else if (auto light = std::dynamic_pointer_cast<Light>(node); light != nullptr)
        AddComponent(light);
}

void Scene::Add(std::shared_ptr<Animation> animation)
{
    AddComponent(animation);
}

void NodesFixedUpdateCPU(std::shared_ptr<Node> rootNode, float delta)
{
    if (rootNode == nullptr)
        return;
    rootNode->FixedUpdateCPU(delta);
}

void Scene::_FixedUpdateCPU(float delta)
{
    Common::SetUp(Up());
    PhysicsUpdate(delta);
    for (auto& node : RootNodes())
        NodesFixedUpdateCPU(node, delta);
}

void NodesFixedUpdateGPU(std::shared_ptr<Node> rootNode, float delta)
{
    if (rootNode == nullptr)
        return;
    rootNode->FixedUpdateGPU(delta);
}

void Scene::_FixedUpdateGPU(float delta)
{
    for (auto& node : RootNodes())
        NodesFixedUpdateGPU(node, delta);
}

void Scene::PhysicsUpdate(float delta)
{
    _physicsEngine.Simulate(delta);
    _physicsEngine.CheckCollision();
    /*for (auto &node : RootNodes())
		NodesPhysicsUpdate(node);*/
    /*static auto time(SDL_GetTicks());
    time = SDL_GetTicks();
    static auto lastTime(time);
    _physicsEngine.Simulate((time - lastTime) / 1000.f);
    lastTime = time;
    _physicsEngine.CheckCollision();*/
    //for (auto &node : RootNodes())
    //	NodesCollisionCheck(node);
}

void NodesUpdateGPU(std::shared_ptr<Node> rootNode, float delta)
{
    if (rootNode == nullptr)
        return;
    rootNode->UpdateGPU(delta);
}

void Scene::_UpdateGPU(float delta)
{
    for (auto& node : RootNodes())
        NodesUpdateGPU(node, delta);
}

void NodesUpdateCPU(std::shared_ptr<Node> rootNode, float delta)
{
    if (rootNode == nullptr)
        return;
    rootNode->UpdateCPU(delta);
}

void Scene::_UpdateCPU(float delta)
{
    for (auto& node : RootNodes())
        NodesUpdateCPU(node, delta);
}

void DrawNodes(std::shared_ptr<Node> rootNode, RenderMod mode)
{
    if (rootNode == nullptr)
        return;
    rootNode->Draw(mode);
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        DrawNodes(rootNode->GetChild(index), mode);
}

void Scene::Render(const RenderMod& mode)
{
    for (auto node : Scene::Current()->RootNodes())
        DrawNodes(node, mode);
}

void DrawNodesDepth(std::shared_ptr<Node> rootNode, RenderMod mode)
{
    if (rootNode == nullptr)
        return;
    rootNode->DrawDepth(mode);
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        DrawNodesDepth(rootNode->GetChild(index), mode);
}

void Scene::RenderDepth(const RenderMod& mode)
{
    for (auto node : Scene::Current()->RootNodes())
        DrawNodesDepth(node, mode);
}

std::shared_ptr<Scene>& CurrentScene()
{
    static std::shared_ptr<Scene> current(nullptr);
    return current;
}

std::shared_ptr<Scene> Scene::Current()
{
    return CurrentScene();
}

void Scene::SetCurrent(std::shared_ptr<Scene> scene)
{
    CurrentScene() = scene;
}

std::shared_ptr<Camera> Scene::CurrentCamera() const
{
    return _currentCamera;
}

void Scene::SetCurrentCamera(std::shared_ptr<Camera> camera)
{
    auto cameras = Cameras();
    if (std::find(cameras.begin(), cameras.end(), camera) == cameras.end())
        Add(camera);
    _currentCamera = camera;
}

template <typename T>
std::shared_ptr<T> GetByName(const std::string& name, const std::shared_ptr<T> rootNode)
{
    if (rootNode == nullptr || rootNode->Name() == name)
        return std::dynamic_pointer_cast<T>(rootNode);
    //for (const auto object : Nodes()) {
    //	auto result(std::dynamic_pointer_cast<T>(GetByName<Node>(name, object)));
    //	if (result != nullptr)
    //		return result;
    //}
    return nullptr;
}

template <typename T>
std::shared_ptr<T> GetByPointer(std::shared_ptr<Node> node, const std::shared_ptr<T> rootNode)
{
    if (rootNode == nullptr || rootNode == node)
        return std::dynamic_pointer_cast<T>(rootNode);
    //for (const auto object : Nodes()) {
    //	auto result(std::dynamic_pointer_cast<T>(GetByPointer<Node>(node, object)));
    //	if (result != nullptr)
    //		return result;
    //}
    return nullptr;
}

std::shared_ptr<Node> Scene::GetNode(std::shared_ptr<Node> node) const
{
    for (const auto& object : _nodes) {
        auto result(GetByPointer(node, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

std::shared_ptr<Node> Scene::GetNodeByName(const std::string& name) const
{
    for (const auto& object : _nodes) {
        auto result(GetByName(name, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

std::shared_ptr<Light> Scene::GetLightByName(const std::string& name) const
{
    for (const auto& object : Lights()) {
        auto result(GetByName(name, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

std::shared_ptr<Camera> Scene::GetCameraByName(const std::string& name) const
{
    for (const auto& object : Cameras()) {
        auto result(GetByName(name, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Node>>& Scene::RootNodes()
{
    return _rootNodes;
}

const std::vector<std::shared_ptr<Node>>& Scene::Nodes()
{
    return _nodes;
}

const std::vector<std::shared_ptr<Light>> Scene::Lights() const
{
    return GetComponents<Light>();
}

const std::vector<std::shared_ptr<Camera>> Scene::Cameras() const
{
    return GetComponents<Camera>();
}

const std::vector<std::shared_ptr<Animation>> Scene::Animations() const
{
    return GetComponents<Animation>();
}

glm::vec3 Scene::Up() const
{
    return _up;
}

void Scene::SetUp(glm::vec3 up)
{
    _up = up;
}

std::shared_ptr<AABB> Scene::GetLimits() const
{
    return _aabb;
}