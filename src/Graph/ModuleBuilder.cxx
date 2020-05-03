#include "Graph/ModuleBuilder.hxx"

#include "Graph/Graph.hxx"

#include <memory>

namespace ngbs::graph
{
  void ModuleBuilder::Process(const std::string& i_moduleName)
  {
    auto path = std::filesystem::current_path();
    auto module_root = path / "modules" / i_moduleName;
    auto module_file = path / "modules" / i_moduleName / (i_moduleName + "_module.ngbs");
    if (!std::filesystem::exists(module_file))
    {
      std::cerr << "Module " << i_moduleName << " does not exist !";
      assert(false);
      return;
      //Throw
    }


    // Make sure we process the module file first
    this->Process(nlohmann::json::parse(std::ifstream(module_file)));
    for (auto& file : std::filesystem::directory_iterator(module_root))
    {
      if (file != module_file)
      {
        this->Process(nlohmann::json::parse(std::ifstream(file)));
      }
    }
  }

  void ModuleBuilder::Process(const nlohmann::json& i_json)
  {
    // First build dependent modules
    if (i_json.find("depends") != i_json.end())
    {
      for (std::string moduleName : i_json.at("depends"))
      {
        // TODO: Make sure there is no infinite loop...
        this->Process(moduleName);
      }
    }
    

    bool hasExtendKeyword = i_json.find("extends") != i_json.end();
    if (!hasExtendKeyword)
    {
      this->ProcessNewModule(i_json);
    }
    else
    {
      // Find the module that is extended
      std::string moduleName = i_json.at("extends");
      auto module = m_graph.FindNode<module::Module>([moduleName](const module::Module& i_module)
        {
          return i_module.m_name == moduleName;
        }
      );

      if (!module)
      {
        assert(false);
        return;
      }
      this->ExtendModule(i_json, *module);
    }
  }

  void ModuleBuilder::ExtendModule(const nlohmann::json& i_json, module::Module& i_module)
  {
    for (const auto& json_target_type : i_json.at("target_types"))
    {
      auto targetType = i_module.GetTargetTypeByName(json_target_type.at("name"));
      if (targetType)
      {
        targetType->Load(json_target_type);
      }
      else
      {
        module::TargetType tt;// (module->m_scriptEngine);
        tt.Load(json_target_type);
        i_module.m_targetTypes.push_back(std::move(tt));
      }
    }
  }

  void ModuleBuilder::ProcessNewModule(const nlohmann::json& i_json)
  {
    auto module = std::make_unique<module::Module>();
    i_json.at("name").get_to(module->m_name);
    for (const auto& json_target_type : i_json.at("target_types"))
    {
      auto targetType = module->GetTargetTypeByName(json_target_type.at("name"));
      if (targetType)
      {
        targetType->Load(json_target_type);
      }
      else
      {
        module::TargetType tt;// (module->m_scriptEngine);
        tt.Load(json_target_type);
        module->m_targetTypes.push_back(std::move(tt));
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
        //module->m_scriptEngine->eval_file(module_script.string());
      }
    }

    m_graph.RegisterNode(std::move(module));
  }
}
