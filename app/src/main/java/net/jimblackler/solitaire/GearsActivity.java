package net.jimblackler.solitaire;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Window;

public class GearsActivity extends Activity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    setContentView(R.layout.activity_gears);
    findViewById(R.id.new_game).setOnClickListener(v -> {
      setResult(RESULT_OK, new Intent().putExtra("action", "newGame"));
      finish();
    });
    Common.setDecorView(getWindow().getDecorView());
  }
}
