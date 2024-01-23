// This is a benchmark to compare different ways of accessing uniform values in
// OpenGL.
//
// Details here: https://sinf.org/gluniformlocation-benchmark/
// 
// Uses GLFW and Galogen generated opengl header for basic opengl support.
//
// MIT License
// Copyright (c) 2024 Lasse Hassing
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <fstream>

#include "vendor/gl_core_4_3.h"
#include "vendor/glfw/include/glfw/glfw3.h"

#include "shader_cache.h"

#define OUTPUT_WIDTH 1920
#define OUTPUT_HEIGHT 1080
#define LOOPS 1000

#define _LOOKUP_UNIFORM
#undef _STATIC_UNIFORM
#undef _CACHED_UNIFORM

/* Vertex shader ==================================
#version 430
layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec3 fragPos;

uniform vec4 uniform0;
uniform vec4 uniform1;
uniform vec4 uniform2;
uniform vec4 uniform3;
uniform vec4 uniform4;
uniform vec4 uniform5;
uniform vec4 uniform6;
uniform vec4 uniform7;
uniform vec4 uniform8;
uniform vec4 uniform9;

void main() {
    vec4 calc = uniform0 + uniform1 + uniform2 + uniform3 + uniform4 + 
                uniform5 + uniform6 + uniform7 + uniform8 + uniform9;
    fragPos = vec3(inPosition.x, inPosition.y, calc.z);
}
=================================================*/
const char *vertSource = "#version 430\nlayout(location = 0) in vec2 inPosition; layout(loca\
tion = 0) out vec3 fragPos; uniform vec4 uniform0; uniform vec4 uniform1; uniform vec4 unifo\
rm2; uniform vec4 uniform3; uniform vec4 uniform4; uniform vec4 uniform5; uniform vec4 unifo\
rm6; uniform vec4 uniform7; uniform vec4 uniform8; uniform vec4 uniform9; void main() { vec4\
 calc = uniform0 + uniform1 + uniform2 + uniform3 + uniform4 + uniform5 + uniform6 + uniform\
7 + uniform8 + uniform9; fragPos = vec3(inPosition.x, inPosition.y, calc.z); }";

