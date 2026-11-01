#pragma once

#include <Core/Type/TypeAlias.h>
#include <yaml-cpp/yaml.h>

#include <memory>

namespace URay
{

class UISystem;
class Widget;
struct PanelContext;

class EditorLayout
{
public:
    bool SaveLayout(const Widget& root, YAML::Node& outLayout) const;
    UniquePtr<Widget> LoadLayout(const YAML::Node& layout, UISystem& uiSystem, PanelContext& context) const;
};

} // namespace URay
