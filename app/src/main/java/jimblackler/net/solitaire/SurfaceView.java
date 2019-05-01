package jimblackler.net.solitaire;

import android.content.Context;
import android.opengl.GLSurfaceView;

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

    private final Context context;

    Renderer(Context context) {
      this.context = context;
    }

    public void onDrawFrame(GL10 gl) {
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
