#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/File/VirtualPath.h"

namespace URay
{

class Label;

class DirectoryEntryWidget final : public Widget
{
public:
    DirectoryEntryWidget(const VirtualPath& path);

public:
    void Arrange(const Rect& rect) override;

private:
    VirtualPath path;
};

} // namespace URay
