/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:12:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:09:16 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Object.hpp"
#include <string>

class Texture;
class Framebuffer;
class Shader;

class Material : public Object {
public:
    static std::shared_ptr<Material> create(const std::string&);
    static std::shared_ptr<Material> get(unsigned index);
    static std::shared_ptr<Material> get_by_name(const std::string&);
    virtual void bind_values();
    virtual void bind_textures();
    std::shared_ptr<Shader> shader();
    std::shared_ptr<Shader> depth_shader();
    std::shared_ptr<Texture> texture_albedo();
    std::shared_ptr<Texture> texture_specular();
    std::shared_ptr<Texture> texture_emitting();
    std::shared_ptr<Texture> texture_normal();
    std::shared_ptr<Texture> texture_height();
    std::shared_ptr<Texture> texture_roughness();
    std::shared_ptr<Texture> texture_metallic();
    std::shared_ptr<Texture> texture_ao();
    void set_texture_albedo(std::shared_ptr<Texture>);
    void set_texture_specular(std::shared_ptr<Texture>);
    void set_texture_emitting(std::shared_ptr<Texture>);
    void set_texture_normal(std::shared_ptr<Texture>);
    void set_texture_height(std::shared_ptr<Texture>);
    void set_texture_roughness(std::shared_ptr<Texture>);
    void set_texture_metallic(std::shared_ptr<Texture>);
    void set_texture_ao(std::shared_ptr<Texture>);
    VEC3 albedo { 0, 0, 0 };
    VEC3 specular { 0.04, 0.04, 0.04 };
    VEC3 emitting { 0, 0, 0 };
    VEC2 uv_scale { 1, 1 };
    float roughness { 0.5 };
    float metallic { 0 };
    float alpha { 1 };
    float parallax { 0.01 };
    float ior { 1 };

protected:
    static std::vector<std::shared_ptr<Material>> _materials;
    std::weak_ptr<Shader> _shader;
    std::weak_ptr<Shader> _depth_shader;
    std::weak_ptr<Texture> _texture_albedo;
    std::weak_ptr<Texture> _texture_specular;
    std::weak_ptr<Texture> _texture_emitting;
    std::weak_ptr<Texture> _texture_normal;
    std::weak_ptr<Texture> _texture_height;
    std::weak_ptr<Texture> _texture_roughness;
    std::weak_ptr<Texture> _texture_metallic;
    std::weak_ptr<Texture> _texture_ao;
    Material(const std::string& name);
};