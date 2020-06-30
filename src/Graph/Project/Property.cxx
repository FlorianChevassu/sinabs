#include "Property.hxx"

namespace ngbs::graph::project
{
  DataType GetPropertyTypeFromString(const std::string& i_propertyType)
  {
    if (i_propertyType == "string")
    {
      return DataType::String;
    }
    if (i_propertyType == "folder")
    {
      return DataType::Folder;
    }
    if (i_propertyType == "file")
    {
      return DataType::File;
    }
    if (i_propertyType == "bool")
    {
      return DataType::Bool;
    }
    if (i_propertyType == "number")
    {
      return DataType::Number;
    }

    return DataType::Unknown;
  }

  std::string GetStringFromPropertyType(DataType i_propertyType)
  {
    switch (i_propertyType)
    {
    case DataType::String: return "string";
    case DataType::Folder: return "folder";
    case DataType::File: return "file";
    case DataType::Bool: return "bool";
    case DataType::Number: return "number";
    case DataType::Unknown: return "unknown";
    }
  }
}
