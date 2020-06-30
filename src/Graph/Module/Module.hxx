#pragma once

#include "Graph/Node.hxx"

#include "TargetType.hxx"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace ngbs::graph::module
{

class Module : public NodeBase
{
public:
  Module() : NodeBase(nullptr) {}

  static const NodeType Type = NodeType::Module;
  NodeType GetNodeType() override { return Module::Type; }

  std::string m_name;
  std::vector<TargetType> m_targetTypes;

  const TargetType* GetTargetTypeByName(const std::string& i_name) const;
  TargetType* GetTargetTypeByName(const std::string& i_name);

  static void Load(const nlohmann::json& i_json, Module& i_module);
  static Module Load(const nlohmann::json& i_json);
};

}
