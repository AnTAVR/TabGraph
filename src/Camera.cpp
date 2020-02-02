/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:25:02
*/

#include "Camera.hpp"
#include "Common.hpp"
#include "Window.hpp" // for Window
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h> // for sin, cos

std::weak_ptr<Camera> Camera::_current;

Camera::Camera(const std::string& name, float ifov, CameraProjection proj)
    : Node(name)
{
    _fov = ifov;
    _projection_type = proj;
}

std::shared_ptr<Camera> Camera::Create(const std::string& name, float ifov, CameraProjection proj)
{
    std::shared_ptr<Camera> camera(new Camera(name, ifov, proj));
    Camera::Add(camera);
    return (camera);
}

std::shared_ptr<Camera> Camera::Create(std::shared_ptr<Camera> otherCamera)
{
    return std::shared_ptr<Camera>(new Camera(*otherCamera));
}

std::shared_ptr<Camera> Camera::GetById(int64_t id)
{
    return std::dynamic_pointer_cast<Camera>(Node::GetById(id));
}

std::shared_ptr<Camera> Camera::GetByName(const std::string& name)
{
    return std::dynamic_pointer_cast<Camera>(Node::GetByName(name));
}

std::shared_ptr<Camera> Camera::current()
{
    return (_current.lock());
}

glm::vec3 Camera::Forward() const
{
    return Rotation();
}

void Camera::SetForward(glm::vec3 forward)
{
    SetRotation(forward);
}

glm::vec3 Camera::Right() const
{
    return glm::cross(Common::Up(), Forward());
}

glm::vec3 Camera::Up() const
{
    return glm::cross(Forward(), Right());
}

void Camera::set_current(std::shared_ptr<Camera> camera)
{
    _current = camera;
}

void Camera::UpdateViewMatrix()
{
    UpdateTranslationMatrix();
    UpdateRotationMatrix();
    SetViewMatrix(TranslationMatrix() * RotationMatrix());
}

void Camera::UpdateProjectionMatrix()
{
    if (_projection_type == PerspectiveCamera)
        SetProjectionMatrix(glm::perspective(Fov(), float(Window::size().x) / float(Window::size().y), Znear(), Zfar()));
    else
        SetProjectionMatrix(glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w));
}

void Camera::UpdateTransformMatrix()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    //Node::UpdateTransformMatrix();
}

glm::mat4 Camera::ViewMatrix() const
{
    return TransformMatrix();
}

void Camera::SetViewMatrix(glm::mat4 view)
{
    SetTransformMatrix(view);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    return _projection;
}

void Camera::SetProjectionMatrix(glm::mat4 projection)
{
    _projection = projection;
}

glm::ivec4& Camera::frustum()
{
    return _frustum;
}

float Camera::Fov() const
{
    return _fov;
}

void Camera::SetFov(float fov)
{
    _fov = fov;
}

float Camera::Znear() const
{
    return _znear;
}

void Camera::SetZnear(float znear)
{
    _znear = znear;
}

float Camera::Zfar() const
{
    return _zfar;
}

void Camera::SetZfar(float zfar)
{
    _zfar = zfar;
}

