add_executable(NGBS
  NGBS.cxx
  CMakeLists.txt
)

add_subdirectory(Graph)

find_package(nlohmann_json 3.7.3 REQUIRED)

target_link_libraries(NGBS
  PRIVATE
    nlohmann_json::nlohmann_json
)

target_include_directories(NGBS
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/thirdparty/ChaiScript/include
)

file(GLOB_RECURSE sources *.cxx *.hxx *.txt *.cmake)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources} PREFIX Sources)

