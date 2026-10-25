#include "CommandLine.h"
#include <Core/Type/Types.h>

namespace URay
{

CommandLine::CommandLine(int argc, char** argv)
{
    arguments.reserve(argc > 1 ? argc - 1 : 0);

    for (int i = 1; i < argc; ++i)
    {
        arguments.emplace_back(argv[i]);
    }
}

CommandLine::~CommandLine() = default;

bool CommandLine::Has(const String& option) const
{
    for (const String& arg : arguments)
    {
        if (arg == option)
            return true;
    }

    return false;
}

std::optional<String> CommandLine::Get(const String& option) const
{
    size_t argumentCount = arguments.size();

    for (size_t i = 0; i < argumentCount; ++i)
    {
        if (arguments[i] != option)
            continue;

        if (i + 1 >= arguments.size())
            return std::nullopt;

        return arguments[i + 1];
    }

    return std::nullopt;
}

} // namespace URay
