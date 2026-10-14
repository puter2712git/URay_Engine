#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>

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
    virtual bool CanAccept() const { return true; }

private:
    DialogResult result = DialogResult::None;
    EventRay<DialogResult> closedRay;
};

} // namespace URay
