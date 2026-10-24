#include "UUID.h"

#include <charconv>
#include <format>
#include <random>

namespace URay
{

UUID UUID::Generate()
{
    static std::random_device rd;
    static std::mt19937_64 generator(rd());

    UUID ret = {};

    ret.high = generator();
    ret.low = generator();

    ret.high &= 0xFFFFFFFFFFFF0FFFULL;
    ret.high |= 0x0000000000004000ULL;

    ret.low &= 0x3FFFFFFFFFFFFFFFULL;
    ret.low |= 0x8000000000000000ULL;

    return ret;
}

std::string UUID::ToString() const
{
    return std::format(
        "{:08x}-{:04x}-{:04x}-{:04x}-{:012x}",
        (high >> 32) & 0xFFFFFFFFULL,
        (high >> 16) & 0xFFFFULL,
        high & 0xFFFFULL,
        (low >> 48) & 0xFFFFULL,
        low & 0xFFFFFFFFFFFFULL);
}

UUID UUID::FromString(const std::string& str)
{
    if (str.size() != 36)
        return {};

    if (str[8] != '-' ||
        str[13] != '-' ||
        str[18] != '-' ||
        str[23] != '-')
    {
        return {};
    }

    char hex[32];

    size_t j = 0;

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '-')
            continue;

        hex[j++] = str[i];
    }

    UUID ret;

    auto highResult = std::from_chars(
        hex,
        hex + 16,
        ret.high,
        16);

    auto lowResult = std::from_chars(
        hex + 16,
        hex + 32,
        ret.low,
        16);

    if (highResult.ec != std::errc{} ||
        lowResult.ec != std::errc{})
    {
        return {};
    }

    return ret;
}

} // namespace URay
