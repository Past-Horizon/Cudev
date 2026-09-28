#include <gtest/gtest.h>

#include <Cudev/Utf/8/U8Codec.h>

#include <initializer_list>
#include <string>
#include <vector>

namespace {

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string result;
    result.reserve(values.size());

    for (const auto value : values)
    {
        result.push_back(static_cast<char>(value));
    }

    return result;
}

struct EncodingCase
{
    char32_t codePoint;
    std::string expected;
};

struct InvalidSequence
{
    std::string bytes;
    Cudev::CodecError error;
};

}

TEST(Utf8Tests, EncodesAndDecodesUnicodeScalars)
{
    const Cudev::Utf8::U8Codec codec;
    const std::u32string input = U"A\u00A3\u20AC\U0001F642";

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_EQ(encoded.value(), "A\xC2\xA3\xE2\x82\xAC\xF0\x9F\x99\x82");

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf8Tests, RejectsMalformedSequences)
{
    const Cudev::Utf8::U8Codec codec;
    EXPECT_EQ(codec.Validate("\x80").error(), Cudev::CodecError::InvalidLeadingByte);
    EXPECT_EQ(codec.Validate("\xE2\x28\xA1").error(), Cudev::CodecError::InvalidContinuationByte);
    EXPECT_EQ(codec.Validate("\xF0\x9F\x92").error(), Cudev::CodecError::TruncatedSequence);
    EXPECT_EQ(codec.Validate("\xC0\x80").error(), Cudev::CodecError::OverlongEncoding);
    EXPECT_EQ(codec.Validate("\xED\xA0\x80").error(), Cudev::CodecError::SurrogateCodePoint);
    EXPECT_TRUE(codec.Validate("\xF4\x90\x80\x80").failed());
}

TEST(Utf8Tests, RejectsInvalidCodePointsWhenEncoding)
{
    const Cudev::Utf8::U8Codec codec;
    const auto surrogate = codec.Encode(std::u32string(1, static_cast<char32_t>(0xD800)));
    ASSERT_TRUE(surrogate.failed());
    EXPECT_EQ(surrogate.error(), Cudev::CodecError::SurrogateCodePoint);

    const auto outOfRange = codec.Encode(std::u32string(1, 0x110000));
    ASSERT_TRUE(outOfRange.failed());
    EXPECT_EQ(outOfRange.error(), Cudev::CodecError::CodePointOutOfRange);
}

TEST(Utf8Tests, U8CodecImplementsCodecContract)
{
    const Cudev::Utf8::U8Codec codec;
    const auto encoded = codec.Encode(U"hello");

    ASSERT_TRUE(encoded.succeeded());
    EXPECT_TRUE(codec.Validate(encoded.value()).succeeded());

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), U"hello");
}

TEST(Utf8Tests, AcceptsEmptyInput)
{
    const Cudev::Utf8::U8Codec codec;

    const auto encoded = codec.Encode({});
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_TRUE(encoded.value().empty());

    const auto decoded = codec.Decode({});
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_TRUE(decoded.value().empty());
    EXPECT_TRUE(codec.Validate({}).succeeded());
}

TEST(Utf8Tests, EncodesEverySequenceLengthAtItsBoundaries)
{
    const Cudev::Utf8::U8Codec codec;
    const std::vector<EncodingCase> cases = {
        {0x0000, Bytes({0x00})},
        {0x007F, Bytes({0x7F})},
        {0x0080, Bytes({0xC2, 0x80})},
        {0x07FF, Bytes({0xDF, 0xBF})},
        {0x0800, Bytes({0xE0, 0xA0, 0x80})},
        {0xFFFF, Bytes({0xEF, 0xBF, 0xBF})},
        {0x10000, Bytes({0xF0, 0x90, 0x80, 0x80})},
        {0x10FFFF, Bytes({0xF4, 0x8F, 0xBF, 0xBF})}
    };

    for (const auto& testCase : cases)
    {
        const auto encoded = codec.Encode(std::u32string(1, testCase.codePoint));
        ASSERT_TRUE(encoded.succeeded()) << testCase.codePoint;
        EXPECT_EQ(encoded.value(), testCase.expected) << testCase.codePoint;

        const auto decoded = codec.Decode(testCase.expected);
        ASSERT_TRUE(decoded.succeeded()) << testCase.codePoint;
        EXPECT_EQ(decoded.value(), std::u32string(1, testCase.codePoint));
    }
}

