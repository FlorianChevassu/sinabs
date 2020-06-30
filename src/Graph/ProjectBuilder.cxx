#include "Graph/ProjectBuilder.hxx"

#include "Graph/Project/Project.hxx"


#include <memory>

namespace ngbs::graph
{

  void ProjectBuilder::SetSourceDirectory(const std::filesystem::path& i_path)
  {
    m_sourceDir = i_path;
  }

  void ProjectBuilder::SetBuildDirectory(const std::filesystem::path& i_path)
  {
    m_buildDir = i_path;
  }

  void ProjectBuilder::Process(const nlohmann::json& i_json)
  {
    auto project = std::make_unique<project::Project>();

    auto jsonProject = i_json.at("project");
    project->SetName(jsonProject.at("name"));

    for (auto& jsonTarget : jsonProject.at("targets"))
    {
      auto targetType = this->FindTargetType(jsonTarget.at("type"));
      if (targetType == nullptr)
      {
        // Exception...
        assert(false);
      }
      project->GetDependencies().push_back(this->LoadTarget(jsonTarget, *targetType));
    }

    m_graph.RegisterNode(std::move(project));
  }

  const module::TargetType* ProjectBuilder::FindTargetType(const std::string& i_targetTypeName)
  {
    const auto& moduleNodes = m_graph.GetNodesByType<module::Module>();

    for (const auto& moduleNode : moduleNodes)
    {
      const auto& module = dynamic_cast<module::Module&>(*moduleNode);
      for (const auto& targetType : module.m_targetTypes)
      {
        if (targetType.m_name == i_targetTypeName)
        {
          return &targetType;
        }
      }
    }
    return nullptr;
  }

  std::unique_ptr<project::Property> MakePropertyNode(const module::PropertyType& i_propertyType)
  {
    bool isArray = (i_propertyType.m_value.m_arity.m_value != 1);
    project::DataType propertyType = project::GetPropertyTypeFromString(i_propertyType.m_value.m_type);
    std::unique_ptr<project::Property> property;
    switch (propertyType)
    {
    case project::DataType::Bool:
    {
      if (isArray)
      {
        property = std::make_unique<project::ConcreteProperty<std::vector<bool>>>(nullptr, i_propertyType);
      }
      else
      {
        property = std::make_unique<project::ConcreteProperty<bool>>(nullptr, i_propertyType);
      }
      break;
    }
    case project::DataType::File:
    case project::DataType::Folder:
    {
      if (isArray)
      {
        property = std::make_unique<project::ConcreteProperty<std::vector<std::filesystem::path>>>(nullptr, i_propertyType);
      }
      else
      {
        property = std::make_unique<project::ConcreteProperty<std::filesystem::path>>(nullptr, i_propertyType);
      }
      break;
    }
    case project::DataType::Number:
    {
      if (isArray)
      {
        property = std::make_unique<project::ConcreteProperty<std::vector<int>>>(nullptr, i_propertyType);
      }
      else
      {
        property = std::make_unique<project::ConcreteProperty<int>>(nullptr, i_propertyType);
      }
      break;
    }
    case project::DataType::String:
    {
      if (isArray)
      {
        property = std::make_unique<project::ConcreteProperty<std::vector<std::string>>>(nullptr, i_propertyType);
      }
      else
      {
        property = std::make_unique<project::ConcreteProperty<std::string>>(nullptr, i_propertyType);
      }
      break;
    }
    }

    property->SetIsArray(
      i_propertyType.m_value.m_arity.m_value == -1
      || i_propertyType.m_value.m_arity.m_value > 1
    );
    property->SetName(i_propertyType.m_name);
    property->SetDataType(project::GetPropertyTypeFromString(i_propertyType.m_value.m_type));

    return property;
  }

