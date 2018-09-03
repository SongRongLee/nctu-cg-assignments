#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H
#include <stdio.h>
#include <stdlib.h>
#include "glew.h"
#include "glut.h"
bool ShaderLoad(GLhandleARB programId, char* shaderSrc, GLenum shaderType);

#endif
