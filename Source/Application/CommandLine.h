#pragma once

#include <optional>
#include <string>
#include <vector>

namespace URay
{

class CommandLine
{
public:
    CommandLine(int argc, char** argv);
    ~CommandLine();

public:
    bool Has(const std::string& option) const;

    std::optional<std::string> Get(const std::string& option) const;

private:
    std::vector<std::string> arguments;
};

} // namespace URay
