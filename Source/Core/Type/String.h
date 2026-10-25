#pragma once
#include <Core/Type/Types.h>


#include <string>
#include <string_view>

namespace URay::UTF8
{

inline String Encode(std::u32string_view text)
{
    String result;
    result.reserve(text.size());

    for (char32_t codepoint : text)
    {
        uint32 value = static_cast<uint32>(codepoint);

        const bool isSurrogate = value >= 0xD800 && value <= 0xDFFF;

        if (value > 0x10FFFF || isSurrogate)
        {
            value = 0xFFFD;
        }

        if (value <= 0x7F)
        {
            result.push_back(static_cast<char>(value));
        }
        else if (value <= 0x7FF)
        {
            result.push_back(static_cast<char>(0xC0 | (value >> 6)));
            result.push_back(static_cast<char>(0x80 | (value & 0x3F)));
        }
        else if (value <= 0xFFFF)
        {
            result.push_back(static_cast<char>(0xE0 | (value >> 12)));
            result.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (value & 0x3F)));
        }
        else
        {
            result.push_back(static_cast<char>(0xF0 | (value >> 18)));
            result.push_back(static_cast<char>(0x80 | ((value >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (value & 0x3F)));
        }
    }

    return result;
}

inline std::u32string Decode(StringView text)
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
