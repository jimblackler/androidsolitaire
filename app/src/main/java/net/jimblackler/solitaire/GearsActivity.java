package net.jimblackler.solitaire;

import android.app.Activity;
import android.os.Bundle;
import jimblackler.net.solitaire.R;

public class GearsActivity extends Activity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_gears);
    Common.setDecorView(getWindow().getDecorView());
  }
}
