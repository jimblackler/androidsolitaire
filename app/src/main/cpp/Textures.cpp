#include "Textures.h"

#include <cstdlib>

int Random(int uboundExclusive) {
  int r = rand();
  return r % uboundExclusive;
}

unsigned char *GenerateTexture(int width, int height) {
  // Needs freeing.
 auto pixelData = (unsigned char *) malloc((size_t) width * height * 3);
  unsigned char *p;
  int x, y;
  for (y = 0, p = pixelData; y < height; y++) {
    for (x = 0; x < width; x++, p += 3) {
      p[0] = p[1] = p[2] = (unsigned char) (128 + ((x > 2 && y > 2) ? Random(128) : 0));
    }
  }
  return pixelData;
}
