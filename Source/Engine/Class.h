#pragma once

#include "Engine/Property/Property.h"

#include <string>
#include <vector>

namespace URay
{

class Class
{
public:
    Class(const std::string& name);

public:
void AddProperty()

private:
    std::string name;
    std::vector<Property> properties;
};

} // namespace URay
