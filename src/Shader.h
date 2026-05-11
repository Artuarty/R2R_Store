#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    GLuint Program;

    Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexCode, fragmentCode;
        std::ifstream vFile, fFile;
        vFile.exceptions(std::ifstream::badbit);
        fFile.exceptions(std::ifstream::badbit);
        try {
            vFile.open(vertexPath);
            fFile.open(fragmentPath);
            std::stringstream vss, fss;
            vss << vFile.rdbuf();
            fss << fFile.rdbuf();
            vertexCode   = vss.str();
            fragmentCode = fss.str();
        } catch (std::ifstream::failure&) {
            std::cerr << "ERROR::SHADER::FILE_NOT_READ: "
                      << vertexPath << " / " << fragmentPath << "\n";
        }

        const char* vSrc = vertexCode.c_str();
        const char* fSrc = fragmentCode.c_str();
        GLuint vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vSrc, nullptr);
        glCompileShader(vert);
        checkCompile(vert, "VERTEX");

        GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &fSrc, nullptr);
        glCompileShader(frag);
        checkCompile(frag, "FRAGMENT");

        Program = glCreateProgram();
        glAttachShader(Program, vert);
        glAttachShader(Program, frag);
        glLinkProgram(Program);
        checkLink(Program);

        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    void Use() { glUseProgram(Program); }

    void setInt  (const char* n, int v)         { glUniform1i (glGetUniformLocation(Program, n), v); }
    void setFloat(const char* n, float v)        { glUniform1f (glGetUniformLocation(Program, n), v); }
    void setBool (const char* n, bool v)         { glUniform1i (glGetUniformLocation(Program, n), (int)v); }
    void setVec3 (const char* n, glm::vec3 v)   { glUniform3fv(glGetUniformLocation(Program, n), 1, glm::value_ptr(v)); }
    void setMat4 (const char* n, glm::mat4 m)   { glUniformMatrix4fv(glGetUniformLocation(Program, n), 1, GL_FALSE, glm::value_ptr(m)); }

private:
    void checkCompile(GLuint s, const char* type) {
        GLint ok; GLchar log[512];
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(s, 512, nullptr, log);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILE\n" << log << "\n";
        }
    }
    void checkLink(GLuint p) {
        GLint ok; GLchar log[512];
        glGetProgramiv(p, GL_LINK_STATUS, &ok);
        if (!ok) {
            glGetProgramInfoLog(p, 512, nullptr, log);
            std::cerr << "ERROR::SHADER::LINK\n" << log << "\n";
        }
    }
};
