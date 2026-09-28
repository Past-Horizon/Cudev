#include <gtest/gtest.h>

#include <Cudev/Utf/16/U16Codec.h>

#include <utility>
#include <vector>

TEST(Utf16Tests, EncodesAndDecodesUnicodeScalars)
{
    const Cudev::Utf16::U16Codec codec;
    const std::u32string input = U"A\u00A3\u20AC\U0001F642";

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_EQ(encoded.value(), (std::u16string{
        u'A', u'\u00A3', u'\u20AC',
        static_cast<char16_t>(0xD83D),
        static_cast<char16_t>(0xDE42)}));

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf16Tests, EncodesAstralBoundaryValues)
{
    const Cudev::Utf16::U16Codec codec;
    const std::vector<std::pair<char32_t, std::u16string>> cases = {
        {0x10000, {
            static_cast<char16_t>(0xD800),
            static_cast<char16_t>(0xDC00)}},
        {0x10FFFF, {
            static_cast<char16_t>(0xDBFF),
            static_cast<char16_t>(0xDFFF)}}
    };

    for (const auto& testCase : cases)
    {
        const auto encoded = codec.Encode(std::u32string(1, testCase.first));
        ASSERT_TRUE(encoded.succeeded());
        EXPECT_EQ(encoded.value(), testCase.second);
    }
}

TEST(Utf16Tests, PreservesBmpValuesAroundSurrogateRange)
{
    const Cudev::Utf16::U16Codec codec;
    const std::u32string input = {
        0xD7FF, 0xE000, 0xFFFE, 0xFFFF};

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    ASSERT_EQ(encoded.value().size(), input.size());
    EXPECT_EQ(encoded.value()[0], static_cast<char16_t>(0xD7FF));
    EXPECT_EQ(encoded.value()[1], static_cast<char16_t>(0xE000));
    EXPECT_EQ(encoded.value()[2], static_cast<char16_t>(0xFFFE));
    EXPECT_EQ(encoded.value()[3], static_cast<char16_t>(0xFFFF));

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf16Tests, EncodesSurrogatePairMathAtInternalBoundaries)
{
    const Cudev::Utf16::U16Codec codec;
    const std::vector<std::pair<char32_t, std::u16string>> cases = {
        {0x10001, {
            static_cast<char16_t>(0xD800),
            static_cast<char16_t>(0xDC01)}},
        {0x103FF, {
            static_cast<char16_t>(0xD800),
            static_cast<char16_t>(0xDFFF)}},
        {0x10400, {
            static_cast<char16_t>(0xD801),
            static_cast<char16_t>(0xDC00)}},
        {0x10FFFE, {
            static_cast<char16_t>(0xDBFF),
            static_cast<char16_t>(0xDFFE)}}
    };

    for (const auto& testCase : cases)
    {
        const auto encoded = codec.Encode(std::u32string(1, testCase.first));
        ASSERT_TRUE(encoded.succeeded()) << testCase.first;
        EXPECT_EQ(encoded.value(), testCase.second) << testCase.first;

        const auto decoded = codec.Decode(testCase.second);
        ASSERT_TRUE(decoded.succeeded()) << testCase.first;
        EXPECT_EQ(decoded.value(), std::u32string(1, testCase.first));
    }
}

TEST(Utf16Tests, HandlesMultiplePairsAndBmpValuesWithoutLosingAlignment)
{
    const Cudev::Utf16::U16Codec codec;
    const std::u32string input = {
        U'A', 0x10000, 0x10FFFF, 0, 0x1F642, 0xE000, U'Z'};

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_TRUE(codec.Validate(encoded.value()).succeeded());

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf16Tests, AcceptsEmptyInputAndEmbeddedNulls)
{
    const Cudev::Utf16::U16Codec codec;
    const std::u32string input = {U'A', 0, U'B'};

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_TRUE(codec.Validate(encoded.value()).succeeded());

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf16Tests, RejectsInvalidScalarsWhenEncoding)
{
    const Cudev::Utf16::U16Codec codec;

    const auto surrogate = codec.Encode(std::u32string(1, static_cast<char32_t>(0xD800)));
    ASSERT_TRUE(surrogate.failed());
    EXPECT_EQ(surrogate.error(), Cudev::CodecError::SurrogateCodePoint);

    const auto outOfRange = codec.Encode(std::u32string(1, 0x110000));
    ASSERT_TRUE(outOfRange.failed());
    EXPECT_EQ(outOfRange.error(), Cudev::CodecError::CodePointOutOfRange);
}

