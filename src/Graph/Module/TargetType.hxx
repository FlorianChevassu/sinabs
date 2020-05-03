#pragma once

#include "Graph/Node.hxx"

#include "Graph/Module/PropertyType.hxx"

#include "InlineTarget.hxx"

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <vector>
#include <set>

class Target;
namespace ngbs::graph::module
{

class TargetType : public NodeBase
{
public:
  static const NodeType Type = NodeType::TargetType;
  NodeType GetNodeType() override { return TargetType::Type; }

  PropertyType* GetPropertyTypeByName(const std::string& i_name);

  std::string m_name;
  std::set<std::string> m_dependentTargetTypes;
  std::vector<InlineTarget> m_inlineTargets;
  std::vector<PropertyType> m_properties;
  std::optional<std::string> m_defaultProperty;

  std::string GenerateRule(const Target& i_target) const;

  void Load(const nlohmann::json& i_json);
};

}
