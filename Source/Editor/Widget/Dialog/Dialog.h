#pragma once

#include <Core/Ray/EventRay.h>
#include <Engine/UI/Widget.h>

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
    EventRay<DialogResult>& GetClosedRay() { return closedRay; }

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }

    virtual bool CanAccept() const { return true; }

private:
    DialogResult result = DialogResult::None;
    EventRay<DialogResult> closedRay;
};

} // namespace URay
