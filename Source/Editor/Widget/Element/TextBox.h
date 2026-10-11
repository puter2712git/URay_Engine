#pragma once

#include <Editor/Widget/Style/EditorStyles.h>
#include <Editor/Widget/Widget.h>

#include <Core/Math/Padding.h>

#include <string>

namespace URay
{

class TextBox : public Widget
{
public:
    explicit TextBox(const std::string& text = {});

public:
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnTextInput(const TextInputEvent& event) override;
    EventReply OnKeyDown(const KeyEvent& event) override;

    Rect GetContentRect() const override;

    const std::string& GetText() const { return text; }
    void SetText(const std::string& text) { this->text = text; }

    void SetOutlineColor(const Color& color) { outlineColor = color; }
    void SetOutlineWidth(float width) { outlineWidth = width; }

    bool IsEditing() const { return isEditing; }

    bool IsReadonly() const { return isReadonly; }
    void SetReadonly(bool value) { isReadonly = value; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;
    void OnPaintForeground(Render::UIDrawContext& context) override;

    virtual bool TryCommitText(const std::string& editingText, std::string& committedText)
    {
        committedText = editingText;
        return true;
    }
    virtual void OnTextChanged(const std::string& prevText, const std::string& currText) {}
    virtual void OnTextCommitted(const std::string& prevText, const std::string& committedText) {}

    void BeginEdit();
    void CommitEdit();
    void CancelEdit();

    void RefreshText();

private:
    std::string text;

    std::u32string editingText;
    std::string originalText;
    bool isEditing = false;
    bool isReadonly = false;

    Padding padding = Padding(6.0f, 4.0f);
    Render::TextStyle textStyle = EditorStyle::SmallText();

    Color outlineColor = EditorStyle::Border;
    float outlineWidth = 1.0f;
};

} // namespace URay
