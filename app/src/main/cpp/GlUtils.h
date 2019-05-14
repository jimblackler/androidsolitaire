#include <GLES3/gl32.h>
#include <android/log.h>
#include <android/asset_manager.h>

extern bool checkGlError(const char *funcName);
GLuint loadProgram(const char *vertexShaderFilename,
                   const char *fragmentShaderFilename, AAssetManager *assetManager);