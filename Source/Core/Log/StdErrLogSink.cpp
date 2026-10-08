#include "StdErrLogSink.h"

#include <iostream>

namespace URay
{

StdErrLogSink::StdErrLogSink() = default;

StdErrLogSink::~StdErrLogSink() = default;

void StdErrLogSink::Write(std::string_view msg)
{
    std::cerr << msg << '\n';
}

} // namespace URay
