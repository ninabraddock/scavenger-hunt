#ifndef SCAVENGER-HUNT
#define SCAVENGER-HUNT

#include <glad/glad.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)
#define glFunction(func, ...) func(__VA_ARGS__); CHECK_GL_ERROR()

#endif //SCAVENGER-HUNT
