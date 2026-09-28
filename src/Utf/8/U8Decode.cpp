#include <Cudev/Utf/8/U8Codec.h>

#include <Cudev/Utf/Common/ScalarValue.h>

#include <cstddef>

namespace {

Cudev::Result<char32_t, Cudev::CodecError> DecodeNext(
    std::string_view bytes,
    std::size_t& offset)
{
    const auto firstByte = static_cast<unsigned char>(bytes[offset]);
    std::size_t sequenceLength = 0;
    char32_t codePoint = 0;
    char32_t minimumCodePoint = 0;

    if (firstByte <= 0x7F)
    {
        ++offset;
        return Cudev::Result<char32_t, Cudev::CodecError>::success(firstByte);
    }

    if (firstByte >= 0xC2 && firstByte <= 0xDF)
    {
        sequenceLength = 2;
        codePoint = firstByte & 0x1F;
        minimumCodePoint = 0x80;
    }
    else if (firstByte >= 0xE0 && firstByte <= 0xEF)
    {
        sequenceLength = 3;
        codePoint = firstByte & 0x0F;
        minimumCodePoint = 0x800;
    }
    else if (firstByte >= 0xF0 && firstByte <= 0xF4)
    {
        sequenceLength = 4;
        codePoint = firstByte & 0x07;
        minimumCodePoint = 0x10000;
    }
    else if (firstByte == 0xC0 || firstByte == 0xC1)
    {
        return Cudev::Result<char32_t, Cudev::CodecError>::failure(
            Cudev::CodecError::OverlongEncoding);
    }
    else
    {
        return Cudev::Result<char32_t, Cudev::CodecError>::failure(
            Cudev::CodecError::InvalidLeadingByte);
    }

    if (offset + sequenceLength > bytes.size())
    {
        return Cudev::Result<char32_t, Cudev::CodecError>::failure(
            Cudev::CodecError::TruncatedSequence);
    }

    const auto secondByte = static_cast<unsigned char>(bytes[offset + 1]);
    if ((secondByte & 0xC0) == 0x80)
    {
        if (firstByte == 0xE0 && secondByte < 0xA0)
        {
            return Cudev::Result<char32_t, Cudev::CodecError>::failure(
                Cudev::CodecError::OverlongEncoding);
        }

        if (firstByte == 0xF0 && secondByte < 0x90)
        {
            return Cudev::Result<char32_t, Cudev::CodecError>::failure(
                Cudev::CodecError::OverlongEncoding);
        }

        if (firstByte == 0xED && secondByte >= 0xA0)
        {
            return Cudev::Result<char32_t, Cudev::CodecError>::failure(
                Cudev::CodecError::SurrogateCodePoint);
        }

        if (firstByte == 0xF4 && secondByte >= 0x90)
        {
            return Cudev::Result<char32_t, Cudev::CodecError>::failure(
                Cudev::CodecError::CodePointOutOfRange);
        }
    }

    for (std::size_t index = 1; index < sequenceLength; ++index)
    {
        const auto continuationByte = static_cast<unsigned char>(bytes[offset + index]);
        if ((continuationByte & 0xC0) != 0x80)
        {
            return Cudev::Result<char32_t, Cudev::CodecError>::failure(
                Cudev::CodecError::InvalidContinuationByte);
        }

        codePoint = (codePoint << 6) | (continuationByte & 0x3F);
    }

    offset += sequenceLength;

    if (codePoint < minimumCodePoint)
    {
        return Cudev::Result<char32_t, Cudev::CodecError>::failure(
            Cudev::CodecError::OverlongEncoding);
    }

    if (codePoint >= 0xD800 && codePoint <= 0xDFFF)
    {
        return Cudev::Result<char32_t, Cudev::CodecError>::failure(
            Cudev::CodecError::SurrogateCodePoint);
    }

    if (!Cudev::Utf::IsScalarValue(codePoint))
    {
        return Cudev::Result<char32_t, Cudev::CodecError>::failure(
            Cudev::CodecError::CodePointOutOfRange);
    }

    return Cudev::Result<char32_t, Cudev::CodecError>::success(codePoint);
}

}

namespace Cudev::Utf8 {

Result<std::u32string, CodecError> U8Codec::Decode(std::string_view input) const
{
    std::u32string codePoints;
    codePoints.reserve(input.size());

    std::size_t offset = 0;
    while (offset < input.size())
    {
        auto codePoint = DecodeNext(input, offset);
        if (codePoint.failed())
        {
            return Result<std::u32string, CodecError>::failure(*codePoint.error());
        }

        codePoints.push_back(codePoint.value());
    }

    return Result<std::u32string, CodecError>::success(std::move(codePoints));
}

Result<void, CodecError> U8Codec::Validate(std::string_view input) const
{
    std::size_t offset = 0;
    while (offset < input.size())
    {
        auto codePoint = DecodeNext(input, offset);
        if (codePoint.failed())
        {
            return Result<void, CodecError>::failure(*codePoint.error());
        }
    }

    return Result<void, CodecError>::success();
}

}