#pragma once

#include "Core/Type/Types.h"

#include <string>
#include <string_view>

namespace URay::UTF8
{

inline std::u32string Decode(std::string_view text)
{
    std::u32string result;
    result.reserve(text.size());

    size_t index = 0;

    while (index < text.size())
    {
        const uint8 first = static_cast<uint8>(static_cast<unsigned char>(text[index]));

        if (first <= 0x7F)
        {
            result.push_back(static_cast<char32_t>(first));
            ++index;
            continue;
        }

        uint32 codepoint = 0;
        uint32 continuationCount = 0;
        uint32 minCodepoint = 0;

        if (first >= 0xC2 && first <= 0xDF)
        {
            codepoint = first & 0x1F;
            continuationCount = 1;
            minCodepoint = 0x80;
        }
        else if (first >= 0xE0 && first <= 0xEF)
        {
            codepoint = first & 0x0F;
            continuationCount = 2;
            minCodepoint = 0x800;
        }
        else if (first >= 0xF0 && first <= 0xF4)
        {
            codepoint = first & 0x07;
            continuationCount = 3;
            minCodepoint = 0x10000;
        }
        else
        {
            result.push_back(U'\uFFFD');
            ++index;
            continue;
        }

        if (index + continuationCount >= text.size())
        {
            result.push_back(U'\uFFFD');
            ++index;
            continue;
        }

        bool valid = true;

        for (uint32 i = 1; i <= continuationCount; ++i)
        {
            const uint8 byte = static_cast<uint8>(static_cast<unsigned char>(text[index + i]));

            if ((byte & 0xC0) != 0x80)
            {
                valid = false;
                break;
            }

            codepoint = (codepoint << 6) | (byte & 0x3F);
        }

        const bool isSurrogate = codepoint >= 0xD800 && codepoint <= 0xDFFF;

        if (!valid || codepoint < minCodepoint || codepoint > 0x10FFFF || isSurrogate)
        {
            result.push_back(U'\uFFFD');
            ++index;
            continue;
        }

        result.push_back(static_cast<char32_t>(codepoint));
        index += continuationCount + 1;
    }

    return result;
}

} // namespace URay::UTF8
