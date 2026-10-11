#pragma once

#include <Editor/Widget/Widget.h>

#include <memory>

namespace URay
{

class PopupLayer;

class RootWidget final : public Widget
{
public:
    explicit RootWidget(std::unique_ptr<Widget> mainContent);

public:
    void Arrange(const Rect& rect) override;

    PopupLayer& GetPopupLayer() const { return *popupLayer; }

private:
    Widget* mainContent = nullptr;
    PopupLayer* popupLayer = nullptr;
};

} // namespace URay
