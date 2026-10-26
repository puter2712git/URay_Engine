#include "StdErrLogSink.h"
#include <Core/Type/TypeAlias.h>

#include <iostream>

namespace URay
{

StdErrLogSink::StdErrLogSink() = default;

StdErrLogSink::~StdErrLogSink() = default;

void StdErrLogSink::Write(StringView msg)
{
    std::cerr << msg << '\n';
}

} // namespace URay
