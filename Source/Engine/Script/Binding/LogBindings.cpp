#include "LogBindings.h"

#include <Core/Log/LogSystem.h>

#include <string>

namespace URay::ScriptBindings
{

bool Log(ScriptSystem&, std::string_view message)
{
    URAY_LOG("%s", std::string(message).c_str());
    return true;
}

} // namespace URay::ScriptBindings
