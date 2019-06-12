package net.jimblackler.solitaire;

import android.os.Bundle;

public class RulesActivity extends HtmlActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    load("rules.html");
  }
}
