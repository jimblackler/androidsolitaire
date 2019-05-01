package jimblackler.net.solitaire;

import android.app.Activity;
import android.content.ComponentCallbacks2;
import android.os.Bundle;

public class MainActivity extends Activity implements ComponentCallbacks2 {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    SurfaceView surfaceView = new SurfaceView(getApplication());
    setContentView(surfaceView);
  }
}
