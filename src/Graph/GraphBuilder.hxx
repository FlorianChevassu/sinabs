#pragma once

#include "Graph/Graph.hxx"
#include "Graph/Project/Project.hxx"
#include "Graph/Project/Property.hxx"
#include "Graph/Project/Target.hxx"

#include <nlohmann/json.hpp>

#include <filesystem>

namespace ngbs::graph
{
  class GraphBuilder
  {
  public:
    void Process(const nlohmann::json& i_jsonProject);

    Graph GetResult();

    void SetSourceDirectory(const std::filesystem::path& i_path);
    void SetBuildDirectory(const std::filesystem::path& i_path);

  private:
    Graph m_graph;
    std::filesystem::path m_sourceDir;
    std::filesystem::path m_buildDir;

  protected:
    module::Module* FindModule(const std::string& i_moduleName);
    void LoadModule(const std::string& i_moduleName);

    void LoadProject(const nlohmann::json& i_jsonProject);

    void PropagateProperties();
  };
}
