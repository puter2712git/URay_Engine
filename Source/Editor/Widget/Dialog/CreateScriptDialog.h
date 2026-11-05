#pragma once

#include <Editor/Widget/Dialog/Dialog.h>

namespace URay
{

class StringField;
class Surface;
class VerticalLayout;

class CreateScriptDialog final : public Dialog
{
public:
    CreateScriptDialog();

public:
    void Arrange(const Rect& rect) override;

    const String& GetScriptName() const;

protected:
    Vector2 OnMeasure() override;

private:
    Surface* background = nullptr;
    VerticalLayout* layout = nullptr;
    StringField* nameField = nullptr;
};

} // namespace URay
