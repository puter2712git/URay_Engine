#pragma once

#include <yaml-cpp/yaml.h>

namespace URay
{

class Widget;

class VirtualPath;
class VirtualFilesystem;

class EditorLayout
{
public:
    EditorLayout();

public:
    YAML::Node SaveLayout(const Widget& root);
    void LoadLayout(Widget& root, const YAML::Node& layout);

private:
    void CollectSplitters(const Widget& root, YAML::Node& splitters);
    void ApplySplitters(Widget& root, const YAML::Node& layout);
};

} // namespace URay
