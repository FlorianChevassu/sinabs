#include "ScriptEngine.hxx"

#include "Graph/Graph.hxx"

#include "Graph/Module/PropertyType.hxx"
#include "Graph/Module/TargetType.hxx"

#include "Graph/Project/Property.hxx"
#include "Graph/Project/Target.hxx"

#include "Graph/ProjectBuilder.hxx"

#include "Utils/TypeTraits.hxx"

#include <filesystem>

namespace ngbs
{
  ScriptEngine::ScriptEngine()
  {
    AddModelClasses();
  }

  namespace
  {
    //-------------------------------------------------------------------
    template <class T>
    chaiscript::Boxed_Value GetBoxedValue(const T& i_value)
    {
      if constexpr (!utils::is_vector_v<T>)
      {
        return chaiscript::Boxed_Value(i_value);
      }
      else
      {
        // Otherwise, create a vector of boxed values
        std::vector<chaiscript::Boxed_Value> result;
        result.reserve(i_value.size());
        for (const auto& val : i_value)
        {
          result.push_back(chaiscript::Boxed_Value(val));
        }
        return chaiscript::Boxed_Value(result);
      }
    }

    //-------------------------------------------------------------------
    std::vector<const graph::project::Property*> GetTargetProperties(const graph::project::Target* i_target)
    {
      std::vector<const graph::project::Property*> result;

      const auto& dependencies = i_target->GetDependencies();
      for (const auto& dep : dependencies)
      {
        if (dep->GetNodeType() == graph::NodeType::Property)
        {
          result.push_back(dynamic_cast<const graph::project::Property*>(dep));
        }
      }
      return result;
    }

    //-------------------------------------------------------------------
    graph::project::Property* GetTargetProperty(graph::project::Target* i_target, const std::string& i_propertyName)
    {
      auto& dependencies = i_target->GetDependencies();
      for (auto& dep : dependencies)
      {
        if (dep->GetNodeType() == graph::NodeType::Property)
        {
          auto property = dynamic_cast<graph::project::Property*>(dep);
          if (property->GetName() == i_propertyName)
          {
            // TODO: If the property has an arity > 1, concatenate all the properties of this type.
            // If its arity is 1, throw an exception if iit has more than one possible value.
            return property;
          }
        }
      }
      return nullptr;
    }

    //-------------------------------------------------------------------
    graph::project::Property* AddTargetProperty(
      graph::Graph& i_graph,
      graph::project::Target* i_target,
      const std::string& i_propertyName)
    {
      auto propertyType = graph::module::FindPropertyTypeByName(i_target->GetType(), i_propertyName);
      if (propertyType == nullptr)
      {
        //TODO throw
        assert(false);
      }
      auto property = graph::MakePropertyNode(*propertyType);
      i_target->GetDependencies().push_back(property.get());
      return i_graph.RegisterNode(std::move(property));
    }

    //-------------------------------------------------------------------
    graph::project::Property* GetOrCreateTargetProperty(
      graph::Graph& i_graph,
      graph::project::Target* i_target,
      const std::string& i_propertyName)
    {
      auto property = GetTargetProperty(i_target, i_propertyName);
      if (property)
      {
        return property;
      }
      return AddTargetProperty(i_graph, i_target, i_propertyName);
    }

    //-------------------------------------------------------------------
    std::vector<graph::project::Target*> GetTargetDependentTargets(graph::project::Target* i_target)
    {
      std::vector<graph::project::Target*> result;

      auto& dependencies = i_target->GetDependencies();
      for (auto& dep : dependencies)
      {
        if (dep->GetNodeType() == graph::NodeType::Target)
        {
          result.push_back(dynamic_cast<graph::project::Target*>(dep));
        }
      }
      return result;
    };

    //-------------------------------------------------------------------
    std::string GetTargetType(const graph::project::Target* i_target)
    {
      return i_target->GetType().m_name;
    };

