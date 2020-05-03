#pragma once

#include "Graph/Node.hxx"

#include "ModuleItem.hxx"

#include "Graph/Module/Arity.hxx"

#include <nlohmann/json.hpp>

#include <string>

namespace ngbs::graph::module
{
  class PropertyType : public NodeBase
  {
  public:
    static const NodeType Type = NodeType::PropertyType;
    NodeType GetNodeType() override { return PropertyType::Type; }



    void Load(const nlohmann::json& i_json);

    std::string m_name;

    struct Value
    {
      std::string m_type;
      Arity m_arity = 1;
    };
    Value m_value;

    enum class Propagation
    {
      None = 0,
      Children = 1 << 0,
      Parent = 1 << 1
    };

    inline friend Propagation  operator~  (Propagation a)                 { return (Propagation)~(int)a; }
    inline friend Propagation  operator|  (Propagation a,  Propagation b) { return (Propagation)((int)a | (int)b); }
    inline friend Propagation  operator&  (Propagation a,  Propagation b) { return (Propagation)((int)a & (int)b); }
    inline friend Propagation  operator^  (Propagation a,  Propagation b) { return (Propagation)((int)a ^ (int)b); }
    inline friend Propagation& operator|= (Propagation& a, Propagation b) { return (Propagation&)((int&)a |= (int)b); }
    inline friend Propagation& operator&= (Propagation& a, Propagation b) { return (Propagation&)((int&)a &= (int)b); }
    inline friend Propagation& operator^= (Propagation& a, Propagation b) { return (Propagation&)((int&)a ^= (int)b); }

    Propagation m_default_propagation = Propagation::None;
  };

}
