#pragma once

#include <chaiscript/chaiscript.hpp>

namespace ngbs::graph::module
{
  class ModuleItem
  {
  public:
    ModuleItem(chaiscript::ChaiScript& i_scriptEngine)
      : m_scriptEngine(i_scriptEngine)
    {
    }

    chaiscript::ChaiScript& GetScriptEngine() { return m_scriptEngine; }

  private:
    chaiscript::ChaiScript& m_scriptEngine;
  };


  namespace utils
  {
    template <typename T, std::string T::* attribute, typename C>
    T* GetObjectByName(C& i_collection, const std::string& i_name)
    {
      auto it = std::find_if(std::begin(i_collection), std::end(i_collection),
        [&i_name](const T& i_item) {
          return i_item.*attribute == i_name;
        }
      );
      if (it == std::end(i_collection))
      {
        return nullptr;
      }
      return &*it;
    }
  }
}
