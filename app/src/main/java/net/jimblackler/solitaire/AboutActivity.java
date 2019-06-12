package net.jimblackler.solitaire;

import android.os.Bundle;

public class AboutActivity extends HtmlActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    load("about.html");
  }
}
