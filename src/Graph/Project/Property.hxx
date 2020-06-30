#pragma once

#include "Graph/Node.hxx"

#include <string>

namespace ngbs::graph::module
{
  class PropertyType;
}
namespace ngbs::graph::project
{
  enum class DataType
  {
    Unknown,
    String,
    Folder,
    File,
    Bool,
    Number
  };

  class Property : public NodeBase
  {
  public:
    Property(NodeBase* i_parent, const module::PropertyType& i_type)
      : NodeBase(i_parent)
      , m_type(i_type)
    {
    }

    static const NodeType Type = NodeType::Property;
    NodeType GetNodeType() override { return Type; }

    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& i_name) { m_name = i_name; }

    DataType GetDataType() const { return m_dataType; }
    void SetDataType(const DataType& i_dataType) { m_dataType = i_dataType; }

    const module::PropertyType& GetPropertyType() const { return m_type; }

    bool IsArray() const { return m_isArray; }
    void SetIsArray(bool i_value) { m_isArray = i_value; }
  private:
    std::string m_name;
    DataType m_dataType = DataType::Unknown;
    bool m_isArray = false;
    const module::PropertyType& m_type;
  };

  template <typename T>
  class ConcreteProperty : public Property
  {
  public:
    using Property::Property;

    const T& GetValue() const { return m_value; }
    void SetValue(const T& i_value) { m_value = i_value; }
  private:
    T m_value;
  };

  DataType GetPropertyTypeFromString(const std::string& i_propertyType);
  std::string GetStringFromPropertyType(DataType i_propertyType);
}
