#include <Cudev/Utf/16/U16Codec.h>

#include <Cudev/Utf/Common/ScalarValue.h>

namespace Cudev::Utf16 {

namespace {

bool IsHighSurrogate(char16_t codeUnit) noexcept
{
    return codeUnit >= 0xD800 && codeUnit <= 0xDBFF;
}

bool IsLowSurrogate(char16_t codeUnit) noexcept
{
    return codeUnit >= 0xDC00 && codeUnit <= 0xDFFF;
}

}

Result<std::u32string, CodecError> U16Codec::Decode(std::u16string_view input) const
{
    std::u32string codePoints;
    codePoints.reserve(input.size());

    std::size_t offset = 0;
    while (offset < input.size())
    {
        const auto codeUnit = input[offset++];
        if (IsLowSurrogate(codeUnit))
        {
            return Result<std::u32string, CodecError>::failure(
                CodecError::SurrogateCodePoint);
        }

        if (!IsHighSurrogate(codeUnit))
        {
            codePoints.push_back(static_cast<char32_t>(codeUnit));
            continue;
        }

        if (offset == input.size())
        {
            return Result<std::u32string, CodecError>::failure(
                CodecError::TruncatedSequence);
        }

        const auto lowSurrogate = input[offset];
        if (!IsLowSurrogate(lowSurrogate))
        {
            return Result<std::u32string, CodecError>::failure(
                CodecError::SurrogateCodePoint);
        }

        ++offset;
        const auto codePoint = 0x10000
            + ((static_cast<char32_t>(codeUnit) - 0xD800) << 10)
            + (static_cast<char32_t>(lowSurrogate) - 0xDC00);
        codePoints.push_back(codePoint);
    }

    return Result<std::u32string, CodecError>::success(std::move(codePoints));
}

Result<void, CodecError> U16Codec::Validate(std::u16string_view input) const
{
    std::size_t offset = 0;
    while (offset < input.size())
    {
        const auto codeUnit = input[offset++];
        if (IsLowSurrogate(codeUnit))
        {
            return Result<void, CodecError>::failure(CodecError::SurrogateCodePoint);
        }

        if (!IsHighSurrogate(codeUnit))
        {
            continue;
        }

        if (offset == input.size())
        {
            return Result<void, CodecError>::failure(CodecError::TruncatedSequence);
        }

        if (!IsLowSurrogate(input[offset]))
        {
            return Result<void, CodecError>::failure(CodecError::SurrogateCodePoint);
        }

        ++offset;
    }

    return Result<void, CodecError>::success();
}

}