class MathUtils {
public:
  static float toT(float start, float end, float value) {
    return (value - start) / (end - start);
  }

  static float tInRange(float start, float end, float t) {
    return t * (end - start) + start;
  }
};