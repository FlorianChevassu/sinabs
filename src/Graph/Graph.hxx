#pragma once

#include "Module/Module.hxx"

#include "Graph/Node.hxx"
#include "Graph/Project/Project.hxx"
#include "Graph/Project/Property.hxx"
#include "Graph/Project/Target.hxx"

#include <nlohmann/json.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>

namespace ngbs::graph
{
  class Graph
  {
  public:
    template <typename T>
    T* RegisterNode(std::unique_ptr<T>&& i_node)
    {
      auto res = i_node.get();
      m_nodes.push_back(std::move(i_node));
      m_nodesByType[res->GetNodeType()].push_back(res);
      return res;
    }

    template <typename T>
    T* FindNode(std::function<bool(const T&)> i_predicate)
    {
      auto& nodes = m_nodesByType[T::Type];

      auto it = std::find_if(nodes.begin(), nodes.end(),
        [&i_predicate](NodeBase* i_node)
        {
          T* concreteNode = dynamic_cast<T*>(i_node);
          return i_predicate(*concreteNode);
        }
      );

      if (it != nodes.end())
      {
        return dynamic_cast<T*>(*it);
      }
      return nullptr;
    }

    template <class T>
    const std::vector<NodeBase*>& GetNodesByType() const
    {
      auto it = m_nodesByType.find(T::Type);
      if (it != m_nodesByType.end())
      {
        return it->second;
      }
      //TODO: Should never happen. Should m_nodesByType be mutable so that we can use operator[] ?
      throw std::runtime_error("");
    }

    std::vector<const NodeBase*> GetNodes() const
    {
      std::vector<const NodeBase*> res;
      res.reserve(m_nodes.size());
      for (auto& node : m_nodes)
      {
        res.push_back(node.get());
      }
      return res;
    }

  private:
    std::vector<std::unique_ptr<NodeBase>> m_nodes;
    std::map<NodeType, std::vector<NodeBase*>> m_nodesByType;
  };

}
