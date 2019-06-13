#include <stdint.h>

class AAssetManager;

extern uint8_t *load(const char *filename, bool addNull,
                     AAssetManager *assetManager, size_t *sizeOut = nullptr);