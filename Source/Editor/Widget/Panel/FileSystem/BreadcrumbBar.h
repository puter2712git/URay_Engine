#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class HorizontalLayout;

class BreadcrumbBar final : public Widget, public RayEmitter
{
public:
    BreadcrumbBar();

public:
    void Arrange(const Rect& rect) override;

    void SetPath(const VirtualPath& path);

protected:
    Vector2 OnMeasure() override;

private:
    void RebuildItems();
    void OnItemActivated(VirtualPath path);

private:
    VirtualPath currentPath;

    HorizontalLayout* layout = nullptr;

};

} // namespace URay
