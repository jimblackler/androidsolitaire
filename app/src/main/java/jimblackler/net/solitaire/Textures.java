package jimblackler.net.solitaire;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;

class Textures {

  static Bitmap textAsBitmap(String text, float textSize, int textColor) {
    Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    paint.setTextSize(textSize);
    paint.setColor(textColor);
    paint.setTextAlign(Paint.Align.LEFT);
    float baseline = -paint.ascent(); // ascent() is negative
    int width = Math.round(paint.measureText(text));
    int height = Math.round(baseline + paint.descent());

    Bitmap image = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(image);
    canvas.drawText(text, 0, baseline, paint);
    return image;
  }

}
