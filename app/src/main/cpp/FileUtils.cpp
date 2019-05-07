#include <cstdlib>
#include "FileUtils.h"

char *load(const char *filename, AAssetManager *assetManager) {
  AAsset *asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
  if (!asset) {
    return nullptr;
  }
  auto size = (size_t) AAsset_getLength(asset);
  auto buffer = (char *) malloc(sizeof(char) * size);
  AAsset_read(asset, buffer, size);
  AAsset_close(asset);
  return buffer;
}



