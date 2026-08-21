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
    EditorLayout(VirtualFilesystem& filesystem);

public:
    void SaveLayout(const Widget& root);
    void LoadLayout(Widget& root);

private:
    void CollectSplitters(const Widget& root, YAML::Node& splitters);
    void ApplySplitters(Widget& root, const YAML::Node& layout);

private:
    VirtualFilesystem& filesystem;
};

} // namespace URay
