#pragma once

#include <GL/glew.h>
#include <memory>
#include <string>
#include <unordered_map>

class ShaderStage {
public:
    static std::shared_ptr<ShaderStage> Create(GLenum stage = 0, const std::string& code = "");
    void Compile();
    void Recompile();
    std::string FullCode();
    std::string Code() const;
    GLuint Glid() const;
    GLenum Stage() const;
    bool Compiled() const;
    void SetDefine(const std::string, const std::string = "");
    void RemoveDefine(const std::string);
    std::string Technique() const;
    void SetTechnique(const std::string);
    void Delete();
    ~ShaderStage();

private:
    ShaderStage(GLenum stage = 0, const std::string& code = "");

    GLuint _glid { 0 };
    GLenum _stage { 0 };
    std::string _fullCode { "" };
    std::string _code { "" };
    std::string _technique { "" };
    bool _compiled { false };
    std::unordered_map<std::string, std::string> _defines;
};
