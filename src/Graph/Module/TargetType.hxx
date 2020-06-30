#pragma once

#include "Graph/Module/PropertyType.hxx"

#include "InlineTarget.hxx"

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <vector>
#include <set>

namespace ngbs::graph
{
  class Graph;
  namespace project
  {
    class Target;
  }
  namespace module
  {
    class TargetType
    {
    public:
      PropertyType* GetPropertyTypeByName(const std::string& i_name);

      std::string m_name;
      std::set<std::string> m_dependentTargetTypes;
      std::vector<InlineTarget> m_inlineTargets;
      std::vector<PropertyType> m_properties;
      std::optional<std::string> m_defaultProperty;
      std::string m_ruleGenerator;

      std::string GenerateRule(graph::Graph& i_graph, project::Target& i_target) const;

      void Load(const nlohmann::json& i_json);
    };

    const PropertyType* FindPropertyTypeByName(const TargetType& i_targetType, const std::string& i_name);
    const InlineTarget* FindInlineTargetByName(const TargetType& i_targetType, const std::string& i_name);
  }
}
