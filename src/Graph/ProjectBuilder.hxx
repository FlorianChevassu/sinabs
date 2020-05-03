#pragma once

#include "Graph/Graph.hxx"

#include <nlohmann/json.hpp>

namespace ngbs::graph
{
  class ProjectBuilder
  {
  public:
    ProjectBuilder(Graph& i_graph) : m_graph(i_graph) {}
    void Process(const nlohmann::json& i_jsonProject);
  
  protected:
    const module::TargetType* FindTargetType(const std::string& i_targetTypeName);
    Target* LoadTarget(const nlohmann::json& i_jsonTarget, const module::TargetType& i_targetType);

    const module::PropertyType* FindPropertyTypeByName(const module::TargetType& i_targetType, const std::string& i_name);
    const module::InlineTarget* FindInlineTargetByName(const std::vector<module::InlineTarget>& i_inlineTargets, const std::string& i_name);


    NodeBase* LoadProperty(const nlohmann::json& i_jsonProperty, const module::PropertyType& i_propertyType);
  private:
    Graph& m_graph;
  };
}
