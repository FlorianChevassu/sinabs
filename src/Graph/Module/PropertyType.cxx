#include "PropertyType.hxx"

namespace ngbs::graph::module
{
  void PropertyType::Load(const nlohmann::json& i_json)
  {
    i_json.at("name").get_to(this->m_name);

    auto jsonValue = i_json.at("value");
    jsonValue.at("type").get_to(this->m_value.m_type);
    if (jsonValue.find("arity") != jsonValue.end())
    {
      jsonValue.at("arity").get_to(this->m_value.m_arity);
    }

    if (i_json.find("default_propagation") != i_json.end())
    {
      std::vector<std::string> propatations = i_json.at("default_propagation");
      this->m_default_propagation = Propagation::None;
      for (auto& propagation : propatations)
      {
        if (propagation == "children")
        {
          this->m_default_propagation |= Propagation::Children;
        }
        else if (propagation == "parent")
        {
          this->m_default_propagation |= Propagation::Parent;
        }
        else
        {
          // TODO: emit error ?
          assert(propagation == "none");
          this->m_default_propagation = Propagation::None;
        }
      }
    }
  }
}