/* Fragment shader ================================
#version 430
layout(location = 0) in vec3 fragPos;
layout(location = 0) out vec4 outColor;

uniform vec4 uniform10;
uniform vec4 uniform11;
uniform vec4 uniform12;
uniform vec4 uniform13;
uniform vec4 uniform14;
uniform vec4 uniform15;
uniform vec4 uniform16;
uniform vec4 uniform17;
uniform vec4 uniform18;
uniform vec4 uniform19;

void main() {
    vec4 calc = uniform10 + uniform11 + uniform12 + uniform13 + uniform14 + 
                uniform15 + uniform16 + uniform17 + uniform18 + uniform19;
    outColor = vec4(fragPos.x, fragPos.y, calc.z, calc.w);
}
=================================================*/
const char* fragSource = "#version 430\nlayout(location = 0) in vec3 fragPos; layout(locatio\
n = 0) out vec4 outColor; uniform vec4 uniform10; uniform vec4 uniform11; uniform vec4 unifo\
rm12; uniform vec4 uniform13; uniform vec4 uniform14; uniform vec4 uniform15; uniform vec4 u\
niform16; uniform vec4 uniform17; uniform vec4 uniform18; uniform vec4 uniform19; void main(\
) { vec4 calc = uniform10 + uniform11 + uniform12 + uniform13 + uniform14 + uniform15 + unif\
orm16 + uniform17 + uniform18 + uniform19; outColor = vec4(fragPos.x, fragPos.y, calc.z, cal\
c.w); }";

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    // Setup basic OpenGL instance and window with GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(OUTPUT_WIDTH, OUTPUT_HEIGHT, "gl_uniform_benchmark", NULL, NULL);  
    glfwMakeContextCurrent(window);  
    glViewport(0, 0, OUTPUT_WIDTH, OUTPUT_HEIGHT);

    // Setup OpenGL buffer and vertex array with a 2D square that can be rendered
    // with GL_TRIANGLE_STRIP.
    GLuint vbo, vao, fbo, fbTexture;
    float vertexdata[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vertexdata, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Build shader.
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    GLint attrib = glGetAttribLocation(program, "inPosition");
    glEnableVertexAttribArray(attrib);
    size_t attriboffset = 0;
    glVertexAttribPointer(attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(attriboffset));
    glUseProgram(program);

    #ifdef _STATIC_UNIFORM
        GLint uniform0 = glGetUniformLocation(program, "uniform0");        
        GLint uniform1 = glGetUniformLocation(program, "uniform1");
        GLint uniform2 = glGetUniformLocation(program, "uniform2");
        GLint uniform3 = glGetUniformLocation(program, "uniform3");
        GLint uniform4 = glGetUniformLocation(program, "uniform4");
        GLint uniform5 = glGetUniformLocation(program, "uniform5");
        GLint uniform6 = glGetUniformLocation(program, "uniform6");
        GLint uniform7 = glGetUniformLocation(program, "uniform7");
        GLint uniform8 = glGetUniformLocation(program, "uniform8");
        GLint uniform9 = glGetUniformLocation(program, "uniform9");
        GLint uniform10 = glGetUniformLocation(program, "uniform10");
        GLint uniform11 = glGetUniformLocation(program, "uniform11");
        GLint uniform12 = glGetUniformLocation(program, "uniform12");
        GLint uniform13 = glGetUniformLocation(program, "uniform13");
        GLint uniform14 = glGetUniformLocation(program, "uniform14");
        GLint uniform15 = glGetUniformLocation(program, "uniform15");
        GLint uniform16 = glGetUniformLocation(program, "uniform16");
        GLint uniform17 = glGetUniformLocation(program, "uniform17");
        GLint uniform18 = glGetUniformLocation(program, "uniform18");
        GLint uniform19 = glGetUniformLocation(program, "uniform19");

        if(uniform0 == -1 || uniform19 == -1) {
            MessageBoxA(NULL, "Uniform missing!", "GL Error", MB_OK | MB_ICONERROR);
            exit(0);
        }
    #endif

    #ifdef _CACHED_UNIFORM
        ShaderCache* cache = new ShaderCache(program);
    #endif

    glClearColor(0.10f, 0.15f, 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LARGE_INTEGER first; first.QuadPart = 0;
    LARGE_INTEGER second; second.QuadPart = 0;
    LARGE_INTEGER fastest; fastest.QuadPart = 10000000;
    LARGE_INTEGER slowest; slowest.QuadPart = 0;
    LARGE_INTEGER total; total.QuadPart = 0;
    
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq); 

    for(int i=0; i<LOOPS; i++) {
        glfwPollEvents();
        float testdata[4] = {rand()%1001/1000.0f, rand()%1001/1000.0f, rand()%1001/1000.0f};
        LARGE_INTEGER start;
        LARGE_INTEGER stop;

        QueryPerformanceCounter(&start);
        for(int u=0; u<50; u++) {
            #ifdef _LOOKUP_UNIFORM
            glUniform4fv(glGetUniformLocation(program, "uniform0"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform1"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform2"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform3"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform4"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform5"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform6"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform7"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform8"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform9"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform10"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform11"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform12"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform13"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform14"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform15"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform16"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform17"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform18"), 1, testdata);
            glUniform4fv(glGetUniformLocation(program, "uniform19"), 1, testdata);
            #endif

            #ifdef _STATIC_UNIFORM
            glUniform4fv(uniform0, 1, testdata);
            glUniform4fv(uniform1, 1, testdata);
            glUniform4fv(uniform2, 1, testdata);
            glUniform4fv(uniform3, 1, testdata);
            glUniform4fv(uniform4, 1, testdata);
            glUniform4fv(uniform5, 1, testdata);
            glUniform4fv(uniform6, 1, testdata);
            glUniform4fv(uniform7, 1, testdata);
            glUniform4fv(uniform8, 1, testdata);
            glUniform4fv(uniform9, 1, testdata);
            glUniform4fv(uniform10, 1, testdata);
            glUniform4fv(uniform11, 1, testdata);
            glUniform4fv(uniform12, 1, testdata);
            glUniform4fv(uniform13, 1, testdata);
            glUniform4fv(uniform14, 1, testdata);
            glUniform4fv(uniform15, 1, testdata);
            glUniform4fv(uniform16, 1, testdata);
            glUniform4fv(uniform17, 1, testdata);
            glUniform4fv(uniform18, 1, testdata);
            glUniform4fv(uniform19, 1, testdata);
            #endif

            #ifdef _CACHED_UNIFORM
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform0")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform1")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform2")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform3")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform4")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform5")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform6")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform7")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform8")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform9")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform10")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform11")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform12")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform13")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform14")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform15")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform16")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform17")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform18")), 1, testdata);
            glUniform4fv(cache->UniformLocation(UNIFORMNAME("uniform19")), 1, testdata);
            #endif
        }
        QueryPerformanceCounter(&stop);

        start.QuadPart *= 1000000;
        start.QuadPart /= freq.QuadPart;
        stop.QuadPart *= 1000000;
        stop.QuadPart /= freq.QuadPart;

        LARGE_INTEGER tdiff;
        tdiff.QuadPart = (stop.QuadPart - start.QuadPart);

        if(first.QuadPart == 0) {
            first = tdiff;
        } else if(second.QuadPart == 0) {
            second = tdiff;
        }
        if(tdiff.QuadPart < fastest.QuadPart) {
            fastest = tdiff;
        }
        if(tdiff.QuadPart > slowest.QuadPart) {
            slowest = tdiff;
        }
        total.QuadPart += tdiff.QuadPart;

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);

        if(glGetError() != 0) {
            MessageBoxA(NULL, "GL Error!", "GL Error", MB_OK | MB_ICONERROR);
            exit(1);
        }
    }

    char* results = (char*)calloc(1000, sizeof(char));
    sprintf(results, "First: %llu\nSecond: %llu\nFastest: %llu\nSlowest: %llu\n\nAverage: %llu",
            first.QuadPart, second.QuadPart, fastest.QuadPart, slowest.QuadPart, (total.QuadPart/(long long)LOOPS) );

    MessageBoxA(NULL, results, "Results", MB_OK);

    return 0;
}
