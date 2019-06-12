package net.jimblackler.solitaire;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;

abstract class HtmlActivity extends Activity {
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
  }

  protected void load(String page) {
    setContentView(R.layout.activity_html);
    WebView webView = findViewById(R.id.webView);
    webView.loadUrl("file:///android_asset/web/" + page);
  }
}
