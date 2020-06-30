#pragma once

#include "Graph/Node.hxx"

#include <string>

namespace ngbs::graph::project
{
  class Project : public NodeBase
  {
  public:
    Project() : NodeBase(nullptr) {}
    static const NodeType Type = NodeType::Project;
    NodeType GetNodeType() override { return Type; }

    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& i_name) { m_name = i_name; }
  private:
    std::string m_name;
  };
}
