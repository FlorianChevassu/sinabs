#include "Graph/GraphBuilder.hxx"

#include "Graph/ModuleBuilder.hxx"
#include "Graph/ProjectBuilder.hxx"


namespace ngbs::graph
{
  void GraphBuilder::Process(const nlohmann::json& i_jsonProject)
  {
    for (auto jsonModule : i_jsonProject.at("modules"))
    {
      this->LoadModule(jsonModule);
    }

    this->LoadProject(i_jsonProject);
  }

  module::Module* GraphBuilder::FindModule(const std::string& i_moduleName)
  {
    return m_graph.FindNode<module::Module>([i_moduleName](const module::Module& i_module)
      {
        return i_module.m_name == i_moduleName;
      });
  }

  void GraphBuilder::LoadModule(const std::string& i_moduleName)
  {
    // First check that the module is not already loaded
    if (FindModule(i_moduleName))
    {
      return;
    }

    ModuleBuilder moduleBuilder(m_graph);
    moduleBuilder.Process(i_moduleName);
  }

  void GraphBuilder::LoadProject(const nlohmann::json& i_jsonProject)
  {
    ProjectBuilder projectBuilder(m_graph);
    projectBuilder.Process(i_jsonProject);
  }
}
