#pragma once

#include "Graph/Node.hxx"

#include <string>
namespace ngbs::graph::module
{
  class TargetType;
}

class Target : public NodeBase
{
public:
  Target(const ngbs::graph::module::TargetType& i_type) : m_type(i_type){}
  NodeType GetNodeType() override { return NodeType::Target; }

  const std::string& GetName() const { return m_name; }
  void SetName(const std::string& i_name) { m_name = i_name; }

  const ngbs::graph::module::TargetType& GetType() const { return m_type; }
private:
  std::string m_name;
  const ngbs::graph::module::TargetType& m_type;
};
