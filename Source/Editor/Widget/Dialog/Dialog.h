#pragma once

#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

enum class DialogResult
{
    None,
    Accepted,
    Cancelled
};

URAY_ENUM_TYPE(DialogResult)

class Dialog : public Widget, public RayEmitter
{
public:
    Dialog();

    virtual void Accept();
    virtual void Cancel();

    DialogResult GetResult() const { return result; }

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }

    virtual bool CanAccept() const { return true; }

private:
    DialogResult result = DialogResult::None;
};

} // namespace URay
