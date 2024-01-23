#include "shader_cache.h"
#include <windows.h>

ShaderCache::ShaderCache(GLuint program) {
    _program = program;
}

GLint ShaderCache::UniformLocation(const char* name, uint32_t id) {
    if (_uniforms.contains(id)) {
        return _uniforms[id];
    }

    GLint loc = glGetUniformLocation(_program, name);
    _uniforms[id] = loc;

    if(loc == -1) {
        MessageBoxA(NULL, "Uniform missing!", "GL Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    return loc;
}
