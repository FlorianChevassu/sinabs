#include "graph/Module/TargetType.hxx"

#include "Graph/Graph.hxx"

#include "Graph/Project/Property.hxx"
#include "Graph/Project/Target.hxx"

#include "Graph/ScriptEngine.hxx"

#include "Utils/TypeTraits.hxx"

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
          PropertyType p;
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
    if (i_json.find("rule_generator") != i_json.end())
    {
      auto jsonRuleGenerator = i_json.at("rule_generator");
      if (jsonRuleGenerator.is_string())
      {
        jsonRuleGenerator.get_to(this->m_ruleGenerator);
      }
      if (jsonRuleGenerator.find("path") != jsonRuleGenerator.end())
      {
        // TODO: We should do this in the "RuleGeneratorBuilder" ?
        auto scriptFile = std::filesystem::current_path() / jsonRuleGenerator.at("path").get<std::string>();
        if (!std::filesystem::exists(scriptFile))
        {
          // TODO throw
          assert(false);
        }

        std::ifstream ifs(scriptFile);
        this->m_ruleGenerator = std::string((std::istreambuf_iterator<char>(ifs)),
          std::istreambuf_iterator<char>());
      }
    }
  }

  std::string TargetType::GenerateRule(graph::Graph& i_graph, project::Target& i_target) const
  {
    if (this->m_ruleGenerator.empty())
    {
      return "";
    }

    ScriptEngine engine;
    engine.SetGraph(i_graph);
    engine.AddTarget(i_target);

    std::string rule;
    //try
    {
      rule = engine.Evaluate(this->m_ruleGenerator);
    }
    /*catch (std::exception& e)
    {
      int k = 2;
      std::cerr << e.what() << std::endl;
    }*/
    // Initialize the Chaiscript environment
    // Run the rule generator script
    // Extract the result

    return rule;
  }

  const PropertyType* FindPropertyTypeByName(const TargetType& i_targetType, const std::string& i_name)
  {
    auto it = std::find_if(i_targetType.m_properties.begin(), i_targetType.m_properties.end(),
      [&i_name](const auto& prop)
      {
        return prop.m_name == i_name;
      }
    );
    if (it != i_targetType.m_properties.end())
    {
      return &*it;
    }
    return nullptr;
  }

  const InlineTarget* FindInlineTargetByName(const TargetType& i_targetType, const std::string& i_name)
  {
    auto inlineTargetIt = std::find_if(i_targetType.m_inlineTargets.begin(), i_targetType.m_inlineTargets.end(),
      [&i_name](const auto& i_inlineTarget)
      {
        return i_inlineTarget.m_name == i_name;
      }
    );
    if (inlineTargetIt != i_targetType.m_inlineTargets.end())
    {
      return &*inlineTargetIt;
    }
    return nullptr;
  }

}
