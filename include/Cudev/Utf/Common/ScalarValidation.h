/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <Cudev/Core/CodecError.h>

#include <optional>

namespace Cudev::Utf {

/*
    @summary
    Validates a Unicode scalar value.

    @param codePoint
    The Unicode code point to validate.

    @returns
    The matching codec error, or no value when codePoint is valid.
*/
constexpr std::optional<CodecError> ValidateScalarValue(char32_t codePoint) noexcept
{
    if (codePoint > 0x10FFFF)
    {
        return CodecError::CodePointOutOfRange;
    }

    if (codePoint >= 0xD800 && codePoint <= 0xDFFF)
    {
        return CodecError::SurrogateCodePoint;
    }

    return std::nullopt;
}

}