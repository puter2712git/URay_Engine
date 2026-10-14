#include "Dialog.h"

namespace URay
{

void Dialog::Accept()
{
    if (result != DialogResult::None || !CanAccept())
        return;

    result = DialogResult::Accepted;
    closedRay.Emit(result);
}

void Dialog::Cancel()
{
    if (result != DialogResult::None)
        return;

    result = DialogResult::Cancelled;
    closedRay.Emit(result);
}

} // namespace URay
