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
    findViewById(R.id.undo).setOnClickListener(v -> {
      setResult(RESULT_OK, new Intent().putExtra("action", "undo"));
      finish();
    });
    findViewById(R.id.new_game).setOnClickListener(v -> {
      setResult(RESULT_OK, new Intent().putExtra("action", "newGame"));
      finish();
    });
    findViewById(R.id.retry_game).setOnClickListener(v -> {
      setResult(RESULT_OK, new Intent().putExtra("action", "restartGame"));
      finish();
    });
    findViewById(R.id.settings).setOnClickListener(v -> {
      startActivityForResult(new Intent(this, SettingsActivity.class), 0);
      finish();
    });
    findViewById(R.id.rules).setOnClickListener(v -> {
      startActivityForResult(new Intent(this, RulesActivity.class), 0);
      finish();
    });
    findViewById(R.id.about).setOnClickListener(v -> {
      startActivityForResult(new Intent(this, AboutActivity.class), 0);
      finish();
    });
    findViewById(R.id.about2).setOnClickListener(v -> {
      startActivityForResult(new Intent(this, AboutActivity.class), 0);
      finish();
    });
    Common.setDecorView(getWindow().getDecorView());
  }
}
