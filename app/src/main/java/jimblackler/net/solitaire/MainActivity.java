package jimblackler.net.solitaire;

import android.app.Activity;
import android.content.ComponentCallbacks2;
import android.os.Bundle;

public class MainActivity extends Activity implements ComponentCallbacks2 {
  private SurfaceView surfaceView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    surfaceView = new SurfaceView(getApplication());
    setContentView(surfaceView);
  }

  @Override
  protected void onPause() {
    super.onPause();
    surfaceView.onPause();
  }

  @Override
  protected void onResume() {
    super.onResume();
    surfaceView.onResume();
  }

  /**
   * Release memory when the UI becomes hidden or when system resources become low.
   *
   * @param level the memory-related event that was raised.
   */
  public void onTrimMemory(int level) {

    // Determine which lifecycle or system event was raised.
    switch (level) {

      case ComponentCallbacks2.TRIM_MEMORY_UI_HIDDEN:

                /*
                   Release any UI objects that currently hold memory.

                   The user interface has moved to the background.
                */

        break;

      case ComponentCallbacks2.TRIM_MEMORY_RUNNING_MODERATE:
      case ComponentCallbacks2.TRIM_MEMORY_RUNNING_LOW:
      case ComponentCallbacks2.TRIM_MEMORY_RUNNING_CRITICAL:

                /*
                   Release any memory that your app doesn't need to run.

                   The device is running low on memory while the app is running.
                   The event raised indicates the severity of the memory-related event.
                   If the event is TRIM_MEMORY_RUNNING_CRITICAL, then the system will
                   begin killing background processes.
                */

        break;

      case ComponentCallbacks2.TRIM_MEMORY_BACKGROUND:
      case ComponentCallbacks2.TRIM_MEMORY_MODERATE:
      case ComponentCallbacks2.TRIM_MEMORY_COMPLETE:

                /*
                   Release as much memory as the process can.

                   The app is on the LRU list and the system is running low on memory.
                   The event raised indicates where the app sits within the LRU list.
                   If the event is TRIM_MEMORY_COMPLETE, the process will be one of
                   the first to be terminated.
                */

        break;

      default:
                /*
                  Release any non-critical data structures.

                  The app received an unrecognized memory level value
                  from the system. Treat this as a generic low-memory message.
                */
        break;
    }
  }
}