TEST(Utf8Tests, AcceptsRestrictedSecondByteBoundaries)
{
    const Cudev::Utf8::U8Codec codec;
    const std::vector<std::string> validSequences = {
        Bytes({0xE0, 0xA0, 0x80}),
        Bytes({0xED, 0x9F, 0xBF}),
        Bytes({0xF0, 0x90, 0x80, 0x80}),
        Bytes({0xF4, 0x8F, 0xBF, 0xBF})
    };

    for (const auto& sequence : validSequences)
    {
        EXPECT_TRUE(codec.Validate(sequence).succeeded());
        EXPECT_TRUE(codec.Decode(sequence).succeeded());
    }
}

TEST(Utf8Tests, RejectsInvalidContinuationAtEveryPosition)
{
    const Cudev::Utf8::U8Codec codec;
    const std::vector<std::string> validSequences = {
        Bytes({0xC2, 0x80}),
        Bytes({0xE1, 0x80, 0x80}),
        Bytes({0xF1, 0x80, 0x80, 0x80})
    };

    for (const auto& validSequence : validSequences)
    {
        for (std::size_t index = 1; index < validSequence.size(); ++index)
        {
            auto malformed = validSequence;
            malformed[index] = static_cast<char>(0x41);

            const auto validation = codec.Validate(malformed);
            ASSERT_TRUE(validation.failed());
            EXPECT_EQ(validation.error(), Cudev::CodecError::InvalidContinuationByte);

            const auto decoded = codec.Decode(malformed);
            ASSERT_TRUE(decoded.failed());
            EXPECT_EQ(decoded.error(), Cudev::CodecError::InvalidContinuationByte);
        }
    }
}

TEST(Utf8Tests, PreservesEmbeddedNullsAndSequenceAlignment)
{
    const Cudev::Utf8::U8Codec codec;
    const std::u32string input = {
        U'A', 0, 0x007F, 0x0080, 0x0800, 0x10000, U'Z'
    };

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    EXPECT_TRUE(codec.Validate(encoded.value()).succeeded());

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf8Tests, ExhaustivelyRoundTripsUnicodeScalarValues)
{
    const Cudev::Utf8::U8Codec codec;
    std::u32string input;
    input.reserve(0x110000 - 0x800);

    for (char32_t codePoint = 0; codePoint <= 0x10FFFF; ++codePoint)
    {
        if (codePoint < 0xD800 || codePoint > 0xDFFF)
        {
            input.push_back(codePoint);
        }
    }

    const auto encoded = codec.Encode(input);
    ASSERT_TRUE(encoded.succeeded());
    ASSERT_TRUE(codec.Validate(encoded.value()).succeeded());

    const auto decoded = codec.Decode(encoded.value());
    ASSERT_TRUE(decoded.succeeded());
    EXPECT_EQ(decoded.value(), input);
}

TEST(Utf8Tests, RoundTripsDeterministicScalarValues)
{
    const Cudev::Utf8::U8Codec codec;

    for (char32_t codePoint = 0; codePoint <= 0x10FFFF; codePoint += 997)
    {
        if (codePoint >= 0xD800 && codePoint <= 0xDFFF)
        {
            codePoint = 0xE000;
        }

        const std::u32string input(1, codePoint);
        const auto encoded = codec.Encode(input);
        ASSERT_TRUE(encoded.succeeded()) << codePoint;
        EXPECT_TRUE(codec.Validate(encoded.value()).succeeded()) << codePoint;

        const auto decoded = codec.Decode(encoded.value());
        ASSERT_TRUE(decoded.succeeded()) << codePoint;
        EXPECT_EQ(decoded.value(), input) << codePoint;
    }
}

TEST(Utf8Tests, RejectsEveryLoneContinuationByte)
{
    const Cudev::Utf8::U8Codec codec;

    for (unsigned int byte = 0x80; byte <= 0xBF; ++byte)
    {
        const auto result = codec.Validate(Bytes({byte}));
        ASSERT_TRUE(result.failed()) << byte;
        EXPECT_EQ(result.error(), Cudev::CodecError::InvalidLeadingByte) << byte;
    }
}

