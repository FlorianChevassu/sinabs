#pragma once

#include "Graph/Node.hxx"

#include <string>

class Project : public NodeBase
{
public:
  NodeType GetNodeType() override { return NodeType::Project; }

  const std::string& GetName() const { return m_name; }
  void SetName(const std::string& i_name) { m_name = i_name; }
private:
  std::string m_name;
};
