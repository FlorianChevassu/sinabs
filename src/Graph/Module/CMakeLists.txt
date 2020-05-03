target_sources(NGBS
  PRIVATE
    CMakeLists.txt
    Arity.hxx
    Arity.cxx
    InlineTarget.hxx
    InlineTarget.cxx
    Module.hxx
    Module.cxx
    ModuleItem.hxx
    PropertyType.hxx
    PropertyType.cxx
    TargetType.hxx
    TargetType.cxx
)

target_compile_options(NGBS PRIVATE  /bigobj)

set_source_files_properties(Module.cxx PROPERTIES COMPILE_OPTIONS /bigobj)


