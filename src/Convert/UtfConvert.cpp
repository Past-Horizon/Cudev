#include <Cudev/Convert/UtfConvert.h>

#include <Cudev/Utf/16/U16Codec.h>
#include <Cudev/Utf/32/U32Codec.h>
#include <Cudev/Utf/8/U8Codec.h>

namespace Cudev::Convert {

namespace {

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

template <typename Output>
Result<Output, CodecError> EncodeDecoded(
    Result<std::u32string, CodecError> decoded,
    const auto& codec)
{
    if (decoded.failed())
    {
        return Result<Output, CodecError>::failure(*decoded.error());
    }

    return codec.Encode(decoded.value());
}

}

Result<std::string, CodecError> ToUtf8(std::u16string_view input)
{
    return EncodeDecoded<std::string>(Utf16Codec().Decode(input), Utf8Codec());
}

Result<std::string, CodecError> ToUtf8(std::u32string_view input)
{
    return Utf8Codec().Encode(input);
}

Result<std::u16string, CodecError> ToUtf16(std::string_view input)
{
    return EncodeDecoded<std::u16string>(Utf8Codec().Decode(input), Utf16Codec());
}

Result<std::u16string, CodecError> ToUtf16(std::u32string_view input)
{
    return Utf16Codec().Encode(input);
}

Result<std::u32string, CodecError> ToUtf32(std::string_view input)
{
    return Utf8Codec().Decode(input);
}

Result<std::u32string, CodecError> ToUtf32(std::u16string_view input)
{
    return Utf16Codec().Decode(input);
}

}