TEST(Utf16Tests, RejectsMalformedSurrogateSequences)
{
    const Cudev::Utf16::U16Codec codec;
    const std::vector<std::pair<std::u16string, Cudev::CodecError>> cases = {
        {{static_cast<char16_t>(0xD800)}, Cudev::CodecError::TruncatedSequence},
        {{static_cast<char16_t>(0xDC00)}, Cudev::CodecError::SurrogateCodePoint},
        {{static_cast<char16_t>(0xD800), u'A'}, Cudev::CodecError::SurrogateCodePoint},
        {{static_cast<char16_t>(0xD800), static_cast<char16_t>(0xD800)},
            Cudev::CodecError::SurrogateCodePoint},
        {{u'A', static_cast<char16_t>(0xDC00)}, Cudev::CodecError::SurrogateCodePoint}
    };

    for (const auto& testCase : cases)
    {
        const auto validation = codec.Validate(testCase.first);
        ASSERT_TRUE(validation.failed());
        EXPECT_EQ(validation.error(), testCase.second);

        const auto decoded = codec.Decode(testCase.first);
        ASSERT_TRUE(decoded.failed());
        EXPECT_EQ(decoded.error(), testCase.second);
    }
}

TEST(Utf16Tests, RejectsMalformedPairsAfterValidInput)
{
    const Cudev::Utf16::U16Codec codec;
    const std::vector<std::u16string> cases = {
        {u'A', static_cast<char16_t>(0xD800)},
        {u'A', static_cast<char16_t>(0xDC00)},
        {static_cast<char16_t>(0xD800), u'A', u'B'},
        {static_cast<char16_t>(0xD800), static_cast<char16_t>(0xDC00),
            static_cast<char16_t>(0xDC00)}
    };

    for (const auto& input : cases)
    {
        const auto validation = codec.Validate(input);
        ASSERT_TRUE(validation.failed());

        const auto decoded = codec.Decode(input);
        ASSERT_TRUE(decoded.failed());
        EXPECT_EQ(decoded.error(), validation.error());
    }
}

TEST(Utf16Tests, RoundTripsDeterministicUnicodeScalars)
{
    const Cudev::Utf16::U16Codec codec;

    for (char32_t codePoint = 0; codePoint <= 0x10FFFF; codePoint += 719)
    {
        if (codePoint >= 0xD800 && codePoint <= 0xDFFF)
        {
            codePoint = 0xE000;
        }

        const std::u32string input(1, codePoint);
        const auto encoded = codec.Encode(input);
        ASSERT_TRUE(encoded.succeeded()) << codePoint;
        ASSERT_TRUE(codec.Validate(encoded.value()).succeeded()) << codePoint;

        const auto decoded = codec.Decode(encoded.value());
        ASSERT_TRUE(decoded.succeeded()) << codePoint;
        EXPECT_EQ(decoded.value(), input) << codePoint;
    }
}

TEST(Utf16Tests, ReportsTruncationForHighSurrogateAfterValidPair)
{
    const Cudev::Utf16::U16Codec codec;
    const std::u16string input{
        static_cast<char16_t>(0xD800), static_cast<char16_t>(0xDC00),
        static_cast<char16_t>(0xD801)};

    const auto validation = codec.Validate(input);
    ASSERT_TRUE(validation.failed());
    EXPECT_EQ(validation.error(), Cudev::CodecError::TruncatedSequence);

    const auto decoded = codec.Decode(input);
    ASSERT_TRUE(decoded.failed());
    EXPECT_EQ(decoded.error(), Cudev::CodecError::TruncatedSequence);
}

TEST(Utf16Tests, DecodesEveryNonSurrogateCodeUnit)
{
    const Cudev::Utf16::U16Codec codec;
    std::u16string input;
    std::u32string expected;
    input.reserve(0x10000 - 0x800);
    expected.reserve(0x10000 - 0x800);

    for (char32_t codeUnit = 0; codeUnit <= 0xFFFF; ++codeUnit)
    {
        if (codeUnit >= 0xD800 && codeUnit <= 0xDFFF)
        {
            continue;
        }

        input.push_back(static_cast<char16_t>(codeUnit));
        expected.push_back(codeUnit);
    }

    ASSERT_TRUE(codec.Validate(input).succeeded());
    const auto decoded = codec.Decode(input);
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), expected);
}