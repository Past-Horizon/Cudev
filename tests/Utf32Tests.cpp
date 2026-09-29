#include <gtest/gtest.h>

#include <Cudev/Utf/32/U32Codec.h>

#include <vector>

TEST(Utf32Tests, EncodesAndDecodesUnicodeScalars)
{
    const Cudev::Utf32::U32Codec codec;
    const std::u32string input = U"A\u00A3\u20AC\U0001F642";

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_EQ(encoded.value(), input);

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf32Tests, PreservesEmptyInputAndEmbeddedNulls)
{
    const Cudev::Utf32::U32Codec codec;
    const std::u32string input = {U'A', 0, 0x10000, U'Z'};

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_EQ(encoded.value(), input);

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf32Tests, AcceptsUnicodeScalarBoundaries)
{
    const Cudev::Utf32::U32Codec codec;
    const std::vector<char32_t> codePoints = {
        0x0000, 0x007F, 0x0080, 0xD7FF, 0xE000, 0xFFFF,
        0x10000, 0x10FFFF};

    for (const char32_t codePoint : codePoints)
    {
        const std::u32string input(1, codePoint);
        const auto encoded = codec.Encode(input);
        ASSERT_TRUE(encoded.succeeded()) << codePoint;
        EXPECT_EQ(encoded.value(), input) << codePoint;
        EXPECT_TRUE(codec.Validate(encoded.value()).succeeded()) << codePoint;

        const auto decoded = codec.Decode(encoded.value());
        ASSERT_TRUE(decoded.succeeded()) << codePoint;
        EXPECT_EQ(decoded.value(), input) << codePoint;
    }
}

TEST(Utf32Tests, RejectsInvalidScalarValues)
{
    const Cudev::Utf32::U32Codec codec;
    const std::vector<std::pair<char32_t, Cudev::CodecError>> cases = {
        {static_cast<char32_t>(0xD800), Cudev::CodecError::SurrogateCodePoint},
        {static_cast<char32_t>(0xDFFF), Cudev::CodecError::SurrogateCodePoint},
        {static_cast<char32_t>(0x110000), Cudev::CodecError::CodePointOutOfRange}};

    for (const auto& testCase : cases)
    {
        const std::u32string input(1, testCase.first);

        const auto encoded = codec.Encode(input);
        ASSERT_TRUE(encoded.failed()) << testCase.first;
        EXPECT_EQ(encoded.error(), testCase.second) << testCase.first;

        const auto validation = codec.Validate(input);
        ASSERT_TRUE(validation.failed()) << testCase.first;
        EXPECT_EQ(validation.error(), testCase.second) << testCase.first;

        const auto decoded = codec.Decode(input);
        ASSERT_TRUE(decoded.failed()) << testCase.first;
        EXPECT_EQ(decoded.error(), testCase.second) << testCase.first;
    }
}

TEST(Utf32Tests, DoesNotReturnPartialDecodeForInvalidInput)
{
    const Cudev::Utf32::U32Codec codec;
    const std::u32string input = {U'A', 0x110000, U'Z'};

    const auto decoded = codec.Decode(input);
    ASSERT_TRUE(decoded.failed());
    EXPECT_EQ(decoded.error(), Cudev::CodecError::CodePointOutOfRange);
}
