#pragma once

#include "Graph/Graph.hxx"

#include <nlohmann/json.hpp>

#include <filesystem>

namespace ngbs::graph
{
  class ProjectBuilder
  {
  public:
    ProjectBuilder(Graph& i_graph) : m_graph(i_graph) {}
    void Process(const nlohmann::json& i_jsonProject);

    void SetSourceDirectory(const std::filesystem::path& i_path);
    void SetBuildDirectory(const std::filesystem::path& i_path);
  
  protected:
    const module::TargetType* FindTargetType(const std::string& i_targetTypeName);
    project::Target* LoadTarget(const nlohmann::json& i_jsonTarget, const module::TargetType& i_targetType);

    NodeBase* LoadProperty(const nlohmann::json& i_jsonProperty, const module::PropertyType& i_propertyType);
  private:
    Graph& m_graph;
    std::filesystem::path m_sourceDir;
    std::filesystem::path m_buildDir;
  };

  std::unique_ptr<project::Property> MakePropertyNode(const module::PropertyType& i_propertyType);

  template <typename T>
  std::unique_ptr<project::Property> MakePropertyNode(const module::PropertyType& i_propertyType, T&& i_value)
  {
    auto property = MakePropertyNode(i_propertyType);
    auto propertyPtr = dynamic_cast<project::ConcreteProperty<std::remove_reference_t<T>>*>(property.get());
    propertyPtr->SetValue(std::move(i_value));
    return property;
  }
}