TEST(Utf8Tests, RejectsInvalidLeadingBytes)
{
    const Cudev::Utf8::U8Codec codec;

    for (unsigned int byte = 0xF5; byte <= 0xFF; ++byte)
    {
        const auto result = codec.Validate(Bytes({byte}));
        ASSERT_TRUE(result.failed()) << byte;
        EXPECT_EQ(result.error(), Cudev::CodecError::InvalidLeadingByte) << byte;
    }
}

TEST(Utf8Tests, RejectsMalformedSequencesWithConsistentErrors)
{
    const Cudev::Utf8::U8Codec codec;
    const std::vector<InvalidSequence> cases = {
        {Bytes({0xC2}), Cudev::CodecError::TruncatedSequence},
        {Bytes({0xE0, 0xA0}), Cudev::CodecError::TruncatedSequence},
        {Bytes({0xF0, 0x90, 0x80}), Cudev::CodecError::TruncatedSequence},
        {Bytes({0xC2, 0x41}), Cudev::CodecError::InvalidContinuationByte},
        {Bytes({0xE0, 0xA0, 0x41}), Cudev::CodecError::InvalidContinuationByte},
        {Bytes({0xF0, 0x90, 0x80, 0x41}), Cudev::CodecError::InvalidContinuationByte},
        {Bytes({0xC0, 0x80}), Cudev::CodecError::OverlongEncoding},
        {Bytes({0xE0, 0x80, 0x80}), Cudev::CodecError::OverlongEncoding},
        {Bytes({0xF0, 0x80, 0x80, 0x80}), Cudev::CodecError::OverlongEncoding},
        {Bytes({0xED, 0xA0, 0x80}), Cudev::CodecError::SurrogateCodePoint},
        {Bytes({0xF4, 0x90, 0x80, 0x80}), Cudev::CodecError::CodePointOutOfRange}
    };

    for (const auto& testCase : cases)
    {
        const auto validation = codec.Validate(testCase.bytes);
        ASSERT_TRUE(validation.failed());
        EXPECT_EQ(validation.error(), testCase.error);

        const auto decoded = codec.Decode(testCase.bytes);
        ASSERT_TRUE(decoded.failed());
        EXPECT_EQ(decoded.error(), testCase.error);
    }
}

TEST(Utf8Tests, DoesNotReturnPartialDecodeForMalformedInput)
{
    const Cudev::Utf8::U8Codec codec;
    const auto decoded = codec.Decode(Bytes({0x41, 0xC2}));

    ASSERT_TRUE(decoded.failed());
    EXPECT_EQ(decoded.error(), Cudev::CodecError::TruncatedSequence);
}

TEST(Utf8Tests, ReportsOverlongBeforeLaterInvalidContinuation)
{
    const Cudev::Utf8::U8Codec codec;
    const auto result = codec.Validate(Bytes({0xE0, 0x80, 0x41}));
    ASSERT_TRUE(result.failed());
    EXPECT_EQ(result.error(), Cudev::CodecError::OverlongEncoding);
}

TEST(Utf8Tests, ReportsSurrogateBeforeLaterInvalidContinuation)
{
    const Cudev::Utf8::U8Codec codec;
    const auto result = codec.Validate(Bytes({0xED, 0xA0, 0x41}));
    ASSERT_TRUE(result.failed());
    EXPECT_EQ(result.error(), Cudev::CodecError::SurrogateCodePoint);
}

TEST(Utf8Tests, ReportsOutOfRangeBeforeLaterInvalidContinuation)
{
    const Cudev::Utf8::U8Codec codec;
    const auto result = codec.Validate(Bytes({0xF4, 0x90, 0x80, 0x41}));
    ASSERT_TRUE(result.failed());
    EXPECT_EQ(result.error(), Cudev::CodecError::CodePointOutOfRange);
}

TEST(Utf8Tests, ReportsF0OverlongBeforeLaterInvalidContinuation)
{
    const Cudev::Utf8::U8Codec codec;
    const auto result = codec.Validate(Bytes({0xF0, 0x80, 0x80, 0x41}));
    ASSERT_TRUE(result.failed());
    EXPECT_EQ(result.error(), Cudev::CodecError::OverlongEncoding);
}