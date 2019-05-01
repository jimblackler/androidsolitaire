package jimblackler.net.solitaire;

import android.content.Context;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;

import javax.microedition.khronos.opengles.GL10;
import java.io.IOException;

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
