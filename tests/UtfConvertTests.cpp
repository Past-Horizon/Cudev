#include <gtest/gtest.h>

#include <Cudev/Convert/UtfConvert.h>

TEST(UtfConvertTests, ConvertsEveryFixedWidthDirection)
{
    const std::string utf8 = "A\xC2\xA3\xE2\x82\xAC\xF0\x9F\x99\x82";
    const std::u16string utf16 = {
        u'A', static_cast<char16_t>(0x00A3), static_cast<char16_t>(0x20AC),
        static_cast<char16_t>(0xD83D), static_cast<char16_t>(0xDE42)};
    const std::u32string utf32 = U"A\u00A3\u20AC\U0001F642";

    const auto utf8To16 = Cudev::Convert::ToUtf16(utf8);
    ASSERT_TRUE(utf8To16.succeeded());
    EXPECT_EQ(utf8To16.value(), utf16);

    const auto utf8To32 = Cudev::Convert::ToUtf32(utf8);
    ASSERT_TRUE(utf8To32.succeeded());
    EXPECT_EQ(utf8To32.value(), utf32);

    const auto utf16To8 = Cudev::Convert::ToUtf8(utf16);
    ASSERT_TRUE(utf16To8.succeeded());
    EXPECT_EQ(utf16To8.value(), utf8);

    const auto utf16To32 = Cudev::Convert::ToUtf32(utf16);
    ASSERT_TRUE(utf16To32.succeeded());
    EXPECT_EQ(utf16To32.value(), utf32);

    const auto utf32To8 = Cudev::Convert::ToUtf8(utf32);
    ASSERT_TRUE(utf32To8.succeeded());
    EXPECT_EQ(utf32To8.value(), utf8);

    const auto utf32To16 = Cudev::Convert::ToUtf16(utf32);
    ASSERT_TRUE(utf32To16.succeeded());
    EXPECT_EQ(utf32To16.value(), utf16);
}

TEST(UtfConvertTests, PreservesEmbeddedNulls)
{
    const std::u32string input = {U'A', 0, 0x10000, U'Z'};

    const auto utf16 = Cudev::Convert::ToUtf16(input);
    ASSERT_TRUE(utf16.succeeded());

    const auto utf8 = Cudev::Convert::ToUtf8(utf16.value());
    ASSERT_TRUE(utf8.succeeded());

    const auto output = Cudev::Convert::ToUtf32(utf8.value());
    ASSERT_TRUE(output.succeeded());
    EXPECT_EQ(output.value(), input);
}

TEST(UtfConvertTests, PropagatesMalformedInputErrors)
{
    const auto malformedUtf8 = Cudev::Convert::ToUtf16("\xF0\x9F\x92");
    ASSERT_TRUE(malformedUtf8.failed());
    EXPECT_EQ(malformedUtf8.error(), Cudev::CodecError::TruncatedSequence);

    const std::u16string malformedUtf16 = {
        static_cast<char16_t>(0xD800)};
    const auto utf32 = Cudev::Convert::ToUtf32(malformedUtf16);
    ASSERT_TRUE(utf32.failed());
    EXPECT_EQ(utf32.error(), Cudev::CodecError::TruncatedSequence);

    const std::u32string invalidUtf32 = {
        static_cast<char32_t>(0xD800)};
    const auto utf8 = Cudev::Convert::ToUtf8(invalidUtf32);
    ASSERT_TRUE(utf8.failed());
    EXPECT_EQ(utf8.error(), Cudev::CodecError::SurrogateCodePoint);
}
