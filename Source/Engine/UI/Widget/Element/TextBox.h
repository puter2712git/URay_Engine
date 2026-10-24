#pragma once

#include <Core/Math/Color.h>
#include <Core/Math/Padding.h>
#include <Engine/UI/Widget/Widget.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

#include <string>

namespace URay
{

class TextBox : public Widget
{
public:
    explicit TextBox(const std::string& text = {});

public:
    EventReply OnPointerEnter() override;
    EventReply OnPointerLeave() override;
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnTextInput(const TextInputEvent& event) override;
    EventReply OnKeyDown(const KeyEvent& event) override;

    Rect GetContentRect() const override;

    const std::string& GetText() const { return text; }
    void SetText(const std::string& text) { this->text = text; }

    void SetTextStyle(const Render::TextStyle& style) { textStyle = style; }
    void SetOutlineColor(const Color& color) { outlineColor = color; }
    void SetOutlineWidth(float width) { outlineWidth = width; }
    void SetNormalColor(const Color& color) { normalColor = color; }
    void SetHoverColor(const Color& color) { hoverColor = color; }
    void SetFocusedColor(const Color& color) { focusedColor = color; }
    void SetSelectionColor(const Color& color) { selectionColor = color; }
    void SetCaretColor(const Color& color) { caretColor = color; }

    bool IsEditing() const { return isEditing; }
    bool IsTextInputActive() const override { return isEditing; }

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

    void ApplyTextClick(const Vector2& position, uint8 clickCount);

    void SelectAll();

private:
    size_t GetSelectionStart() const;
    size_t GetSelectionEnd() const;
    bool HasSelection() const;

    void ClearSelection();
    void DeleteSelection();

    float GetPrefixWidth(size_t characterCount) const;
    size_t GetCaretIndexAtX(float contentX) const;

    void SelectWordAt(size_t caretPosition);

private:
    std::string text;

    std::u32string editingText;
    std::string originalText;
    bool isEditing = false;
    bool isReadonly = false;

    Padding padding = Padding(6.0f, 4.0f);
    Render::TextStyle textStyle = {};

    Color normalColor = {};
    Color hoverColor = {};
    Color focusedColor = {};
    Color outlineColor = {};
    Color selectionColor = {};
    Color caretColor = {};
    float outlineWidth = 1.0f;

    size_t caretIndex = 0;
    size_t selectionAnchor = 0;
};

} // namespace URay
