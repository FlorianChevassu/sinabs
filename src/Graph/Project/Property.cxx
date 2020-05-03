#include "Property.hxx"

PropertyType GetPropertyTypeFromString(const std::string& i_propertyType)
{
  if (i_propertyType == "string")
  {
    return PropertyType::String;
  }
  if (i_propertyType == "folder")
  {
    return PropertyType::Folder;
  }
  if (i_propertyType == "file")
  {
    return PropertyType::File;
  }
  if (i_propertyType == "bool")
  {
    return PropertyType::Bool;
  }
  if (i_propertyType == "number")
  {
    return PropertyType::Number;
  }

  return PropertyType::Unknown;
}
