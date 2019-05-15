package net.jimblackler.solitaire;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends NativeActivity {
  void setDecorView() {
    View decorView = getWindow().getDecorView();
      decorView.setSystemUiVisibility(
          View.SYSTEM_UI_FLAG_FULLSCREEN
              | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
              | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
              | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
              | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
              | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setDecorView();
    View decorView = getWindow().getDecorView();
    decorView.setOnSystemUiVisibilityChangeListener
        ((int visibility) ->
            setDecorView()
        );
  }

  protected void onResume() {
    super.onResume();
    setDecorView();
  }
}
