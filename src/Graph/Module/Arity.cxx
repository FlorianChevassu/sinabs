#include "Arity.hxx"

#include <string>

namespace ngbs::graph::module
{

void from_json(const nlohmann::json& i_json, Arity& i_arity)
{
  std::string value;
  i_json.get_to(value);
  if (value == "n")
  {
    i_arity.m_value = -1;
  }
  else
  {
    std::string value;
    i_json.get_to(value);
    i_arity.m_value = std::stoi(value);
  }
}

}
