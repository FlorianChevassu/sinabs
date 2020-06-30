#include "Graph/GraphBuilder.hxx"

#include "Graph/ModuleBuilder.hxx"
#include "Graph/ProjectBuilder.hxx"

#include "Graph/Module/PropertyType.hxx"


namespace ngbs::graph
{
  Graph GraphBuilder::GetResult()
  {
    Graph tmp;
    std::swap(tmp, m_graph);
    return tmp;
  }

  void GraphBuilder::SetSourceDirectory(const std::filesystem::path&i_path)
  {
    m_sourceDir = i_path;
  }

  void GraphBuilder::SetBuildDirectory(const std::filesystem::path& i_path)
  {
    m_buildDir = i_path;
  }

  void GraphBuilder::Process(const nlohmann::json& i_jsonProject)
  {
    // Make sure the core module is always loaded first
    this->LoadModule("core");

    for (auto jsonModule : i_jsonProject.at("modules"))
    {
      this->LoadModule(jsonModule);
    }

    this->LoadProject(i_jsonProject);

    this->PropagateProperties();
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
    moduleBuilder.SetSourceDirectory(m_sourceDir);
    moduleBuilder.SetBuildDirectory(m_buildDir);
    moduleBuilder.Process(i_moduleName);
  }

  void GraphBuilder::LoadProject(const nlohmann::json& i_jsonProject)
  {
    ProjectBuilder projectBuilder(m_graph);
    projectBuilder.SetSourceDirectory(m_sourceDir);
    projectBuilder.SetBuildDirectory(m_buildDir);
    projectBuilder.Process(i_jsonProject);
  }

  void GraphBuilder::PropagateProperties()
  {
    auto propertyNodes = m_graph.GetNodesByType<project::Property>();
    for (auto node : propertyNodes)
    {
      auto& property = *dynamic_cast<project::Property*>(node);
      const auto& propertyType = property.GetPropertyType();

      auto target = dynamic_cast<project::Target*>(property.GetParent());
      if (!target)
      {
        // TODO: throw. A property should ALWAYS have a parent target.
        assert(false);
        continue;
      }

      auto SetPropertyIfValid = [](project::Property& i_property, project::Target& i_target)
      {
        const auto& targetProps = i_target.GetType().m_properties;
        auto it = std::find_if(targetProps.begin(), targetProps.end(),
          [&](const module::PropertyType& i_propertyType)
          {
            return i_property.GetPropertyType().m_name == i_propertyType.m_name;
          }
        );
        if (it != targetProps.end())
        {
          i_target.GetDependencies().push_back(&i_property);
        }
      };

      auto PropagateToChildren = [&property, SetPropertyIfValid](NodeBase* i_node, bool i_recursive = false)
      {
        auto Impl = [&property, SetPropertyIfValid](NodeBase* i_node, bool i_recursive, auto& Propagate) -> void
        {
          for (auto child : i_node->GetDependencies())
          {
            auto childTarget = dynamic_cast<project::Target*>(child);
            if (!childTarget)
            {
              continue;
            }
            SetPropertyIfValid(property, *childTarget);
            if (i_recursive)
            {
              Propagate(child, i_recursive, Propagate);
            }
          }
        };
        Impl(i_node, i_recursive, Impl);
      };

      // TODO: Also take into account what propagation is defined on the property itself
      switch (propertyType.m_default_propagation)
      {
      case module::PropertyType::Propagation::Children:
      {
        PropagateToChildren(target);
        break;
      }
      case module::PropertyType::Propagation::Descendents:
      {
        PropagateToChildren(target, true);
        break;
      }
      case module::PropertyType::Propagation::Parent:
      {
        auto parentTarget = dynamic_cast<project::Target*>(target->GetParent());
        if (parentTarget)
        {
          SetPropertyIfValid(property , *parentTarget);
        }
        break;
      }
      case module::PropertyType::Propagation::None:
      default:
        break;
      }
    }
  }
}
