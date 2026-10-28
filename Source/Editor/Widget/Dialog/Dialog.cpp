#include "Dialog.h"

namespace URay
{

Dialog::Dialog()
{
    URAY_DECLARE_RAY(Closed, DialogResult);
}

void Dialog::Accept()
{
    if (result != DialogResult::None || !CanAccept())
        return;

    result = DialogResult::Accepted;
    Emit("Closed", result);
}

void Dialog::Cancel()
{
    if (result != DialogResult::None)
        return;

    result = DialogResult::Cancelled;
    Emit("Closed", result);
}

} // namespace URay
