#pragma once

#include <Cudev/Utf/ICodec.h>

#include <string>
#include <string_view>

namespace Cudev::Utf8 {

class U8Codec final
    : public Utf::ICodec<
          std::string,
          std::string_view,
          std::u32string,
          std::u32string_view>
{
public:
    Result<std::string, CodecError> Encode(std::u32string_view input) const override;
    Result<std::u32string, CodecError> Decode(std::string_view input) const override;
    Result<void, CodecError> Validate(std::string_view input) const override;
};

}