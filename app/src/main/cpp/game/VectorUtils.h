namespace vector_utils {

  template<typename T>
  int indexOf(const std::vector<T> &vector, T value) {
    auto entry = std::find(vector.begin(), vector.end(), value);
    if (entry == vector.end()) {
      return -1;
    }
    return entry - vector.begin();
  }

  template<typename T>
  bool remove(std::vector<T> &vector, T value) {
    auto entry = std::find(vector.begin(), vector.end(), value);
    if (entry == vector.end()) {
      return false;
    }
    vector.erase(entry);
    return true;
  }
}

