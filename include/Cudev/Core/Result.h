#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace Cudev {

enum class ResultStatus
{
    Success,
    Failure,
    Info,
    Warning
};

template <typename Value, typename Error = std::string>
class Result
{
public:
    static Result success(Value value, std::string message = {})
    {
        return Result(ResultStatus::Success, std::move(value), std::nullopt, std::move(message));
    }

    static Result failure(Error error, std::string message = {})
    {
        if constexpr (std::is_same_v<Error, std::string>)
        {
            if (message.empty())
            {
                message = error;
            }
        }

        return Result(ResultStatus::Failure, std::nullopt, std::move(error), std::move(message));
    }

    static Result info(Value value, std::string message = {})
    {
        return Result(ResultStatus::Info, std::move(value), std::nullopt, std::move(message));
    }

    static Result warning(Value value, std::string message = {})
    {
        return Result(ResultStatus::Warning, std::move(value), std::nullopt, std::move(message));
    }

    constexpr ResultStatus status() const noexcept
    {
        return status_;
    }

    constexpr bool succeeded() const noexcept
    {
        return status_ == ResultStatus::Success;
    }

    constexpr bool failed() const noexcept
    {
        return status_ == ResultStatus::Failure;
    }

    const Value& value() const &
    {
        return value_.value();
    }

    Value&& value() &&
    {
        return std::move(value_.value());
    }

    const std::string& message() const noexcept
    {
        return message_;
    }

    const std::optional<Error>& error() const noexcept
    {
        return error_;
    }

private:
    Result(ResultStatus status, std::optional<Value> value, std::optional<Error> error, std::string message)
        : status_(status), value_(std::move(value)), error_(std::move(error)), message_(std::move(message))
    {
    }

    ResultStatus status_;
    std::optional<Value> value_;
    std::optional<Error> error_;
    std::string message_;
};

template <>
class Result<void, std::string>
{
public:
    static Result success(std::string message = {})
    {
        return Result(ResultStatus::Success, std::nullopt, std::move(message));
    }

    static Result failure(std::string error, std::string message = {})
    {
        if (message.empty())
        {
            message = error;
        }

        return Result(ResultStatus::Failure, std::move(error), std::move(message));
    }

    static Result info(std::string message)
    {
        return Result(ResultStatus::Info, std::nullopt, std::move(message));
    }

    static Result warning(std::string message)
    {
        return Result(ResultStatus::Warning, std::nullopt, std::move(message));
    }

    constexpr ResultStatus status() const noexcept
    {
        return status_;
    }

    constexpr bool succeeded() const noexcept
    {
        return status_ == ResultStatus::Success;
    }

    constexpr bool failed() const noexcept
    {
        return status_ == ResultStatus::Failure;
    }

    const std::string& message() const noexcept
    {
        return message_;
    }

    const std::optional<std::string>& error() const noexcept
    {
        return error_;
    }

private:
    Result(ResultStatus status, std::optional<std::string> error, std::string message)
        : status_(status), error_(std::move(error)), message_(std::move(message))
    {
    }

    ResultStatus status_;
    std::optional<std::string> error_;
    std::string message_;
};

template <typename Error>
class Result<void, Error>
{
public:
    static Result success(std::string message = {})
    {
        return Result(ResultStatus::Success, std::nullopt, std::move(message));
    }

    static Result failure(Error error, std::string message = {})
    {
        return Result(ResultStatus::Failure, std::move(error), std::move(message));
    }

    static Result info(std::string message)
    {
        return Result(ResultStatus::Info, std::nullopt, std::move(message));
    }

    static Result warning(std::string message)
    {
        return Result(ResultStatus::Warning, std::nullopt, std::move(message));
    }

    constexpr ResultStatus status() const noexcept
    {
        return status_;
    }

    constexpr bool succeeded() const noexcept
    {
        return status_ == ResultStatus::Success;
    }

    constexpr bool failed() const noexcept
    {
        return status_ == ResultStatus::Failure;
    }

    const std::string& message() const noexcept
    {
        return message_;
    }

    const std::optional<Error>& error() const noexcept
    {
        return error_;
    }

private:
    Result(ResultStatus status, std::optional<Error> error, std::string message)
        : status_(status), error_(std::move(error)), message_(std::move(message))
    {
    }

    ResultStatus status_;
    std::optional<Error> error_;
    std::string message_;
};

}
