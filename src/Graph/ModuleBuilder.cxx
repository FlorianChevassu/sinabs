#include "Graph/ModuleBuilder.hxx"

#include "Graph/Graph.hxx"

#include <memory>

namespace ngbs::graph
{
  void ModuleBuilder::SetSourceDirectory(const std::filesystem::path& i_path)
  {
    m_sourceDir = i_path;
  }

  void ModuleBuilder::SetBuildDirectory(const std::filesystem::path& i_path)
  {
    m_buildDir = i_path;
  }

  void ModuleBuilder::Process(const std::string& i_moduleName)
  {
    // For now modules are supposed to be in the source directory. Add a "standard" module dir, in the NGBS installation folder for example.
    auto module_root = m_sourceDir / "modules" / i_moduleName;
    auto module_file = m_sourceDir / "modules" / i_moduleName / (i_moduleName + "_module.ngbs");
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
        std::filesystem::path filePath = file;
        if (filePath.extension() != ".ngbs")
        {
          continue;
        }
        try
        {
          this->Process(nlohmann::json::parse(std::ifstream(filePath)));
        }
        catch (std::exception& e)
        {
          std::cerr << e.what();
          int k = 2;
        }
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
        auto baseTargetType = i_module.GetTargetTypeByName(json_target_type.at("base_target"));
        assert(baseTargetType);
        module::TargetType tt = *baseTargetType;
        tt.Load(json_target_type);
        i_module.m_targetTypes.push_back(std::move(tt));
      }
    }
  }

  std::string ModuleBuilder::GetModuleFromTargetId(const std::string& i_targetId)
  {
    auto it = i_targetId.find("::");
    if (it != std::string::npos)
    {
      return i_targetId.substr(0, it);
    }
    return "";
  }

  module::TargetType* ModuleBuilder::GetTargetTypeByName(const std::string& i_name)
  {
    auto moduleName = this->GetModuleFromTargetId(i_name);
    assert(!moduleName.empty());
    auto module = this->m_graph.FindNode<module::Module>([moduleName](const module::Module& i_module) { return i_module.m_name == moduleName; });
    return module->GetTargetTypeByName(i_name.substr(moduleName.size() + 2));
  }

  void ModuleBuilder::ProcessNewModule(const nlohmann::json& i_json)
  {
    auto module = std::make_unique<module::Module>();
    i_json.at("name").get_to(module->m_name);
    for (const auto& json_target_type : i_json.at("target_types"))
    {
      auto targetTypeName = json_target_type.at("name");
      auto targetType = module->GetTargetTypeByName(targetTypeName);
      if (targetType)
      {
        targetType->Load(json_target_type);
      }
      else
      {
        if (targetTypeName != "base_target")
        {
          module::TargetType* baseTargetType = nullptr;
          if (module->m_name == "core")
          {
            baseTargetType = module->GetTargetTypeByName("base_target");
          }
          else
          {
            baseTargetType = this->GetTargetTypeByName("core::base_target");
          }
          assert(baseTargetType);
          module::TargetType tt = *baseTargetType;
          tt.Load(json_target_type);
          module->m_targetTypes.push_back(std::move(tt));
        }
        else
        {
          module::TargetType tt;
          tt.Load(json_target_type);
          module->m_targetTypes.push_back(std::move(tt));
        }
      }
    }

    m_graph.RegisterNode(std::move(module));
  }
}