  NodeBase* ProjectBuilder::LoadProperty(const nlohmann::json& i_jsonProperty, const module::PropertyType& i_propertyType)
  {
    bool isArray = (i_propertyType.m_value.m_arity.m_value != 1);
    project::DataType propertyType = project::GetPropertyTypeFromString(i_propertyType.m_value.m_type);
    std::unique_ptr<project::Property> property;
    switch (propertyType)
    {
    case project::DataType::Bool:
    {
      if (isArray)
      {
        std::vector<bool> val;
        i_jsonProperty.get_to(val);
        property = MakePropertyNode<>(i_propertyType, val);
      }
      else
      {
        bool val;
        i_jsonProperty.get_to(val);
        property = MakePropertyNode<>(i_propertyType, std::move(val));
      }
      break;
    }
    case project::DataType::File:
    case project::DataType::Folder:
    {
      if (isArray)
      {
        std::vector<std::string> val;
        i_jsonProperty.get_to(val);

        std::vector<std::filesystem::path> paths;
        paths.reserve(val.size());
        for (const auto& s : val)
        {
          std::filesystem::path p = m_sourceDir / s;
          if (!std::filesystem::exists(p))
          {
            // Exception
            assert(false);
          }
          paths.push_back(p);
        }
        property = MakePropertyNode<>(i_propertyType, std::move(paths));
      }
      else
      {
        std::string val;
        i_jsonProperty.get_to(val);

        std::filesystem::path p = m_sourceDir / val;
        if (!std::filesystem::exists(p))
        {
          // Exception
          assert(false);
        }
        property = MakePropertyNode<>(i_propertyType, std::move(p));
      }
      break;
    }
    case project::DataType::Number:
    {
      if (isArray)
      {
        std::vector<int> val;
        i_jsonProperty.get_to(val);
        property = MakePropertyNode<>(i_propertyType, val);
      }
      else
      {
        int val;
        i_jsonProperty.get_to(val);
        property = MakePropertyNode<>(i_propertyType, std::move(val));
      }
      break;
    }
    case project::DataType::String:
    {
      if (isArray)
      {
        std::vector<std::string> val;
        i_jsonProperty.get_to(val);
        property = MakePropertyNode<>(i_propertyType, val);
      }
      else
      {
        std::string val;
        i_jsonProperty.get_to(val);
        property = MakePropertyNode<>(i_propertyType, std::move(val));
      }
      break;
    }
    }
    return m_graph.RegisterNode(std::move(property));
  }

  project::Target* ProjectBuilder::LoadTarget(const nlohmann::json& i_jsonTarget, const module::TargetType& i_targetType)
  {
    auto target = std::make_unique<project::Target>(nullptr, i_targetType);

    auto GetDefaultInlineTarget = [&i_targetType]() -> const module::InlineTarget*
    {
      auto it = std::find_if(
        i_targetType.m_inlineTargets.begin(), i_targetType.m_inlineTargets.end(),
        [](const module::InlineTarget& i_inlineTarget)
          {
            return i_inlineTarget.m_default;
          }
      );
      if (it != i_targetType.m_inlineTargets.end())
      {
        return &*it;
      }
      return nullptr;
    };

    if (i_targetType.m_defaultProperty && i_jsonTarget.is_string())
    {
      // If the target type has a default property and i_jsonTarget is a string, use it to initialize the target.
      auto propertyType = module::FindPropertyTypeByName(i_targetType, *i_targetType.m_defaultProperty);
      // TODO: throw ?
      if (!propertyType)
      {
        // TODO
        throw std::runtime_error("");
      }

      auto prop = this->LoadProperty(i_jsonTarget, *propertyType);
      prop->SetParent(target.get());
      target->GetDependencies().push_back(prop);
    }
    else if (auto defaultInlineTarget = GetDefaultInlineTarget() && i_jsonTarget.is_string())
    {
      // If the target type has a default inline target, and i_jsonTarget is a string, use it to initialize the target.
      const auto& inlineTarget = i_targetType.m_inlineTargets[0];
      auto targetType = this->FindTargetType(inlineTarget.m_type);
      if (targetType == nullptr)
      {
        // Exception...
        assert(false);
        throw std::runtime_error("");
      }

      auto depTarget = this->LoadTarget(i_jsonTarget, *targetType);
      depTarget->SetParent(target.get());
      target->GetDependencies().push_back(depTarget);
    }
    else
    {
      if (i_jsonTarget.find("name") != i_jsonTarget.end())
      {
        target->SetName(i_jsonTarget.at("name"));
      }
      else
      {
        // TODO: If the target has no name, put it is some store where we can find duplicates ?
      }

      // Load inline targets
      for (auto& [key, val] : i_jsonTarget.items())
      {
        auto inlineTarget = module::FindInlineTargetByName(i_targetType, key);
        if (!inlineTarget)
        {
          continue;
        }

        // Create sub targets
        auto targetType = this->FindTargetType(inlineTarget->m_type);
        if (targetType == nullptr)
        {
          // Exception...
          assert(false);
          throw std::runtime_error("");
        }

        if (val.is_array())
        {
          for (auto& jsonTarget : val)
          {
            auto depTarget = this->LoadTarget(jsonTarget, *targetType);
            depTarget->SetParent(target.get());
            target->GetDependencies().push_back(depTarget);
          }
        }
        else if (val.is_object() || val.is_string())
        {
          auto depTarget = this->LoadTarget(val, *targetType);
          depTarget->SetParent(target.get());
          target->GetDependencies().push_back(depTarget);
        }
      }

      // Load properties
      if (i_jsonTarget.find("properties") != i_jsonTarget.end())
      {
        auto jsonProperties = i_jsonTarget.at("properties");
        for (auto& [key, val] : jsonProperties.items())
        {
          // Find property type
          auto propertyType = module::FindPropertyTypeByName(target->GetType(), key);
          if (!propertyType)
          {
            throw std::runtime_error("");
            // TODO
          }
          auto prop = this->LoadProperty(val, *propertyType);
          prop->SetParent(target.get());
          target->GetDependencies().push_back(prop);
        }
      }
    }

    return m_graph.RegisterNode(std::move(target));
  }
}
