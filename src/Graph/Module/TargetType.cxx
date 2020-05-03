#include "graph/Module/TargetType.hxx"

#include "Graph/Project/Target.hxx"

#include <chaiscript/chaiscript.hpp>

#include <filesystem>

namespace ngbs::graph::module
{

  PropertyType* TargetType::GetPropertyTypeByName(const std::string& i_name)
  {
    //return utils::GetObjectByName<PropertyType, &PropertyType::m_name>(this->m_properties, i_name);
    return nullptr;
  }

  void TargetType::Load(const nlohmann::json& i_json)
  {
    if (i_json.find("name") != i_json.end())
    {
      i_json.at("name").get_to(this->m_name);
    }
    if (i_json.find("dependent_target_types") != i_json.end())
    {
      auto& dependent_target_types = i_json.at("dependent_target_types");
      for (auto dependent_target_type : dependent_target_types)
      {
        this->m_dependentTargetTypes.insert(static_cast<std::string>(dependent_target_type));
      }
    }
    if (i_json.find("properties") != i_json.end())
    {
      for (auto& jsonProperty : i_json.at("properties"))
      {
        PropertyType* property = this->GetPropertyTypeByName(jsonProperty.at("name"));
        if (property)
        {
          property->Load(jsonProperty);
        }
        else
        {
          PropertyType p;// (this->GetScriptEngine());
          p.Load(jsonProperty);
          this->m_properties.push_back(std::move(p));
        }
      }
    }
    if (i_json.find("inline_targets") != i_json.end())
    {
      for (auto& jsonInlineTarget : i_json.at("inline_targets"))
      {
        InlineTarget it = jsonInlineTarget;
        this->m_inlineTargets.push_back(std::move(it));
      }
    }
    if (i_json.find("default_property") != i_json.end())
    {
      this->m_defaultProperty = i_json.at("default_property");
    }
  }

  std::string TargetType::GenerateRule(const Target& i_target) const
  {
    return "RULE";
  }

}
