#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include "GLESRenderer.hpp"

char *GLESRenderer::LoadShaderFile(const char *shaderFileName)
{
    FILE *fp = fopen(shaderFileName, "rb");
    if (fp == NULL)
        return NULL;

    fseek(fp , 0 , SEEK_END);
    long totalBytes = ftell(fp);
    fclose(fp);

    char *buf = (char *)malloc(totalBytes+1);
    memset(buf, 0, totalBytes+1);

    fp = fopen(shaderFileName, "rb");
    if (fp == NULL)
        return NULL;

    size_t bytesRead = fread(buf, totalBytes, 1, fp);
    fclose(fp);
    if (bytesRead < 1)
        return NULL;

    return buf;
}

GLuint GLESRenderer::LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0)
        return 0;
    
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char *infoLog = (char *)malloc(sizeof ( char ) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            std::cerr << "*** SHADER COMPILE ERROR:" << std::endl;
            std::cerr << infoLog << std::endl;
            free(infoLog);
        }
        glDeleteShader ( shader );
        return 0;
    }
    
    return shader;
}

GLuint GLESRenderer::LoadProgram(const char *vertShaderSrc, const char *fragShaderSrc)
{
    GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vertShaderSrc);
    if (vertexShader == 0)
        return 0;
    
    GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    GLuint programObject = glCreateProgram();
    if (programObject == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);
    
    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char *infoLog = (char *)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            std::cerr << "*** SHADER LINK ERROR:" << std::endl;
            std::cerr << infoLog << std::endl;
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}
