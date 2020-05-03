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
  graphBuilder.Process(nlohmann::json::parse(std::ifstream(scriptFile)));
  Graph g = graphBuilder.GetResult();

  return EXIT_SUCCESS;
}
