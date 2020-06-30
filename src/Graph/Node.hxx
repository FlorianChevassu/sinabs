#pragma once

#include <vector>

namespace ngbs::graph
{
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
    NodeBase(NodeBase* i_parent) : m_parent(i_parent){}
    virtual ~NodeBase() = default;
    virtual NodeType GetNodeType() = 0;

    std::vector<NodeBase*>& GetDependencies() { return m_dependencies; }
    const std::vector<NodeBase*>& GetDependencies() const { return m_dependencies; }

    void SetParent(NodeBase* i_parent) { m_parent = i_parent; }
    NodeBase* GetParent() { return m_parent; }
    const NodeBase* GetParent() const { return m_parent; }
  private:
    std::vector<NodeBase*> m_dependencies;
    NodeBase* m_parent = nullptr;
  };
}
