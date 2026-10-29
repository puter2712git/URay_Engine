#pragma once

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
    std::unique_ptr<Widget> LoadLayout(const YAML::Node& layout, UISystem& uiSystem, PanelContext& context) const;
};

} // namespace URay
