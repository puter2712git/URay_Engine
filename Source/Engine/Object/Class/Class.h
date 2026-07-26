#pragma once

#include "Engine/Object/Property/Property.h"

#include <string>
#include <vector>

namespace URay
{

class Class
{
public:
    Class(const std::string& name);

public:
    void AddProperty(Property prop) { properties.push_back(prop); }
    const std::vector<Property>& GetProperties() const { return properties; }

    const std::string& GetName() const { return name; }

private:
    std::string name;
    std::vector<Property> properties;
};

} // namespace URay
