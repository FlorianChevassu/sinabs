#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>

namespace ngbs::graph
{
  namespace module
  {
    class Module;
    class TargetType;
  }
  class Graph;

  class ModuleBuilder
  {
  public:
    ModuleBuilder(Graph& i_graph) : m_graph(i_graph) {}
    void Process(const std::string& i_moduleName);
    void Process(const nlohmann::json& i_jsonProject);

    void SetSourceDirectory(const std::filesystem::path& i_path);
    void SetBuildDirectory(const std::filesystem::path& i_path);

  protected:
    void ProcessNewModule(const nlohmann::json& i_jsonProject);
    void ExtendModule(const nlohmann::json& i_jsonProject, module::Module& i_module);
    std::string GetModuleFromTargetId(const std::string& i_targetId);
    module::TargetType* GetTargetTypeByName(const std::string& i_name);

  private:
    Graph& m_graph;
    std::filesystem::path m_sourceDir;
    std::filesystem::path m_buildDir;
  };
}
