#include <cstdlib>
#include "FileUtils.h"
#include <android/asset_manager.h>

uint8_t *load(const char *filename, bool addNull, AAssetManager *assetManager,
              size_t *sizeOut) {
  AAsset *asset =
      AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
  if (!asset) {
    return nullptr;
  }
  auto size = (size_t) AAsset_getLength(asset);
  auto buffer = (uint8_t *) malloc(sizeof(uint8_t) * size + (addNull ? 1 : 0));
  AAsset_read(asset, buffer, size);
  AAsset_close(asset);
  if (addNull) {
    buffer[size] = 0;
  }
  if (sizeOut) {
    *sizeOut = size;
  }
  return buffer;
}
