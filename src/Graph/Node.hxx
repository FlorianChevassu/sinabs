#pragma once

#include <vector>

enum class NodeType
{
  Property,
  Target,
  Project,
  Module,
  TargetType,
  PropertyType
};

class NodeBase
{
public:
  virtual ~NodeBase() = default;
  virtual NodeType GetNodeType() = 0;

  std::vector<NodeBase*>& GetDependencies() { return m_dependencies; }
  const std::vector<NodeBase*>& GetDependencies() const { return m_dependencies; }
private:
  std::vector<NodeBase*> m_dependencies;
};
