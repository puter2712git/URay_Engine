#pragma once

#include <Core/Type/Ray/Ray.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

enum class DialogResult
{
    None,
    Accepted,
    Cancelled
};

class Dialog : public Widget
{
public:
    virtual void Accept();
    virtual void Cancel();

    DialogResult GetResult() const { return result; }

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }

    virtual bool CanAccept() const { return true; }

public:
    Ray<DialogResult> onClosed;

private:
    DialogResult result = DialogResult::None;
};

} // namespace URay
