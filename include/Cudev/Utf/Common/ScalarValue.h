/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <cstdint>

namespace Cudev::Utf {

/*
    @summary
    Tests whether a code point is a Unicode scalar value.

    @param codePoint
    The Unicode code point to test.

    @returns
    True when codePoint is valid and not a surrogate.
*/
constexpr bool IsScalarValue(char32_t codePoint) noexcept
{
    return codePoint <= 0x10FFFF && !(codePoint >= 0xD800 && codePoint <= 0xDFFF);
}

}