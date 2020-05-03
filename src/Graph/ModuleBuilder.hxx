#pragma once

#include <nlohmann/json.hpp>

namespace ngbs::graph
{
  namespace module
  {
    class Module;
  }
  class Graph;

  class ModuleBuilder
  {
  public:
    ModuleBuilder(Graph& i_graph) : m_graph(i_graph) {}
    void Process(const std::string& i_moduleName);
    void Process(const nlohmann::json& i_jsonProject);

  protected:
    void ProcessNewModule(const nlohmann::json& i_jsonProject);
    void ExtendModule(const nlohmann::json& i_jsonProject, module::Module& i_module);

  private:
    Graph& m_graph;
  };
}
