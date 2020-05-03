#include "Module.hxx"

namespace ngbs::module
{

const TargetType* Module::GetTargetTypeByName(const std::string& i_name) const
{
  auto it = std::find_if(this->m_targetTypes.begin(), this->m_targetTypes.end(),
    [&i_name](const TargetType& targetType) {
      return targetType.m_name == i_name;
    }
  );
  if (it == this->m_targetTypes.end())
  {
    return nullptr;
  }
  return &*it;
}

void from_json(const nlohmann::json& i_json, Module& i_module)
{
  i_json.at("name").get_to(i_module.m_name);
  i_json.at("target_types").get_to(i_module.m_targetTypes);
}

}
