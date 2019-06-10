package net.jimblackler.solitaire;

import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.view.Window;
import androidx.annotation.Nullable;

public class SettingsActivity extends PreferenceActivity {

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    super.onCreate(savedInstanceState);
    getFragmentManager().beginTransaction().replace(
        android.R.id.content, new SettingsFragment()).commit();
  }

  public static class SettingsFragment extends PreferenceFragment {
    @Override
    public void onCreate(final Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      addPreferencesFromResource(R.xml.preferences);
    }
  }
}
