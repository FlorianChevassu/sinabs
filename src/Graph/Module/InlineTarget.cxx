#include "InlineTarget.hxx"

namespace ngbs::graph::module
{
  void from_json(const nlohmann::json& i_json, InlineTarget& i_inlineTarget)
  {
    i_json.at("name").get_to(i_inlineTarget.m_name);
    i_json.at("type").get_to(i_inlineTarget.m_type);
    if (i_json.find("default") != i_json.end())
    {
      i_json.at("default").get_to(i_inlineTarget.m_default);
    }
    else
    {
      i_inlineTarget.m_default = false;
    }
    if (i_json.find("arity") != i_json.end())
    {
      i_json.at("arity").get_to(i_inlineTarget.m_arity);
    }
  }

}
