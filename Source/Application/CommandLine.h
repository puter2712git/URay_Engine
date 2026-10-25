#pragma once
#include <Core/Type/Types.h>

#include <optional>
#include <vector>

namespace URay
{

class CommandLine
{
public:
    CommandLine(int argc, char** argv);
    ~CommandLine();

public:
    bool Has(const String& option) const;

    std::optional<String> Get(const String& option) const;

private:
    std::vector<String> arguments;
};

} // namespace URay
