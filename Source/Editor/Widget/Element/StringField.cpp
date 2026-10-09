#include "StringField.h"

#include <Core/Type/String.h>

namespace URay
{

EventReply StringField::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    BeginEdit();

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
}

EventReply StringField::OnTextInput(const TextInputEvent& event)
{
    if (!isEditing)
        return {};

    editingText.push_back(event.codepoint);
    RefreshText();

    return EventReply{
        .handled = true
    };
}

EventReply StringField::OnKeyDown(const KeyEvent& event)
{
    if (!isEditing)
        return {};

    if (event.key == KeyCode::Backspace && !editingText.empty())
    {
        editingText.pop_back();
        RefreshText();

        return EventReply{
            .handled = true
        };
    }

    if (event.key == KeyCode::Enter)
    {
        CommitEdit();
        return EventReply{
            .handled = true
        };
    }

    if (event.key == KeyCode::Escape)
    {
        CancelEdit();
        return EventReply{
            .handled = true
        };
    }

    return {};
}

void StringField::BeginEdit()
{
    if (isEditing)
        return;

    isEditing = true;
    originalText = GetText();
    editingText = UTF8::Decode(originalText);
}

void StringField::CommitEdit()
{
    if (!isEditing)
        return;

    isEditing = false;

    const std::string committedText = UTF8::Encode(editingText);
    if (committedText == originalText)
        return;

    SetText(committedText);
    valueChangedRay.Emit();
}

void StringField::CancelEdit()
{
    if (!isEditing)
        return;

    isEditing = false;
    SetText(originalText);
}

void StringField::RefreshText()
{
    SetText(UTF8::Encode(editingText));
}

} // namespace URay
