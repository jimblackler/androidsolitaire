package net.jimblackler.solitaire;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import jimblackler.net.solitaire.R;

public class GearsActivity extends Activity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_gears);
    findViewById(R.id.new_game).setOnClickListener(v -> {
      Intent intent = new Intent();
      intent.putExtra("action", "newGame");
      setResult(RESULT_OK, intent);
      finish();
    });
    Common.setDecorView(getWindow().getDecorView());
  }
}
