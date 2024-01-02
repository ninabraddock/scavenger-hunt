#ifndef FINAL_PROJECT_NBRADDOC_RROSE1_DEBUG_H
#define FINAL_PROJECT_NBRADDOC_RROSE1_DEBUG_H

#include <glad/glad.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)
#define glFunction(func, ...) func(__VA_ARGS__); CHECK_GL_ERROR()

#endif //FINAL_PROJECT_NBRADDOC_RROSE1_DEBUG_H
