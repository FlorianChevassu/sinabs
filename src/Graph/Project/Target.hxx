#pragma once

#include "Graph/Node.hxx"

#include <string>

namespace ngbs::graph
{
  namespace module
  {
    class TargetType;
  }
  namespace project
  {
    class Target : public NodeBase
    {
    public:
      Target(NodeBase* i_parent, const module::TargetType& i_type)
        : NodeBase(i_parent)
        , m_type(i_type)
      {
      }

      Target(const Target&) = delete;

      static const NodeType Type = NodeType::Target;
      NodeType GetNodeType() override { return Type; }

      const std::string& GetName() const { return m_name; }
      void SetName(const std::string& i_name) { m_name = i_name; }

      const module::TargetType& GetType() const { return m_type; }
    private:
      std::string m_name;
      const module::TargetType& m_type;
    };
  }
}
