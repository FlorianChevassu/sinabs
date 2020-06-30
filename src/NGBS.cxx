#include "Graph/Graph.hxx"
#include "Graph/GraphBuilder.hxx"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>

/*
   Comments in *.ngbs files is not possible right now.
   It may be available in a new version of nlohmann::json.
   See https://github.com/nlohmann/json/issues/1813 and https://github.com/nlohmann/json/issues/1950
*/

using namespace ngbs::graph;

#include <boost/process.hpp>
#include <boost/asio.hpp>

void Build(Graph& i_graph, const std::filesystem::path& i_sourceDir, const std::filesystem::path& i_buildDir);

int main()
{
  auto path = std::filesystem::current_path();
  auto scriptFile = path / "build.ngbs";
  if (!std::filesystem::exists(scriptFile))
  {
    std::cerr << scriptFile << " does not exist !";
    return EXIT_FAILURE;
  }

  GraphBuilder graphBuilder;
  // Source directory will be used as a base for relative file paths
  graphBuilder.SetSourceDirectory(path);
  // Build directory is where all the commands will be generated.
  graphBuilder.SetBuildDirectory(path);
  graphBuilder.Process(nlohmann::json::parse(std::ifstream(scriptFile)));
  Graph g = graphBuilder.GetResult();

  Build(g, path, path / "build");

  //namespace bp = boost::process;

  //boost::asio::io_service ios;
  //std::vector<char> buf(4096);

  //bp::child c(bp::cmd = "g++ main.cpp", bp::std_err> boost::asio::buffer(buf), ios);

  //c.wait(); //wait for the process to exit   
  //int result = c.exit_code();

  // Create a table containing all files (sources, artifacts, etc). For each file, list
  //   - the command that generated it, if any
  //   - its dependent files (the artifacts of its dependencies, or the dependencies themselves if they are files)
  //   - its corresponding target
  //   - its modification date
  //
  // An item is dirty (and its corresponding target) if
  //   - the command changed,
  //   - a dependent file is dirty,
  //   - its modification date is older that that of a dependent file.







  // Now, compare this graph to the one used at the previous generation.
  // If a target differ, mark it as dirty.

  // Finally, if any of the inputs has changed, mark them as dirty.

  // We now have the final graph, and should be able to find which commands should be run, and in which order.
  // But before that, we should compare this graph with the graph that was used with the previous run. This
  // will allow us to flag the targets that are dirty.

  return EXIT_SUCCESS;
}

#include <iostream> // std::cout
#include <utility> // std::pair
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include "Graph/ScriptEngine.hxx"

using namespace boost;

using BGLEdge = std::pair<int, int>;

using BGLGraph = adjacency_list<vecS, vecS, bidirectionalS,
  property<vertex_color_t, default_color_type>
>;

using BGLVertex = graph_traits<BGLGraph>::vertex_descriptor;

int GetNodeIndex(std::vector<NodeBase*>& i_nodeByIndex, NodeBase* i_node)
{
  auto it = std::find(i_nodeByIndex.begin(), i_nodeByIndex.end(), i_node);
  if (it != i_nodeByIndex.end())
  {
    return std::distance(i_nodeByIndex.begin(), it);
  }
  else
  {
    i_nodeByIndex.push_back(i_node);
    return i_nodeByIndex.size() - 1;
  }
}

void CreateEdges(std::vector<NodeBase*>& i_nodeByIndex, std::vector<BGLEdge>& i_edges, NodeBase* i_node)
{
  int nodeIndex = GetNodeIndex(i_nodeByIndex, i_node);
  for (auto dep : i_node->GetDependencies())
  {
    if (dep->GetNodeType() != NodeType::Target)
    {
      continue;
    }
    int depIndex = GetNodeIndex(i_nodeByIndex, dep);
    i_edges.emplace_back(nodeIndex, depIndex);
    CreateEdges(i_nodeByIndex, i_edges, dep);
  }
}

void Build(Graph& i_graph, const std::filesystem::path& i_sourceDir, const std::filesystem::path& i_buildDir)
{
  std::vector<BGLEdge> edges;
  auto& projectNodes = i_graph.GetNodesByType<project::Project>();
  
  std::vector<NodeBase*> nodeByIndex;
  for (auto& projectNode : projectNodes)
  {
    CreateEdges(nodeByIndex, edges, projectNode);
  }

  BGLGraph g(edges.begin(), edges.end(), edges.size());


  //write_graphviz(std::cout, g);

  auto nodes = i_graph.GetNodes();

  std::vector<BGLVertex> make_order;
  boost::topological_sort(g, std::back_inserter(make_order));

  std::cout << "make ordering: ";
  for (auto i = make_order.begin(); i != make_order.end(); ++i)
  {
    auto node = nodeByIndex[*i];
    auto target = dynamic_cast<ngbs::graph::project::Target* > (node);
    if (target)
    {
      const auto& targetType = target->GetType();

      auto ruleGeneratorScript = targetType.m_ruleGenerator;

      if (ruleGeneratorScript.empty())
      {
        continue;
      }

      ngbs::ScriptEngine engine;
      engine.SetGraph(i_graph);
      engine.AddTarget(*target);
      engine.AddVariable("SourceDirectory", i_sourceDir.string());
      engine.AddVariable("BuildDirectory", i_buildDir.string());

      std::string rule;
      try
      {
        rule = engine.Evaluate(ruleGeneratorScript);
      }
      catch (std::exception& e)
      {
        int k = 2;
        std::cerr << e.what() << std::endl;
      }

      std::cout << rule << std::endl;
    }
  }
  std::cout << std::endl;
}