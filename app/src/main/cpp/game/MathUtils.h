class MathUtils {
public:
  template<typename T>
  static float toT(T start, T end, T value) {
    return (float) (value - start) / (end - start);
  }

  template<typename T>
  static float tInRange(T start, T end, float t) {
    return t * (end - start) + start;
  }
};