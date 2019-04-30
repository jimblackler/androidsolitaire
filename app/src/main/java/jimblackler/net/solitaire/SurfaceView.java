package jimblackler.net.solitaire;

import static jimblackler.net.solitaire.GlConstants.glConstant;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.util.Log;
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
      Bitmap texture;
      if (false) {
        texture = Textures.textAsBitmap("Hello", 240, Color.MAGENTA);
      } else {
        texture = BitmapFactory.decodeResource(context.getResources(), R.drawable.apple_webp);
      }
      int[] pixels = GlUtils.getTextureData(texture);
      MainLib.init(new GlService(context, gl), pixels, texture.getWidth(), texture.getHeight());

      int[] results = new int[20];
      gl.glGetIntegerv(gl.GL_NUM_COMPRESSED_TEXTURE_FORMATS, results, 0);
      int numFormats = results[0];
      results = new int[numFormats];

      gl.glGetIntegerv(gl.GL_COMPRESSED_TEXTURE_FORMATS, results, 0);
      for (int i = 0; i < numFormats; i++) {
        Log.i(TAG, "GL_COMPRESSED_TEXTURE_FORMATS: " + Integer.toHexString(results[i]) + " " +
            glConstant(results[i]));
      }

      // Detect capabilities.
      String s = gl.glGetString(gl.GL_EXTENSIONS);
      if (s.contains("GL_IMG_texture_compression_pvrtc")) {
        Log.i(TAG, "Use PVR compressed textures");
      } else if (s.contains("GL_AMD_compressed_ATC_texture") ||
          s.contains("GL_ATI_texture_compression_atitc")) {
        Log.i(TAG, "Load ATI Textures");
      } else if (s.contains("GL_OES_texture_compression_S3TC") ||
          s.contains("GL_EXT_texture_compression_s3tc")) {
        Log.i(TAG, "Use DTX Textures");
      } else {
        Log.i(TAG, "Handle no texture compression founded.");
      }
    }
  }
}
