package net.jimblackler.solitaire;

import android.app.NativeActivity;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.view.View;

import static java.lang.System.loadLibrary;

public class MainActivity extends NativeActivity {

  static {
    loadLibrary("native-activity");
  }

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
    startActivityForResult(intent, 0);
  }

  protected void vibrate() {
    Vibrator vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      vibrator.vibrate(VibrationEffect.createOneShot(
          5, VibrationEffect.DEFAULT_AMPLITUDE));
    } else {
      vibrator.vibrate(15);
    }
  }
  @Override
  protected void onActivityResult(int requestCode, int resultCode,
                                  Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
    if (resultCode == RESULT_OK) {
      String action = data.getStringExtra("action");
      if (action.equals("quit")) {
        finish();
      } else {
        action(action);
      }
    }
  }

  protected void onResume() {
    super.onResume();
    Common.setDecorView(getWindow().getDecorView());
  }

  native void action(String action);
}
