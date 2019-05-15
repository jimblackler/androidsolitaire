package net.jimblackler.solitaire;

import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends NativeActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    View decorView = getWindow().getDecorView();
    Common.setDecorView(decorView);

    decorView.setOnSystemUiVisibilityChangeListener(
        (int visibility) -> Common.setDecorView(decorView));
  }

  protected void gearsPressed() {
    Intent intent = new Intent(this, GearsActivity.class);
    startActivity(intent);
  }

  protected void onResume() {
    super.onResume();
    Common.setDecorView(getWindow().getDecorView());
  }
}