    //-------------------------------------------------------------------
    chaiscript::Boxed_Value GetPropertyValue(const graph::project::Property* i_property)
    {
      switch (i_property->GetDataType())
      {
      case graph::project::DataType::Bool:
        if (i_property->IsArray())
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<std::vector<bool>>*>(i_property)->GetValue());
        }
        else
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<bool>*>(i_property)->GetValue());
        }
      case graph::project::DataType::File:
      case graph::project::DataType::Folder:
        if (i_property->IsArray())
        {
          std::vector<std::string> val;
          auto prop = dynamic_cast<const graph::project::ConcreteProperty<std::vector<std::filesystem::path>>*>(i_property);
          for (const auto& p : prop->GetValue())
          {
            val.push_back(p.string());
          }
          return GetBoxedValue(val);
        }
        else
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<std::filesystem::path>*>(i_property)->GetValue().string());
        }
      case graph::project::DataType::Number:
        if (i_property->IsArray())
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<std::vector<int>>*>(i_property)->GetValue());
        }
        else
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<int>*>(i_property)->GetValue());
        }
      case graph::project::DataType::String:
        if (i_property->IsArray())
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<std::vector<std::string>>*>(i_property)->GetValue());
        }
        else
        {
          return GetBoxedValue(dynamic_cast<const graph::project::ConcreteProperty<std::string>*>(i_property)->GetValue());
        }
      case graph::project::DataType::Unknown:
        assert(false);
      }
    }

    //-------------------------------------------------------------------
    void SetPropertyVectorValue(graph::project::Property* i_property, const std::vector<chaiscript::Boxed_Value>& i_value)
    {
      assert(i_property->IsArray());
      switch (i_property->GetDataType())
      {
      case graph::project::DataType::Bool:
      {
        std::vector<bool> value;
        for (auto& val : i_value)
        {
          value.push_back(chaiscript::boxed_cast<bool>(val));
        }
        dynamic_cast<graph::project::ConcreteProperty<std::vector<bool>>*>(i_property)->SetValue(value);
        break;
      }
      case graph::project::DataType::File:
      case graph::project::DataType::Folder:
      {
        std::vector<std::filesystem::path> value;
        for (auto& val : i_value)
        {
          value.push_back(chaiscript::boxed_cast<std::string>(val));
        }
        dynamic_cast<graph::project::ConcreteProperty<std::vector<std::filesystem::path>>*>(i_property)->SetValue(value);
        break;
      }
      case graph::project::DataType::Number:
      {
        std::vector<int> value;
        for (auto& val : i_value)
        {
          value.push_back(chaiscript::boxed_cast<int>(val));
        }
        dynamic_cast<graph::project::ConcreteProperty<std::vector<int>>*>(i_property)->SetValue(value);
        break;
      }
      case graph::project::DataType::String:
      {
        std::vector<std::string> value;
        for (auto& val : i_value)
        {
          value.push_back(chaiscript::boxed_cast<std::string>(val));
        }
        dynamic_cast<graph::project::ConcreteProperty<std::vector<std::string>>*>(i_property)->SetValue(value);
        break;
      }
      case graph::project::DataType::Unknown:
        assert(false);
      }
    }

    //-------------------------------------------------------------------
    void SetPropertyValue(graph::project::Property* i_property, const chaiscript::Boxed_Value& i_value)
    {
      assert(!i_property->IsArray());
      switch (i_property->GetDataType())
      {
      case graph::project::DataType::Bool:
      {
        auto val = chaiscript::boxed_cast<bool>(i_value);
        dynamic_cast<graph::project::ConcreteProperty<bool>*>(i_property)->SetValue(val);
        break;
      }
      case graph::project::DataType::File:
      case graph::project::DataType::Folder:
      {
          auto val = chaiscript::boxed_cast<std::string>(i_value);
          dynamic_cast<graph::project::ConcreteProperty<std::string>*>(i_property)->SetValue(val);
          break;
      }
      case graph::project::DataType::Number:
      {
        auto val = chaiscript::boxed_cast<int>(i_value);
        dynamic_cast<graph::project::ConcreteProperty<int>*>(i_property)->SetValue(val);
        break;
      }
      case graph::project::DataType::String:
      {
        auto val = chaiscript::boxed_cast<std::string>(i_value);
        dynamic_cast<graph::project::ConcreteProperty<std::string>*>(i_property)->SetValue(val);
        break;
      }
      case graph::project::DataType::Unknown:
        assert(false);
      }
    }
  }

  //-------------------------------------------------------------------
  void ScriptEngine::AddModelClasses()
  {
    chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

    /////////////////////////////////////
    // PROPERTIES
    /////////////////////////////////////

    chaiscript::utility::add_class<graph::project::Property>(*m,
      "Property", {/* constructors */ },
      {
        {chaiscript::fun(&graph::project::Property::GetName), "GetName"},
        {chaiscript::fun(&graph::project::Property::GetDataType), "GetDataType"},
        {chaiscript::fun(&GetPropertyValue), "GetValue"},
        {chaiscript::fun(&SetPropertyValue), "SetValue"},
        {chaiscript::fun(&SetPropertyVectorValue), "SetValue"}
      }
      );
    m_engine.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<graph::project::Property*> >("Properties"));
    m_engine.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<const graph::project::Property*> >("Const_Properties"));

    chaiscript::utility::add_class<graph::project::DataType>(*m,
      "DataType",
      {
        { graph::project::DataType::Bool, "BOOL" },
        { graph::project::DataType::File, "FILE" },
        { graph::project::DataType::Folder, "FOLDER" },
        { graph::project::DataType::Number, "NUMBER" },
        { graph::project::DataType::String, "STRING" },
        { graph::project::DataType::Unknown, "UNKNOWN" },
      }
    );
    m_engine.add(chaiscript::type_conversion<graph::project::DataType, std::string>(&graph::project::GetStringFromPropertyType));

    /////////////////////////////////////
    // TARGETS
    /////////////////////////////////////

    chaiscript::utility::add_class<graph::project::Target>(*m,
      "Target", {/* constructors */ },
      {
        {chaiscript::fun(&graph::project::Target::GetName), "GetName"},
        //{chaiscript::fun(&graph::project::Target::GetType), "GetTargetType"},
        {chaiscript::fun(&GetTargetProperties), "GetProperties"},
        {chaiscript::fun(&GetTargetProperty), "GetProperty"},
        {chaiscript::fun(
          [&](graph::project::Target* i_target,
              const std::string& i_propertyName) {
                return GetOrCreateTargetProperty(this->GetGraph(), i_target, i_propertyName);
            }
          ), "GetOrCreateProperty"},
        {chaiscript::fun(
          [&](graph::project::Target* i_target,
              const std::string& i_propertyName) {
                return AddTargetProperty(this->GetGraph(), i_target, i_propertyName);
            }
          ), "AddProperty"},
        {chaiscript::fun(&GetTargetDependentTargets), "GetDependentTargets"},
        {chaiscript::fun(&GetTargetType), "GetType"}
      }
      );
    m_engine.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<graph::project::Target*> >("Targets"));
    m_engine.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<const graph::project::Target*> >("Const_Targets"));

    m_engine.add(chaiscript::fun([]()
      {
        _CrtDbgBreak();
      }), "pause");


    m_engine.add(chaiscript::fun([&](const std::string& i_source, const std::string& i_base)
      {
        std::filesystem::path source = i_source;
        std::filesystem::path base = i_base;
        return std::filesystem::relative(source, base).string();
      }), "RelativeTo");


    m_engine.add(m);
  }

  void ScriptEngine::AddTarget(graph::project::Target& i_target)
  {
    m_engine.add(chaiscript::var(&i_target), "target");
  }

  std::string ScriptEngine::Evaluate(const std::string& i_script)
  {
    try
    {
      return m_engine.eval<std::string>(i_script);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      std::cerr << e.what() << std::endl;
    }
    // TODO: throw
    return "";
  }
}

