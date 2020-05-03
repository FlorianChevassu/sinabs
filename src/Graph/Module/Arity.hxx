#pragma once

#include <nlohmann/json.hpp>

namespace ngbs::graph::module
{
  struct Arity
  {
    Arity(size_t i_value = 1) : m_value(i_value) { }

    struct N {};
    Arity(N /*unused*/) : m_value(-1) { }

    int m_value;
  };

  void from_json(const nlohmann::json& i_json, Arity& i_arity);

}
