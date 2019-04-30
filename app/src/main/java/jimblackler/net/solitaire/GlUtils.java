package jimblackler.net.solitaire;

import android.graphics.Bitmap;

class GlUtils {
  static int[] getTextureData(Bitmap texture) {
    int[] pixels = new int[texture.getWidth() * texture.getHeight()];
    texture.getPixels(pixels, 0, texture.getWidth(), 0, 0,
        texture.getWidth(), texture.getHeight());
    for (int idx = 0; idx < pixels.length; idx++) {
      int value = pixels[idx];
      int a = value >> 24;
      int r = (value >> 16) & 0xff;
      int g = (value >> 8) & 0xff;
      int b = value & 0xff;
      pixels[idx] = (a << 24) + (b << 16) + (g << 8) + r;  // ARGB -> ABGR
    }
    return pixels;
  }
}
