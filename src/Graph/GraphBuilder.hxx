#pragma once

#include "Graph/Graph.hxx"
#include "Graph/Project/Project.hxx"
#include "Graph/Project/Property.hxx"
#include "Graph/Project/Target.hxx"

#include <nlohmann/json.hpp>

namespace ngbs::graph
{
  class GraphBuilder
  {
  public:
    void Process(const nlohmann::json& i_jsonProject);

    Graph GetResult()
    {
      Graph tmp;
      std::swap(tmp, m_graph);
      return tmp;
    }
  private:
    Graph m_graph;

  protected:
    module::Module* FindModule(const std::string& i_moduleName);
    void LoadModule(const std::string& i_moduleName);

    void LoadProject(const nlohmann::json& i_jsonProject);
  };
}
