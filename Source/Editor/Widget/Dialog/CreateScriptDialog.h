#pragma once

#include <Editor/Widget/Dialog/Dialog.h>
#include <functional>

namespace URay
{

class StringField;
class Label;
class Surface;
class VerticalLayout;

class CreateScriptDialog final : public Dialog
{
public:
    CreateScriptDialog();

public:
    void Arrange(const Rect& rect) override;
    void Accept() override;
    void SetCreateHandler(std::function<String(const String&)> handler);

    const String& GetScriptName() const;

protected:
    Vector2 OnMeasure() override;

private:
    Surface* background = nullptr;
    VerticalLayout* layout = nullptr;
    StringField* nameField = nullptr;
    Label* validationLabel = nullptr;
    std::function<String(const String&)> createHandler;
};

} // namespace URay
