#pragma once

#include "Graph/Node.hxx"

#include <string>

template <typename T>
class Property: public NodeBase
{
public:
  NodeType GetNodeType() override { return NodeType::Property; }

  const std::string& GetName() const { return m_name; }
  void SetName(const std::string& i_name) { m_name = i_name; }

  const T& GetValue() const { return m_value; }
  void SetValue(const T& i_value) { m_value = i_value; }
private:
  std::string m_name;
  T m_value;
};

enum class PropertyType
{
  Unknown,
  String,
  Folder,
  File,
  Bool,
  Number
};

PropertyType GetPropertyTypeFromString(const std::string& i_propertyType);

