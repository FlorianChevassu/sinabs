#include "Module.hxx"

#include "ModuleItem.hxx" // For utils. To remove


#include <filesystem>

namespace ngbs::graph::module
{
  Module::Module()
  {
    m_scriptEngine = std::make_unique<chaiscript::ChaiScript>();
    
    // Inject the required functions
    m_scriptEngine->eval(
      R"__(
      def function_exists(String name) {
        return contains(get_function_objects(), fun(elem) { return elem.first == name; });
      })__"
      );

  }

  TargetType* Module::GetTargetTypeByName(const std::string& i_name)
  {
    return utils::GetObjectByName<TargetType, &TargetType::m_name>(this->m_targetTypes, i_name);
  }

  const TargetType* Module::GetTargetTypeByName(const std::string& i_name) const
  {
    return const_cast<Module*>(this)->GetTargetTypeByName(i_name);
  }

void Module::Load(const nlohmann::json& i_json, Module& i_module)
{
  i_json.at("name").get_to(i_module.m_name);
  for (const auto& json_target_type : i_json.at("target_types"))
  {
    auto targetType = i_module.GetTargetTypeByName(json_target_type.at("name"));
    if (targetType)
    {
      targetType->Load(json_target_type);
    }
    else
    {
      TargetType tt;// (*i_module.m_scriptEngine);
      tt.Load(json_target_type);
      i_module.m_targetTypes.push_back(std::move(tt));
    }
  }

  // Execute the corresponding module script, if any.
  if (i_json.find("script") != i_json.end())
  {
    std::string script = i_json.at("script");
    // TODO: save the path of the module file to be able to find the script relatively
    auto path = std::filesystem::current_path();
    auto module_script = path / script;
    if (std::filesystem::exists(module_script))
    {
      //i_module.m_scriptEngine->eval_file(module_script.string());
    }
  }
}

Module Module::Load(const nlohmann::json& i_json)
{
  Module res;
  Load(i_json, res);
  return res;
}

}
