package jimblackler.net.solitaire;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import java.util.ArrayList;
import java.util.List;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class SurfaceView extends GLSurfaceView {
  private static final String TAG = SurfaceView.class.getName();

  public SurfaceView(Context context) {
    super(context);
    // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
    // supporting OpenGL ES 2.0 or later backwards-compatible versions.
    setEGLConfigChooser(8, 8, 8, 0, 16, 0);
    setEGLContextClientVersion(3);
    setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);

    setRenderer(new Renderer(context));
  }

  private static class Renderer implements GLSurfaceView.Renderer {

    List<byte[]> bytes = new ArrayList<byte[]>();
    private final Context context;

    public Renderer(Context context) {
      this.context = context;
    }

    public void onDrawFrame(GL10 gl) {
      if (false) {
        try {
          int size = 1000000;
          byte[] data = new byte[size];
          bytes.add(data);
        } catch (OutOfMemoryError ignored) {

        }
      }
      MainLib.step();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
      MainLib.resize(width, height);
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
      MainLib.init(new GlService(context, gl));
    }
  }
}
