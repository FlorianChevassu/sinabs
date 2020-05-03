#include "Graph/ProjectBuilder.hxx"

#include "Graph/Project/Project.hxx"


#include <memory>

namespace ngbs::graph
{
  void ProjectBuilder::Process(const nlohmann::json& i_json)
  {
    auto project = std::make_unique<Project>();

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

  const module::PropertyType* ProjectBuilder::FindPropertyTypeByName(const module::TargetType& i_targetType, const std::string& i_name)
  {
    auto it = std::find_if(i_targetType.m_properties.begin(), i_targetType.m_properties.end(),
      [&i_name](const auto& prop)
      {
        return prop.m_name == i_name;
      }
    );
    if (it != i_targetType.m_properties.end())
    {
      return &*it;
    }
    return nullptr;
  }

  const module::InlineTarget* ProjectBuilder::FindInlineTargetByName(const std::vector<module::InlineTarget>& i_inlineTargets, const std::string& i_name)
  {
    auto inlineTargetIt = std::find_if(i_inlineTargets.begin(), i_inlineTargets.end(),
      [&i_name](const auto& i_inlineTarget)
      {
        return i_inlineTarget.m_name == i_name;
      }
    );
    if (inlineTargetIt != i_inlineTargets.end())
    {
      return &*inlineTargetIt;
    }
    return nullptr;
  }

  NodeBase* ProjectBuilder::LoadProperty(const nlohmann::json& i_jsonProperty, const module::PropertyType& i_propertyType)
  {
    bool isArray = (i_propertyType.m_value.m_arity.m_value != 1);
    switch (GetPropertyTypeFromString(i_propertyType.m_value.m_type))
    {
    case PropertyType::Bool:
    {
      if (isArray)
      {
        auto property = std::make_unique<Property<std::vector<bool>>>();
        property->SetName(i_propertyType.m_name);
        std::vector<bool> val;
        i_jsonProperty.get_to(val);
        property->SetValue(val);
        return m_graph.RegisterNode(std::move(property));
      }
      else
      {
        auto property = std::make_unique<Property<bool>>();
        property->SetName(i_propertyType.m_name);
        bool val;
        i_jsonProperty.get_to(val);
        property->SetValue(val);
        return m_graph.RegisterNode(std::move(property));
      }
    }
    case PropertyType::File:
    case PropertyType::Folder:
    {
      if (isArray)
      {
        auto property = std::make_unique<Property<std::vector<std::filesystem::path>>>();
        property->SetName(i_propertyType.m_name);
        std::vector<std::string> val;
        i_jsonProperty.get_to(val);
        std::vector<std::filesystem::path> p(val.size());
        for (const auto& s : val)
        {
          p.push_back(s);
        }
        property->SetValue(p);
        return m_graph.RegisterNode(std::move(property));
      }
      else
      {
        auto property = std::make_unique<Property<std::filesystem::path>>();
        property->SetName(i_propertyType.m_name);
        std::string val;
        i_jsonProperty.get_to(val);
        std::filesystem::path p = val;
        property->SetValue(p);
        return m_graph.RegisterNode(std::move(property));
      }
    }
    case PropertyType::Number:
    {
      if (isArray)
      {
        auto property = std::make_unique<Property<std::vector<int>>>();
        property->SetName(i_propertyType.m_name);
        std::vector<int> val;
        i_jsonProperty.get_to(val);
        property->SetValue(val);
        return m_graph.RegisterNode(std::move(property));
      }
      else
      {
        auto property = std::make_unique<Property<int>>();
        property->SetName(i_propertyType.m_name);
        int val;
        i_jsonProperty.get_to(val);
        property->SetValue(val);
        return m_graph.RegisterNode(std::move(property));
      }
    }
    case PropertyType::String:
    {
      if (isArray)
      {
        auto property = std::make_unique<Property<std::vector<std::string>>>();
        property->SetName(i_propertyType.m_name);
        std::vector<std::string> val;
        i_jsonProperty.get_to(val);
        property->SetValue(val);
        return m_graph.RegisterNode(std::move(property));
      }
      else
      {
        auto property = std::make_unique<Property<std::string>>();
        property->SetName(i_propertyType.m_name);
        std::string val;
        i_jsonProperty.get_to(val);
        property->SetValue(val);
        return m_graph.RegisterNode(std::move(property));
      }
    }
    }
    return nullptr;
  }

  Target* ProjectBuilder::LoadTarget(const nlohmann::json& i_jsonTarget, const module::TargetType& i_targetType)
  {
    auto target = std::make_unique<Target>(i_targetType);

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
      auto propertyType = this->FindPropertyTypeByName(i_targetType, *i_targetType.m_defaultProperty);
      // TODO: throw ?
      if (!propertyType)
      {
        // TODO
        throw std::runtime_error("");
      }

      target->GetDependencies().push_back(
        this->LoadProperty(i_jsonTarget, *propertyType)
      );
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

      target->GetDependencies().push_back(
        this->LoadTarget(i_jsonTarget, *targetType)
      );
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
        auto inlineTarget = this->FindInlineTargetByName(i_targetType.m_inlineTargets, key);
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
            target->GetDependencies().push_back(
              this->LoadTarget(jsonTarget, *targetType)
            );
          }
        }
        else if (val.is_object() || val.is_string())
        {
          target->GetDependencies().push_back(
            this->LoadTarget(val, *targetType)
          );
        }

        targetType->GenerateRule(*target);
      }

      // Load properties
      if (i_jsonTarget.find("properties") != i_jsonTarget.end())
      {
        auto jsonProperties = i_jsonTarget.at("properties");
        for (auto& [key, val] : jsonProperties.items())
        {
          // Find property type
          auto propertyType = this->FindPropertyTypeByName(target->GetType(), key);
          if (!propertyType)
          {
            throw std::runtime_error("");
            // TODO
          }
          target->GetDependencies().push_back(
            this->LoadProperty(val, *propertyType)
          );
        }
      }
    }

    return m_graph.RegisterNode(std::move(target));
  }
}
