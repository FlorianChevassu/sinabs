#pragma once

#include <vector>

namespace ngbs::utils
{
  template<typename T>
  struct is_vector {
    static bool const value = false;
  };

  template<typename T, typename A>
  struct is_vector<std::vector<T, A>> {
    static bool const value = true;
  };

  template <typename T>
  constexpr bool is_vector_v = is_vector<T>::value;
}
