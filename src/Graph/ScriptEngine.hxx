#pragma once

#include <chaiscript/chaiscript.hpp>

namespace ngbs
{
  namespace graph
  {
    class Graph;
    namespace project
    {
      class Target;
    }
  }

  class ScriptEngine
  {
  public:
    ScriptEngine();
    void SetGraph(graph::Graph& i_graph) { m_graph = &i_graph; }
    void AddTarget(graph::project::Target& i_target);

    template <class T>
    void AddVariable(const std::string& i_name, T& i_value)
    {
      m_engine.add(chaiscript::var(i_value), i_name);
    }

    std::string Evaluate(const std::string& i_script);

  protected:
    void AddModelClasses();

    graph::Graph& GetGraph()
    {
      if (m_graph == nullptr)
      {
        assert(false);
        //throw;
      }
      return *m_graph;
    }

  private:
    chaiscript::ChaiScript m_engine;
    graph::Graph* m_graph = nullptr;
  };
}
