#pragma once

#include "Graph/Module/Arity.hxx"

#include <nlohmann/json.hpp>

#include <string>

namespace ngbs::graph::module
{
  struct InlineTarget
  {
    std::string m_name;
    std::string m_type;
    bool m_default;
    Arity m_arity;
  };

  void from_json(const nlohmann::json& i_json, InlineTarget& i_inlinetarget);

}
