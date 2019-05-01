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

  void loadTexture() throws IOException {
    GLUtils.texImage2D(gl.GL_TEXTURE_2D, 0,
        BitmapFactory.decodeResource(context.getResources(), R.drawable.apple_webp), 0);
  }
}
