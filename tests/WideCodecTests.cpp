#include <gtest/gtest.h>

#include <Cudev/Wide/WideCodec.h>

#include <climits>
#include <string>

TEST(WideCodecTests, ConvertsWideTextToEveryUtfEncoding)
{
    const std::wstring input = L"A\u00A3\u20AC\U0001F642";
    const std::string expectedUtf8 = "A\xC2\xA3\xE2\x82\xAC\xF0\x9F\x99\x82";

    const auto utf8 = Cudev::Wide::ToUtf8(input);
    ASSERT_TRUE(utf8.succeeded());
    EXPECT_EQ(utf8.value(), expectedUtf8);

    const auto utf16 = Cudev::Wide::ToUtf16(input);
    ASSERT_TRUE(utf16.succeeded());

    const auto utf32 = Cudev::Wide::ToUtf32(input);
    ASSERT_TRUE(utf32.succeeded());
    EXPECT_EQ(utf32.value(), U"A\u00A3\u20AC\U0001F642");
}

TEST(WideCodecTests, ConvertsEveryUtfEncodingToWideText)
{
    const std::string utf8 = "A\xC2\xA3\xE2\x82\xAC\xF0\x9F\x99\x82";
    const std::u16string utf16 = {
        u'A', static_cast<char16_t>(0x00A3), static_cast<char16_t>(0x20AC),
        static_cast<char16_t>(0xD83D), static_cast<char16_t>(0xDE42)};
    const std::u32string utf32 = U"A\u00A3\u20AC\U0001F642";

    const auto fromUtf8 = Cudev::Wide::FromUtf8(utf8);
    ASSERT_TRUE(fromUtf8.succeeded());
    EXPECT_EQ(fromUtf8.value(), L"A\u00A3\u20AC\U0001F642");

    const auto fromUtf16 = Cudev::Wide::FromUtf16(utf16);
    ASSERT_TRUE(fromUtf16.succeeded());
    EXPECT_EQ(fromUtf16.value(), L"A\u00A3\u20AC\U0001F642");

    const auto fromUtf32 = Cudev::Wide::FromUtf32(utf32);
    ASSERT_TRUE(fromUtf32.succeeded());
    EXPECT_EQ(fromUtf32.value(), L"A\u00A3\u20AC\U0001F642");
}

TEST(WideCodecTests, PreservesEmbeddedNulls)
{
    const std::wstring input = {L'A', L'\0', L'Z'};

    const auto utf8 = Cudev::Wide::ToUtf8(input);
    ASSERT_TRUE(utf8.succeeded());
    ASSERT_EQ(utf8.value().size(), input.size());

    const auto output = Cudev::Wide::FromUtf8(utf8.value());
    ASSERT_TRUE(output.succeeded());
    EXPECT_EQ(output.value(), input);
}

TEST(WideCodecTests, RejectsInvalidWideSequences)
{
    const std::wstring invalid = {
        static_cast<wchar_t>(0xD800)};

    const auto utf8 = Cudev::Wide::ToUtf8(invalid);
    ASSERT_TRUE(utf8.failed());
#if WCHAR_MAX <= 0xFFFF
    EXPECT_EQ(utf8.error(), Cudev::CodecError::TruncatedSequence);
#else
    EXPECT_EQ(utf8.error(), Cudev::CodecError::SurrogateCodePoint);
#endif
}
