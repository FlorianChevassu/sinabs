# NGBS - The Next Generation Build System

## NGBS goals:
- Simple to use
- High level
- Configurable
- Fast
- Easy to use correctly, hard to use incorrectly
- Generic: Can be usedconfigured to do whatever you want. Not tied to C++ at all !

## CARACTERISTICS 
### Frontend
- JSON based language
- Each folder should have a build.ngbs file, defining the steps for the current folder
- Sub folders are automatically added if they have a build.ngbs file
- Core objects:
  - Target: Represents a "step" of the whole process.
  - Property: Represents a generic property of a target.
    - For example, PositionIndependentCode is a boolean property on a target of type executable or shared_library.
    - The property can be public, private or interface. (TODO: think of another API ?)

- Types of targets defined in a "language configuration"
  - Default C++ language provides static_library, shared_library, executable, object, etc.
  - A target just represents a "step" of the process.
  - Each target defines its property set.
    - For example, PositionIndependentCode is only available on the executable and shared_library targets.
 
- A target can have "implicit sub-targets". For example, an executable with sources will create object targets for all the source files

- The generation process should
  - be debugable (?)
  - generate clear error messages

### Backend
- Ninja
- CMake (C++ only) ?


# DETAILS
- The core language have only the concept of "target", and dependencies between them.
- Core library defines a set of specific types of target: source, static_library, shared_library, executable for example
- Each language plugin define the steps needed to "process" a target: for example,a C++ source file is compiled usin "gcc -c FILE, and a shared_library using ld.
- Language plugins can also create new types of target, like jar_library for the Java plugin for example
- Each target type should know how to handle dependent targets. For example static_library compiles source files, and shared_library links static_library targets.
- Each dependency has a scope, like PUBLIC INTERFACE and PRIVATE in CMake
- Each target can either be generated or be "static", i.e. point to a file (like sources, or CMake IMPORTED targets)
- Dependencies between targets are of two kinds: high level (hard coded in the build system language, like libA links with libB) or low level (like source file a.cpp includes b.hpp). Low level dependencies are NOT directly managed by the build system, but provided by the language plugins. For example, C++ plugin will use gcc or msvc to extract deps, Fortran plugin might implement its own solution, etc.
- Targets executing some commands might do some PRE or POST operations. These operations will be specific targets that are always executed.
- Targets are groupes by directories. ??