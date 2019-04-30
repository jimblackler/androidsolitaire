package jimblackler.net.solitaire;

class MainLib {

  static {
    System.loadLibrary("native-lib");
  }

  public static native void init(GlService glService, int[] pixels, int width, int height);

  public static native void resize(int width, int height);

  public static native void step();
}
