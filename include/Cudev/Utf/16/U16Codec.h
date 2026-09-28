#pragma once

#include <Cudev/Utf/ICodec.h>

#include <string>
#include <string_view>

namespace Cudev::Utf16 {

class U16Codec final
    : public Utf::ICodec<
          std::u16string,
          std::u16string_view,
          std::u32string,
          std::u32string_view>
{
public:
    Result<std::u16string, CodecError> Encode(std::u32string_view input) const override;
    Result<std::u32string, CodecError> Decode(std::u16string_view input) const override;
    Result<void, CodecError> Validate(std::u16string_view input) const override;
};

}