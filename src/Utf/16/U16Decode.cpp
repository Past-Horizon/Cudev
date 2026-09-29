#include <Cudev/Utf/16/U16Codec.h>

namespace Cudev::Utf16 {

namespace {

struct DecodedCodeUnit
{
    char32_t value;
    std::size_t width;
};

bool IsHighSurrogate(char16_t codeUnit) noexcept
{
    return codeUnit >= 0xD800 && codeUnit <= 0xDBFF;
}

bool IsLowSurrogate(char16_t codeUnit) noexcept
{
    return codeUnit >= 0xDC00 && codeUnit <= 0xDFFF;
}

Result<DecodedCodeUnit, CodecError> DecodeNext(
    std::u16string_view input,
    std::size_t offset)
{
    const auto codeUnit = input[offset];
    if (IsLowSurrogate(codeUnit))
    {
        return Result<DecodedCodeUnit, CodecError>::failure(
            CodecError::SurrogateCodePoint);
    }

    if (!IsHighSurrogate(codeUnit))
    {
        return Result<DecodedCodeUnit, CodecError>::success({codeUnit, 1});
    }

    if (offset + 1 == input.size())
    {
        return Result<DecodedCodeUnit, CodecError>::failure(
            CodecError::TruncatedSequence);
    }

    const auto lowSurrogate = input[offset + 1];
    if (!IsLowSurrogate(lowSurrogate))
    {
        return Result<DecodedCodeUnit, CodecError>::failure(
            CodecError::SurrogateCodePoint);
    }

    const auto codePoint = 0x10000
        + ((static_cast<char32_t>(codeUnit) - 0xD800) << 10)
        + (static_cast<char32_t>(lowSurrogate) - 0xDC00);
    return Result<DecodedCodeUnit, CodecError>::success({codePoint, 2});
}

}

Result<std::u32string, CodecError> U16Codec::Decode(std::u16string_view input) const
{
    std::u32string codePoints;
    codePoints.reserve(input.size());

    std::size_t offset = 0;
    while (offset < input.size())
    {
        const auto decoded = DecodeNext(input, offset);
        if (decoded.failed())
        {
            return Result<std::u32string, CodecError>::failure(
                *decoded.error());
        }

        codePoints.push_back(decoded.value().value);
        offset += decoded.value().width;
    }

    return Result<std::u32string, CodecError>::success(std::move(codePoints));
}

Result<void, CodecError> U16Codec::Validate(std::u16string_view input) const
{
    std::size_t offset = 0;
    while (offset < input.size())
    {
        const auto decoded = DecodeNext(input, offset);
        if (decoded.failed())
        {
            return Result<void, CodecError>::failure(*decoded.error());
        }

        offset += decoded.value().width;
    }

    return Result<void, CodecError>::success();
}

}