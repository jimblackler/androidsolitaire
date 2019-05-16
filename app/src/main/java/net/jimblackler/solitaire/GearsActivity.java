package net.jimblackler.solitaire;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class GearsActivity extends Activity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_gears);
    findViewById(R.id.new_game).setOnClickListener(v -> {
      setResult(RESULT_OK, new Intent().putExtra("action", "newGame"));
      finish();
    });
    findViewById(R.id.quit).setOnClickListener(v -> {
      setResult(RESULT_OK, new Intent().putExtra("action", "quit"));
      finish();
    });

    Common.setDecorView(getWindow().getDecorView());
  }
}
