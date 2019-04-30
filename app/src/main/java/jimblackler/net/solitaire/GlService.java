package jimblackler.net.solitaire;

import static android.opengl.GLES31Ext.GL_COMPRESSED_RGBA_ASTC_4x4_KHR;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.opengl.GLUtils;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import javax.microedition.khronos.opengles.GL10;

class GlService {
  private final Context context;
  private final GL10 gl;

  GlService(Context context, GL10 gl) {
    this.context = context;
    this.gl = gl;
  }

  private static byte[] toByteArray(InputStream input) throws IOException {
    ByteArrayOutputStream output = new ByteArrayOutputStream();
    byte[] buffer = new byte[1024];
    int bytesRead;
    while ((bytesRead = input.read(buffer)) > 0) {
      output.write(buffer, 0, bytesRead);
    }
    return output.toByteArray();
  }

  private ByteBuffer byteBufferFromResource(int resource) throws IOException  {
    return ByteBuffer.wrap(toByteArray(context.getResources().openRawResource(resource)));
  }

  void loadTexture(String text) throws IOException {
    if (false) {
      int width = 1024;
      int height = 1024;
      gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGBA, width,
          height, 0, gl.GL_RGBA, gl.GL_UNSIGNED_BYTE,
          byteBufferFromResource(R.raw.apple_raw));
    } else if (false) {
      ByteBuffer byteBuffer = byteBufferFromResource(R.raw.apple_compressed);
      int width = 1024;
      int height = 1024;
      gl.glCompressedTexImage2D(gl.GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_4x4_KHR, width,
          height, 0, byteBuffer.capacity() - 16, byteBuffer);
    } else if (true) {
      Bitmap texture;
      if (false) {
        texture = Textures.textAsBitmap(text, 240, Color.MAGENTA);
      } else if (true) {
        texture = BitmapFactory.decodeResource(context.getResources(), R.drawable.apple_webp);
      }

      GLUtils.texImage2D(gl.GL_TEXTURE_2D, 0, texture, 0);
    }
  }
}
