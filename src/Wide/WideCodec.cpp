#include <Cudev/Wide/WideCodec.h>

#include <Cudev/Utf/16/U16Codec.h>
#include <Cudev/Utf/32/U32Codec.h>
#include <Cudev/Utf/8/U8Codec.h>

namespace Cudev::Wide {

namespace {

static_assert(sizeof(wchar_t) == sizeof(char16_t)
    || sizeof(wchar_t) == sizeof(char32_t));

const Utf8::U8Codec& Utf8Codec()
{
    static const Utf8::U8Codec codec;
    return codec;
}

const Utf16::U16Codec& Utf16Codec()
{
    static const Utf16::U16Codec codec;
    return codec;
}

const Utf32::U32Codec& Utf32Codec()
{
    static const Utf32::U32Codec codec;
    return codec;
}

std::u16string ToUtf16Storage(std::wstring_view input)
{
    std::u16string result;
    result.reserve(input.size());
    for (const wchar_t codeUnit : input)
    {
        result.push_back(static_cast<char16_t>(codeUnit));
    }
    return result;
}

std::u32string ToUtf32Storage(std::wstring_view input)
{
    std::u32string result;
    result.reserve(input.size());
    for (const wchar_t codePoint : input)
    {
        result.push_back(static_cast<char32_t>(codePoint));
    }
    return result;
}

Result<std::wstring, CodecError> EncodeWide(std::u32string_view input)
{
    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        const auto encoded = Utf16Codec().Encode(input);
        if (encoded.failed())
        {
            return Result<std::wstring, CodecError>::failure(*encoded.error());
        }

        std::wstring result;
        result.reserve(encoded.value().size());
        for (const char16_t codeUnit : encoded.value())
        {
            result.push_back(static_cast<wchar_t>(codeUnit));
        }
        return Result<std::wstring, CodecError>::success(std::move(result));
    }
    else
    {
        const auto encoded = Utf32Codec().Encode(input);
        if (encoded.failed())
        {
            return Result<std::wstring, CodecError>::failure(*encoded.error());
        }

        std::wstring result;
        result.reserve(encoded.value().size());
        for (const char32_t codePoint : encoded.value())
        {
            result.push_back(static_cast<wchar_t>(codePoint));
        }
        return Result<std::wstring, CodecError>::success(std::move(result));
    }
}

}

Result<std::u32string, CodecError> ToUtf32(std::wstring_view input)
{
    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        return Utf16Codec().Decode(ToUtf16Storage(input));
    }
    else
    {
        return Utf32Codec().Decode(ToUtf32Storage(input));
    }
}

Result<std::u16string, CodecError> ToUtf16(std::wstring_view input)
{
    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        auto result = ToUtf16Storage(input);
        const auto validation = Utf16Codec().Validate(result);
        if (validation.failed())
        {
            return Result<std::u16string, CodecError>::failure(*validation.error());
        }
        return Result<std::u16string, CodecError>::success(std::move(result));
    }
    else
    {
        const auto codePoints = ToUtf32(input);
        if (codePoints.failed())
        {
            return Result<std::u16string, CodecError>::failure(*codePoints.error());
        }
        return Utf16Codec().Encode(codePoints.value());
    }
}

Result<std::string, CodecError> ToUtf8(std::wstring_view input)
{
    const auto codePoints = ToUtf32(input);
    if (codePoints.failed())
    {
        return Result<std::string, CodecError>::failure(*codePoints.error());
    }
    return Utf8Codec().Encode(codePoints.value());
}

Result<std::wstring, CodecError> FromUtf8(std::string_view input)
{
    const auto codePoints = Utf8Codec().Decode(input);
    if (codePoints.failed())
    {
        return Result<std::wstring, CodecError>::failure(*codePoints.error());
    }
    return EncodeWide(codePoints.value());
}

Result<std::wstring, CodecError> FromUtf16(std::u16string_view input)
{
    const auto codePoints = Utf16Codec().Decode(input);
    if (codePoints.failed())
    {
        return Result<std::wstring, CodecError>::failure(*codePoints.error());
    }
    return EncodeWide(codePoints.value());
}

Result<std::wstring, CodecError> FromUtf32(std::u32string_view input)
{
    const auto codePoints = Utf32Codec().Decode(input);
    if (codePoints.failed())
    {
        return Result<std::wstring, CodecError>::failure(*codePoints.error());
    }
    return EncodeWide(codePoints.value());
}

}
